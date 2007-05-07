//
// Process_WIN32.cpp
//
// $Id: //poco/Main/Foundation/src/Process_WIN32.cpp#19 $
//
// Library: Foundation
// Package: Processes
// Module:  Process
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "Poco/Process_WIN32.h"
#include "Poco/Exception.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NamedEvent.h"
#include "Poco/Pipe.h"


namespace Poco {


//
// ProcessHandleImpl
//
ProcessHandleImpl::ProcessHandleImpl(HANDLE hProcess, UInt32 pid):
	_hProcess(hProcess),
	_pid(pid)
{
}


ProcessHandleImpl::~ProcessHandleImpl()
{
	CloseHandle(_hProcess);
}


UInt32 ProcessHandleImpl::id() const
{
	return _pid;
}


int ProcessHandleImpl::wait() const
{
	DWORD rc = WaitForSingleObject(_hProcess, INFINITE);
	if (rc != WAIT_OBJECT_0)
		throw SystemException("Wait failed for process", NumberFormatter::format(_pid));

	DWORD exitCode;
	if (GetExitCodeProcess(_hProcess, &exitCode) == 0)
		throw SystemException("Cannot get exit code for process", NumberFormatter::format(_pid));

	return exitCode;
}


//
// ProcessImpl
//
ProcessImpl::PIDImpl ProcessImpl::idImpl()
{
	return GetCurrentProcessId(); 
}


void ProcessImpl::timesImpl(long& userTime, long& kernelTime)
{
	FILETIME ftCreation;
	FILETIME ftExit;
	FILETIME ftKernel;
	FILETIME ftUser;

	if (GetProcessTimes(GetCurrentProcess(), &ftCreation, &ftExit, &ftKernel, &ftUser) != 0)
	{
		ULARGE_INTEGER time;
		time.LowPart  = ftKernel.dwLowDateTime;
		time.HighPart = ftKernel.dwHighDateTime;
		kernelTime    = long(time.QuadPart/10000000L);
		time.LowPart  = ftUser.dwLowDateTime;
		time.HighPart = ftUser.dwHighDateTime;
		userTime      = long(time.QuadPart/10000000L);
	}
	else
	{
		userTime = kernelTime = -1;
	}
}


ProcessHandleImpl* ProcessImpl::launchImpl(const std::string& command, const ArgsImpl& args, Pipe* inPipe, Pipe* outPipe, Pipe* errPipe)
{
	std::string commandLine = command;
	for (ArgsImpl::const_iterator it = args.begin(); it != args.end(); ++it)
	{
		commandLine.append(" ");
		commandLine.append(*it);
	}		

	STARTUPINFO startupInfo;
	GetStartupInfo(&startupInfo); // take defaults from current process
	startupInfo.cb          = sizeof(STARTUPINFO);
	startupInfo.lpReserved  = NULL;
	startupInfo.lpDesktop   = NULL;
	startupInfo.lpTitle     = NULL;
	startupInfo.dwFlags     = STARTF_FORCEOFFFEEDBACK | STARTF_USESTDHANDLES;
	startupInfo.cbReserved2 = 0;
	startupInfo.lpReserved2 = NULL;
	
	HANDLE hProc = GetCurrentProcess();
	if (inPipe)
	{
		DuplicateHandle(hProc, inPipe->readHandle(), hProc, &startupInfo.hStdInput, 0, TRUE, DUPLICATE_SAME_ACCESS);
		inPipe->close(Pipe::CLOSE_READ);
	}
	else DuplicateHandle(hProc, GetStdHandle(STD_INPUT_HANDLE), hProc, &startupInfo.hStdInput, 0, TRUE, DUPLICATE_SAME_ACCESS);
	// outPipe may be the same as errPipe, so we duplicate first and close later.
	if (outPipe)
		DuplicateHandle(hProc, outPipe->writeHandle(), hProc, &startupInfo.hStdOutput, 0, TRUE, DUPLICATE_SAME_ACCESS);
	else
		DuplicateHandle(hProc, GetStdHandle(STD_OUTPUT_HANDLE), hProc, &startupInfo.hStdOutput, 0, TRUE, DUPLICATE_SAME_ACCESS);
	if (errPipe)
		DuplicateHandle(hProc, errPipe->writeHandle(), hProc, &startupInfo.hStdError, 0, TRUE, DUPLICATE_SAME_ACCESS);
	else
		DuplicateHandle(hProc, GetStdHandle(STD_ERROR_HANDLE), hProc, &startupInfo.hStdError, 0, TRUE, DUPLICATE_SAME_ACCESS);
	if (outPipe) outPipe->close(Pipe::CLOSE_WRITE);
	if (errPipe) errPipe->close(Pipe::CLOSE_WRITE);

	PROCESS_INFORMATION processInfo;
	BOOL rc = CreateProcessA(
		NULL, 
		const_cast<char*>(commandLine.c_str()), 
		NULL, 
		NULL, 
		TRUE, 
		0, 
		NULL, 
		NULL, 
		&startupInfo, 
		&processInfo
	);
	CloseHandle(startupInfo.hStdInput);
	CloseHandle(startupInfo.hStdOutput);
	CloseHandle(startupInfo.hStdError);
	if (rc)
	{
		CloseHandle(processInfo.hThread);
		return new ProcessHandleImpl(processInfo.hProcess, processInfo.dwProcessId);
	}
	else throw SystemException("Cannot launch process", command);
}


void ProcessImpl::killImpl(PIDImpl pid)
{
	HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
	if (hProc)
	{
		if (TerminateProcess(hProc, 0) == 0)
		{
			CloseHandle(hProc);
			throw SystemException("cannot kill process");
		}
		CloseHandle(hProc);
	}
	else
	{
		switch (GetLastError())
		{
		case ERROR_ACCESS_DENIED:
			throw NoPermissionException("cannot kill process");
		case ERROR_NOT_FOUND: 
			throw NotFoundException("cannot kill process");
		default:
			throw SystemException("cannot kill process");
		}
	}
}


void ProcessImpl::requestTerminationImpl(PIDImpl pid)
{
	std::string evName("POCOTRM");
	evName.append(NumberFormatter::formatHex(pid, 8));
	NamedEvent ev(evName);
	ev.set();
}


} // namespace Poco

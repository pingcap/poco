//
// HTTPCredentialsTest.cpp
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "HTTPCredentialsTest.h"
#include "Poco/CppUnit/TestCaller.h"
#include "Poco/CppUnit/TestSuite.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/Net/NetException.h"


using Poco::Net::HTTPRequest;
using Poco::Net::HTTPBasicCredentials;
using Poco::Net::NotAuthenticatedException;


HTTPCredentialsTest::HTTPCredentialsTest(const std::string& name): CppUnit::TestCase(name)
{
}


HTTPCredentialsTest::~HTTPCredentialsTest()
{
}


void HTTPCredentialsTest::testBasicCredentials()
{
	HTTPRequest request;
	assert (!request.hasCredentials());
	
	HTTPBasicCredentials cred("user", "secret");
	cred.authenticate(request);
	assert (request.hasCredentials());
	std::string scheme;
	std::string info;
	request.getCredentials(scheme, info);
	assert (scheme == "Basic");
	assert (info == "dXNlcjpzZWNyZXQ=");
	
	HTTPBasicCredentials cred2(request);
	assert (cred2.getUsername() == "user");
	assert (cred2.getPassword() == "secret");
}


void HTTPCredentialsTest::testProxyBasicCredentials()
{
	HTTPRequest request;
	assert (!request.hasProxyCredentials());
	
	HTTPBasicCredentials cred("user", "secret");
	cred.proxyAuthenticate(request);
	assert (request.hasProxyCredentials());
	std::string scheme;
	std::string info;
	request.getProxyCredentials(scheme, info);
	assert (scheme == "Basic");
	assert (info == "dXNlcjpzZWNyZXQ=");
}


void HTTPCredentialsTest::testBadCredentials()
{
	HTTPRequest request;
	
	std::string scheme;
	std::string info;
	try
	{
		request.getCredentials(scheme, info);
		fail("no credentials - must throw");
	}
	catch (NotAuthenticatedException&)
	{
	}
	
	request.setCredentials("Test", "SomeData");
	request.getCredentials(scheme, info);
	assert (scheme == "Test");
	assert (info == "SomeData");
	
	try
	{
		HTTPBasicCredentials cred(request);
		fail("bad scheme - must throw");
	}
	catch (NotAuthenticatedException&)
	{
	}
}


void HTTPCredentialsTest::setUp()
{
}


void HTTPCredentialsTest::tearDown()
{
}


CppUnit::Test* HTTPCredentialsTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("HTTPCredentialsTest");

	CppUnit_addTest(pSuite, HTTPCredentialsTest, testBasicCredentials);
	CppUnit_addTest(pSuite, HTTPCredentialsTest, testProxyBasicCredentials);
	CppUnit_addTest(pSuite, HTTPCredentialsTest, testBadCredentials);

	return pSuite;
}

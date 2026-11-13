//
// MailTestSuite.cpp
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "MailTestSuite.h"
#include "MailMessageTest.h"
#include "MailStreamTest.h"


CppUnit::Test* MailTestSuite::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("MailTestSuite");

	pSuite->addTest(MailMessageTest::suite());
	pSuite->addTest(MailStreamTest::suite());

	return pSuite;
}

//
// MessageHeaderTest.cpp
//
// Copyright (c) 2005-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "MessageHeaderTest.h"
#include "CppUnit/TestCaller.h"
#include "CppUnit/TestSuite.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/Net/NetException.h"
#include <sstream>


using Poco::Net::MessageHeader;
using Poco::Net::NameValueCollection;
using Poco::Net::MessageException;


MessageHeaderTest::MessageHeaderTest(const std::string& name): CppUnit::TestCase(name)
{
}


MessageHeaderTest::~MessageHeaderTest()
{
}


void MessageHeaderTest::testWrite()
{
	MessageHeader mh;
	mh.set("name1", "value1");
	mh.set("name2", "value2");
	mh.set("name3", "value3");
	
	std::ostringstream ostr;
	mh.write(ostr);
	std::string s = ostr.str();
	assert (s == "name1: value1\r\nname2: value2\r\nname3: value3\r\n");
}


void MessageHeaderTest::testRead1()
{
	std::string s("name1: value1\r\nname2: value2\r\nname3: value3\r\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 3);
	assert (mh["name1"] == "value1");
	assert (mh["name2"] == "value2");
	assert (mh["name3"] == "value3");
}


void MessageHeaderTest::testRead2()
{
	std::string s("name1: value1\nname2: value2\nname3: value3\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 3);
	assert (mh["name1"] == "value1");
	assert (mh["name2"] == "value2");
	assert (mh["name3"] == "value3");
}


void MessageHeaderTest::testRead3()
{
	std::string s("name1: value1\r\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 1);
	assert (mh["name1"] == "value1");
}



void MessageHeaderTest::testRead4()
{
	std::string s("name1: value1\r\nname2: value2\r\n\r\nsomedata");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 2);
	assert (mh["name1"] == "value1");
	assert (mh["name2"] == "value2");
	int ch = istr.get();
	assert (ch == '\r');
	ch = istr.get();
	assert (ch == '\n');
	ch = istr.get();
	assert (ch == 's');
}


void MessageHeaderTest::testRead5()
{
	std::string s("name1:\r\nname2: value2\r\nname3: value3  \r\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 3);
	assert (mh["name1"] == "");
	assert (mh["name2"] == "value2");
	assert (mh["name3"] == "value3");
}


void MessageHeaderTest::testReadFolding1()
{
	std::string s("name1: value1\r\nname2: value21\r\n value22\r\nname3: value3\r\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 3);
	assert (mh["name1"] == "value1");
	assert (mh["name2"] == "value21 value22");
	assert (mh["name3"] == "value3");
}


void MessageHeaderTest::testReadFolding2()
{
	std::string s("name1: value1\nname2: value21\n\tvalue22\nname3: value3\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 3);
	assert (mh["name1"] == "value1");
	assert (mh["name2"] == "value21\tvalue22");
	assert (mh["name3"] == "value3");
}


void MessageHeaderTest::testReadFolding3()
{
	std::string s("name1: value1\r\nname2: value21\r\n value22\r\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 2);
	assert (mh["name1"] == "value1");
	assert (mh["name2"] == "value21 value22");
}


void MessageHeaderTest::testReadFolding4()
{
	std::string s("name1: value1\r\nname2: value21\r\n value22\r\n value23");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 2);
	assert (mh["name1"] == "value1");
	assert (mh["name2"] == "value21 value22 value23");
}


void MessageHeaderTest::testReadFolding5()
{
	std::string s("name1: value1\r\nname2: value21\r\n value22\r\n value23\r\nname3: value3");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.size() == 3);
	assert (mh["name1"] == "value1");
	assert (mh["name2"] == "value21 value22 value23");
	assert (mh["name3"] == "value3");
}


void MessageHeaderTest::testReadInvalid1()
{
	std::string s("name1: value1\r\nname2: value21\r\n value22\r\n value23\r\n");
	s.append(300, 'x');
	std::istringstream istr(s);
	MessageHeader mh;
	try
	{
		mh.read(istr);
		fail("malformed message - must throw");
	}
	catch (MessageException&)
	{
	}
}


void MessageHeaderTest::testReadInvalid2()
{
	std::string s("name1: value1\r\nname2: ");
	s.append(9000, 'x');
	std::istringstream istr(s);
	MessageHeader mh;
	try
	{
		mh.read(istr);
		fail("malformed message - must throw");
	}
	catch (MessageException&)
	{
	}
}


void MessageHeaderTest::testSplitElements()
{
	std::string s;
	std::vector<std::string> v;
	MessageHeader::splitElements(s, v);
	assert (v.empty());
	
	s = "foo";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 1);
	assert (v[0] == "foo");
	
	s = "  foo ";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 1);
	assert (v[0] == "foo");

	s = "foo,bar";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 2);
	assert (v[0] == "foo");
	assert (v[1] == "bar");
	
	s = "foo,,bar";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 2);
	assert (v[0] == "foo");
	assert (v[1] == "bar");

	MessageHeader::splitElements(s, v, false);
	assert (v.size() == 3);
	assert (v[0] == "foo");
	assert (v[1] == "");
	assert (v[2] == "bar");

	s = "foo;param=\"a,b\",bar;param=\"c,d\"";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 2);
	assert (v[0] == "foo;param=\"a,b\"");
	assert (v[1] == "bar;param=\"c,d\"");

	s = "foo; param=\"a,b\",  bar; param=\"c,d\"";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 2);
	assert (v[0] == "foo; param=\"a,b\"");
	assert (v[1] == "bar; param=\"c,d\"");
	
	s = "foo, bar, f00, baz";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 4);
	assert (v[0] == "foo");
	assert (v[1] == "bar");
	assert (v[2] == "f00");
	assert (v[3] == "baz");
	
	s = "a,b,c";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 3);
	assert (v[0] == "a");
	assert (v[1] == "b");
	assert (v[2] == "c");
	
	s = "a=\"value=\\\\\\\"foo, bar\\\\\\\"\",b=foo";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 2);
	assert (v[0] == "a=\"value=\\\"foo, bar\\\"\"");
	assert (v[1] == "b=foo");
	
	s = "a=\\\",b=\\\"";
	MessageHeader::splitElements(s, v);
	assert (v.size() == 2);
	assert (v[0] == "a=\"");
	assert (v[1] == "b=\"");
	
}


void MessageHeaderTest::testSplitParameters()
{
	std::string s;
	std::string v;
	NameValueCollection p;
	
	MessageHeader::splitParameters(s, v, p);
	assert (v.empty());
	assert (p.empty());
	
	s = "multipart/related";
	MessageHeader::splitParameters(s, v, p);
	assert (v == "multipart/related");
	assert (p.empty());
	
	s = "multipart/related; boundary=MIME_boundary_01234567";
	MessageHeader::splitParameters(s, v, p);
	assert (v == "multipart/related");
	assert (p.size() == 1);
	assert (p["boundary"] == "MIME_boundary_01234567");
	
	s = "multipart/related; boundary=\"MIME_boundary_76543210\"";
	MessageHeader::splitParameters(s, v, p);
	assert (v == "multipart/related");
	assert (p.size() == 1);
	assert (p["boundary"] == "MIME_boundary_76543210");
	
	s = "text/plain; charset=us-ascii";
	MessageHeader::splitParameters(s, v, p);
	assert (v == "text/plain");
	assert (p.size() == 1);
	assert (p["charset"] == "us-ascii");
	
	s = "value; p1=foo; p2=bar";
	MessageHeader::splitParameters(s, v, p);
	assert (v == "value");
	assert (p.size() == 2);
	assert (p["p1"] == "foo");
	assert (p["p2"] == "bar");
	
	s = "value; p1=\"foo; bar\"";
	MessageHeader::splitParameters(s, v, p);
	assert (v == "value");
	assert (p.size() == 1);
	assert (p["p1"] == "foo; bar");	

	s = "value ; p1=foo ; p2=bar ";
	MessageHeader::splitParameters(s, v, p);
	assert (v == "value");
	assert (p.size() == 2);
	assert (p["p1"] == "foo");
	assert (p["p2"] == "bar");
}


void MessageHeaderTest::testFieldLimit()
{
	std::string s("name1: value1\r\nname2: value2\r\nname3: value3\r\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.setFieldLimit(2);
	try
	{
		mh.read(istr);
		fail("Field limit exceeded - must throw");
	}
	catch (MessageException&)
	{
	}
}


void MessageHeaderTest::testDecodeWord()
{
	std::string coded("this is pure ASCII");
	std::string decoded = MessageHeader::decodeWord(coded, "ISO-8859-1");
	assert(decoded == coded);

	coded = "(=?ISO-8859-1?Q?a?= =?ISO-8859-1?Q?b?=)";
	decoded = MessageHeader::decodeWord(coded, "ISO-8859-1");
	assert(decoded == "(a b)");

	coded = "Hello =?UTF-8?B?RnJhbmNpcw==?=, good bye";
	decoded = MessageHeader::decodeWord(coded, "ISO-8859-1");
	assert(decoded == "Hello Francis, good bye");
}



void MessageHeaderTest::setUp()
{
}


void MessageHeaderTest::tearDown()
{
}


void MessageHeaderTest::testStrictFieldName()
{
	const std::string invalid[] = {
		"Content-Length : 5\r\n\r\n",
		"Transfer-Encoding\t: chunked\r\n\r\n",
		"Content-Length\r: 5\r\n\r\n",
		"Na\x01me: value\r\n\r\n",
		"N\xC3\xA4me: value\r\n\r\n",
		"X(a): value\r\n\r\n",
		"X/a: value\r\n\r\n",
		": value\r\n\r\n"
	};
	for (const auto& s: invalid)
	{
		std::istringstream istr(s);
		MessageHeader mh;
		mh.setStrictParsing(true);
		try
		{
			mh.read(istr);
			fail("invalid field name must throw: " + s);
		}
		catch (MessageException&)
		{
		}
	}

	std::istringstream istr("Content-Length: 5\r\nX-Name_1.~!#$%&'*+^`|: value\r\n\r\n");
	MessageHeader mh;
	mh.setStrictParsing(true);
	mh.read(istr);
	assert (mh.size() == 2);
	assert (mh.get("Content-Length") == "5");
	assert (mh.get("X-Name_1.~!#$%&'*+^`|") == "value");

	std::istringstream lenient("Content-Length : 5\r\n\r\n");
	MessageHeader mhLenient;
	mhLenient.read(lenient);
	assert (mhLenient.get("Content-Length ") == "5");
}


void MessageHeaderTest::testStrictValueWhitespace()
{
	std::istringstream istr("Transfer-Encoding:\vchunked\r\nContent-Length: \t5\f \r\n\r\n");
	MessageHeader mh;
	mh.setStrictParsing(true);
	mh.read(istr);
	assert (mh.get("Transfer-Encoding") == "\vchunked");
	assert (mh.get("Content-Length") == "5\f");

	std::istringstream lenient("Transfer-Encoding:\vchunked\r\nContent-Length: \t5\f \r\n\r\n");
	MessageHeader mhLenient;
	mhLenient.read(lenient);
	assert (mhLenient.get("Transfer-Encoding") == "chunked");
	assert (mhLenient.get("Content-Length") == "5");
}


void MessageHeaderTest::testFieldLimitLinesWithoutColon()
{
	std::string s;
	for (int i = 0; i < 200; ++i) s += "x\r\n";
	s += "name: value\r\n\r\n";
	std::istringstream istr(s);
	MessageHeader mh;
	try
	{
		mh.read(istr);
		fail("Field limit exceeded - must throw");
	}
	catch (MessageException&)
	{
	}
}


void MessageHeaderTest::testNoDecodeFramingHeaders()
{
	// Auto-decoding must not reach the headers that decide the framing,
	// otherwise an encoded word becomes "chunked" only after a front-end has
	// already framed the message on the undecoded value.
	std::string s("Subject: =?utf-8?B?Y2h1bmtlZA==?=\r\n"
		"Transfer-Encoding: =?utf-8?B?Y2h1bmtlZA==?=\r\n"
		"Content-Length: =?utf-8?B?MTA=?=\r\n\r\n");
	std::istringstream istr(s);
	MessageHeader mh;
	mh.read(istr);
	assert (mh.get("Subject") == "chunked");
	assert (mh.get("Transfer-Encoding") == "=?utf-8?B?Y2h1bmtlZA==?=");
	assert (mh.get("Content-Length") == "=?utf-8?B?MTA=?=");
}



void MessageHeaderTest::testWriteInvalidFieldValue()
{
	const std::string invalid[] = {
		"a\r\nSet-Cookie: b",
		"a\nb",
		"a\rb",
		"a\r\n",
		"a\r\n ",
		"a\r\n \r\n b",
		"a\r\n\t\r\nb",
		std::string("a\0b", 3),
		"a\x01" "b",
		"a\x7F"
	};
	for (const auto& value: invalid)
	{
		MessageHeader mh;
		mh.set("Name1", "value1");
		mh.set("Name2", value);
		std::ostringstream ostr;
		try
		{
			mh.write(ostr);
			fail("invalid field value must throw");
		}
		catch (MessageException&)
		{
		}
		assert (ostr.str().empty());
	}
}


void MessageHeaderTest::testWriteInvalidFieldName()
{
	const std::string invalid[] = {
		"",
		"Na me",
		"Name:",
		"Na\tme",
		"Na\x01me",
		"N\xC3\xA4me"
	};
	for (const auto& name: invalid)
	{
		MessageHeader mh;
		mh.set("Name1", "value1");
		mh.set(name, "value");
		std::ostringstream ostr;
		try
		{
			mh.write(ostr);
			failmsg("invalid field name must throw");
		}
		catch (MessageException&)
		{
		}
		assert (ostr.str().empty());
	}
}


void MessageHeaderTest::testWriteValidFields()
{
	MessageHeader mh;
	mh.set("X-Name_1.~!#$%&\'*+^`|", "a\tb");
	mh.set("Subject", "=?UTF-8?Q?a?=\r\n =?UTF-8?Q?b?=");
	mh.set("Text", "N\xC3\xA4me");
	mh.set("X-A/B(c)", "a\r\n \t b");
	std::ostringstream ostr;
	mh.write(ostr);
	assert (ostr.str() == "X-Name_1.~!#$%&\'*+^`|: a\tb\r\nSubject: =?UTF-8?Q?a?=\r\n =?UTF-8?Q?b?=\r\nText: N\xC3\xA4me\r\nX-A/B(c): a\r\n \t b\r\n");
}


void MessageHeaderTest::testReadDecodedControlCharacters()
{
	std::istringstream istr(
		"X-Q: =?UTF-8?Q?a=0D=0AY:_b?=\r\n"
		"X-B: =?UTF-8?B?YQ0KWTogYg==?=\r\n"
		"X-Nul: =?UTF-8?Q?a=00b?=\r\n"
		"X-Tab: =?UTF-8?Q?a=09b?=\r\n"
		"\r\n");
	MessageHeader mh;
	mh.read(istr);
	assert (mh.get("X-Q") == "=?UTF-8?Q?a=0D=0AY:_b?=");
	assert (mh.get("X-B") == "=?UTF-8?B?YQ0KWTogYg==?=");
	assert (mh.get("X-Nul") == "=?UTF-8?Q?a=00b?=");
	assert (mh.get("X-Tab") == "a\tb");

	std::ostringstream ostr;
	mh.write(ostr);
	assert (ostr.str().find("\r\nY:") == std::string::npos);
}


CppUnit::Test* MessageHeaderTest::suite()
{
	CppUnit::TestSuite* pSuite = new CppUnit::TestSuite("MessageHeaderTest");

	CppUnit_addTest(pSuite, MessageHeaderTest, testWrite);
	CppUnit_addTest(pSuite, MessageHeaderTest, testRead1);
	CppUnit_addTest(pSuite, MessageHeaderTest, testRead2);
	CppUnit_addTest(pSuite, MessageHeaderTest, testRead3);
	CppUnit_addTest(pSuite, MessageHeaderTest, testRead4);
	CppUnit_addTest(pSuite, MessageHeaderTest, testRead5);
	CppUnit_addTest(pSuite, MessageHeaderTest, testReadFolding1);
	CppUnit_addTest(pSuite, MessageHeaderTest, testReadFolding2);
	CppUnit_addTest(pSuite, MessageHeaderTest, testReadFolding3);
	CppUnit_addTest(pSuite, MessageHeaderTest, testReadFolding4);
	CppUnit_addTest(pSuite, MessageHeaderTest, testReadFolding5);
	CppUnit_addTest(pSuite, MessageHeaderTest, testReadInvalid1);
	CppUnit_addTest(pSuite, MessageHeaderTest, testReadInvalid2);
	CppUnit_addTest(pSuite, MessageHeaderTest, testSplitElements);
	CppUnit_addTest(pSuite, MessageHeaderTest, testSplitParameters);
	CppUnit_addTest(pSuite, MessageHeaderTest, testFieldLimit);
	CppUnit_addTest(pSuite, MessageHeaderTest, testDecodeWord);
	CppUnit_addTest(pSuite, MessageHeaderTest, testNoDecodeFramingHeaders);
	CppUnit_addTest(pSuite, MessageHeaderTest, testStrictFieldName);
	CppUnit_addTest(pSuite, MessageHeaderTest, testStrictValueWhitespace);
	CppUnit_addTest(pSuite, MessageHeaderTest, testFieldLimitLinesWithoutColon);
	CppUnit_addTest(pSuite, MessageHeaderTest, testWriteInvalidFieldValue);
	CppUnit_addTest(pSuite, MessageHeaderTest, testWriteInvalidFieldName);
	CppUnit_addTest(pSuite, MessageHeaderTest, testWriteValidFields);
	CppUnit_addTest(pSuite, MessageHeaderTest, testReadDecodedControlCharacters);

	return pSuite;
}

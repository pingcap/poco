#
# Makefile
#
# Makefile for Poco Net testsuite
#

include $(POCO_BASE)/build/rules/global

objects = \
	DNSTest HTTPServerTestSuite MulticastSocketTest SocketStreamTest \
	DatagramSocketTest HTTPStreamFactoryTest SocketTest \
	Driver HTTPTestServer SocketsTestSuite \
	EchoServer HTTPTestSuite NameValueCollectionTest TCPServerTest \
	HTTPClientSessionTest IPAddressTest NetCoreTestSuite TCPServerTestSuite \
	HTTPRequestTest MessageHeaderTest NetTestSuite UDPEchoServer \
	HTTPResponseTest MessagesTestSuite NetworkInterfaceTest \
	HTTPServerTest MulticastEchoServer SocketAddressTest \
	HTTPCookieTest HTTPCredentialsTest MediaTypeTest \
	HTTPClientTestSuite \
	SocketReactorTest ReactorTestSuite \
	RawSocketTest \
	WebSocketTest WebSocketTestSuite \
	OAuth20CredentialsTest OAuthTestSuite \
	PollSetTest

target         = testrunner
target_version = 1
target_libs    = PocoNet PocoFoundation PocoCppUnit

include $(POCO_BASE)/build/rules/exec

ifdef POCO_UNBUNDLED
        SYSLIBS += -lz
endif

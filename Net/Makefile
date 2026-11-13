#
# Makefile
#
# Makefile for Poco Net
#

include $(POCO_BASE)/build/rules/global

SHAREDOPT_CXX += -DNet_EXPORTS

objects = \
	Net DNS HTTPResponse HostEntry Socket \
	DatagramSocket HTTPServer IPAddress IPAddressImpl SocketAddress SocketAddressImpl \
	HTTPBasicCredentials HTTPCookie MediaType \
	DatagramSocketImpl HTTPServerConnection MessageHeader \
	HTTPChunkedStream HTTPServerConnectionFactory MulticastSocket SocketStream \
	HTTPClientSession HTTPServerParams StreamSocket SocketImpl \
	HTTPFixedLengthStream HTTPServerRequest HTTPServerRequestImpl StreamSocketImpl \
	HTTPHeaderStream HTTPServerResponse HTTPServerResponseImpl NameValueCollection TCPServer \
	HTTPMessage HTTPServerSession NetException TCPServerConnection HTTPBufferAllocator \
	HTTPRequest HTTPSession NetworkInterface  \
	HTTPRequestHandler HTTPStream ServerSocket TCPServerDispatcher TCPServerConnectionFactory \
	HTTPRequestHandlerFactory ServerSocketImpl TCPServerParams \
	SocketReactor SocketNotifier SocketNotification AbstractHTTPRequestHandler \
	MailRecipient \
	RawSocket RawSocketImpl \
	WebSocket WebSocketImpl \
	PollSet

target         = PocoNet
target_version = $(LIBVERSION)
target_libs    = PocoFoundation

include $(POCO_BASE)/build/rules/lib

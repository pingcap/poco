#
# Makefile
#
# Makefile for Poco Net Samples
#

.PHONY: projects
clean all: projects
projects:
	$(MAKE) -C dict $(MAKECMDGOALS)
	$(MAKE) -C TimeServer $(MAKECMDGOALS)
	$(MAKE) -C httpget $(MAKECMDGOALS)
	$(MAKE) -C HTTPTimeServer $(MAKECMDGOALS)
	$(MAKE) -C HTTPLoadTest $(MAKECMDGOALS)
	$(MAKE) -C EchoServer $(MAKECMDGOALS)
	$(MAKE) -C WebSocketServer $(MAKECMDGOALS)
	$(MAKE) -C ifconfig $(MAKECMDGOALS)

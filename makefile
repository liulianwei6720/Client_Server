server: server.h server.cc
	g++ server.cc -o server
client: client.h client.cc
	g++ client.cc -o client
debug:
	g++ client.cc -g -o client
	g++ server.cc -g -o server

.PHONY: clean
clean:
	rm server client
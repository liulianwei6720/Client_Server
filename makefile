server: server.h server.cc
	g++ server.cc -o server
client: client.h client.cc
	g++ client.cc -o client
.PHONY: clean
clean:
	rm server client
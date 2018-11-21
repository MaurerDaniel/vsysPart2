

all: server

server: *.cpp
	g++ -std=c++11 -ggdb -o server *.cpp
	

run: server
	drmemory -- server

	
clean:
	rm -f server server



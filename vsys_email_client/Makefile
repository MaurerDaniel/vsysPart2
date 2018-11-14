all: client

client: *.cpp
	g++ -std=c++11 -ggdb -o client *.cpp
	
tests: *.cpp
	g++ -std=c++11 -DTESTS -I../gtest/include ../gtest/lib/gtest_main.a -ggdb -o UEB1_tests *.cpp

run: client
	drmemory -- client
	
run_tests: tests
	drmemory -- UEB1_tests
	
clean:
	rm -f client client UEB*_tests.exe UEB*_tests *.o
debug:
	g++ -std=c++11 -Wall -g *.cpp -o main

release:
	g++ -std=c++11 -DNDEBUG -Wall *.cpp -o main

test-tree:
	g++ -std=c++11 -Wall -g Tests/testTree.cpp -o test
	./test


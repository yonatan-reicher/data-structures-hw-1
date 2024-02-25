debug:
	g++ -std=c++11 -Wall -g *.cpp -o main

release:
	g++ -std=c++11 -DNDEBUG -Wall *.cpp -o main

test-tree:
	g++ -std=c++11 -Wall -g Tests/testTree.cpp -o test
	./test

SOURCES = $(filter-out maina1.cpp,$(wildcard *.cpp))
TESTS = $(wildcard Tests/*.cpp)

unit-test: $(SOURCES) $(TESTS)
	@echo "Test " $(SOURCES)
	for file in $(TESTS); do \
		echo; \
		echo "==========" "Running" $$file "=========="; \
		g++ -std=c++11 -Wall -g $(SOURCES) $$file -o test; \
		./test; \
	done

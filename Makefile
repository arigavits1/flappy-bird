all:
	g++ -c *.cpp -std=c++20 -m64 -O3 -Wall -I include && g++ *.o -o bin/release/main -s -lsfml-graphics -lsfml-window -lsfml-system && ./bin/release/main

cleanUp:
	rm -r *.o
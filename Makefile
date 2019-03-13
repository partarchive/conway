
game: *.cpp
	c++ -std=c++17 -I/usr/local/include -I./include -L/usr/local/lib -lglfw3 -framework Cocoa -framework CoreVideo -framework IOKit main.cpp -o game

play: game
	./game
.PHONY: play
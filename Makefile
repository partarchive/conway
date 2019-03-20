
INCLUDE=-I /usr/local/include -I include

default: game

clean:
	rm -rf obj
	rm -f game

obj:
	mkdir -p obj

obj/glad.so: obj include/glad/*
	cc -shared ${INCLUDE} -o obj/glad.so include/glad/glad.c

obj/imgui.so: obj obj/glad.so include/imgui/*
	c++ --std c++17 -shared ${INCLUDE} -D IMGUI_IMPL_OPENGL_LOADER_GLAD=1 -l glfw -framework OpenGL -o obj/imgui.so include/imgui/*.cpp obj/glad.so

game: *.cpp obj/imgui.so obj/glad.so
	c++ --std=c++17 -I/usr/local/include -I./include -L/usr/local/lib -lglfw -framework Cocoa -framework CoreVideo -framework IOKit main.cpp -o game obj/*.so

play: game
	./game
.PHONY: play

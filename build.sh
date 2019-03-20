#!/bin/sh

INCLUDE="-I /usr/local/include -I ./include"

cc -shared $INCLUDE -o obj/glad.so include/glad/glad.c
c++ --std c++17 -shared $INCLUDE -D IMGUI_IMPL_OPENGL_LOADER_GLAD=1 -l glfw -framework OpenGL -o obj/imgui.so include/imgui/*.cpp obj/glad.so

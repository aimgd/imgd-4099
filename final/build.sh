mkdir -p bin

clang++ -g -o bin/final src/final.cc src/ext/imgui-backends/imgui_impl_sdl2.cpp src/ext/imgui/backends/imgui_impl_opengl3.cpp -lSDL2
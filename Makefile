CXX_FLAGS	:= -std=c++17 -ggdb -I
LIBRARIES	:= -lsfml-graphics -lsfml-window -lsfml-system

all: binaries/main

run: binaries/main
	./binaries/main

binaries/main: src/*.cpp
	g++ $(CXX_FLAGS) include -L lib $^ -o $@ $(LIBRARIES)

clean:
	cd binaries && rm main
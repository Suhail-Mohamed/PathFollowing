CXX_FLAGS	:= -std=c++17 -ggdb -I
LIBRARIES	:= -lsfml-graphics -lsfml-window -lsfml-system

all: bin/main

run: clean all
	./bin/main

bin/main: src/*.cpp
	g++ $(CXX_FLAGS) include -L lib $^ -o $@ $(LIBRARIES)

clean:
	del /p bin\*.exe

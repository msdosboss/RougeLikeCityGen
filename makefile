GXX = gcc
RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: main

debug: debugMain

debugMain: src/main.c src/generator.c src/roadGraph.c src/main.h src/generator.h src/roadGraph.h
	$(GXX) -pg -o build/debugMain src/main.c src/generator.c src/roadGraph.c $(RAYLIB_FLAGS) -lm

main: objects/main.o objects/generator.o objects/roadGraph.o
	$(GXX) -o build/main objects/main.o objects/generator.o objects/roadGraph.o $(RAYLIB_FLAGS) -lm

objects/main.o: src/main.c src/main.h
	$(GXX) -c -o objects/main.o src/main.c $(RAYLIB_FLAGS)

objects/generator.o: src/generator.c src/generator.h
	$(GXX) -c -o objects/generator.o src/generator.c $(RAYLIB_FLAGS) -lm

objects/roadGraph.o: src/roadGraph.c src/roadGraph.h
	$(GXX) -c -o objects/roadGraph.o src/roadGraph.c

clean:
	rm -f objects/*.o
	rm -f build/main
	rm -f build/debugMain 
	rm -f build/gmon.out

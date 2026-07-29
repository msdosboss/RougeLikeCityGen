GXX = gcc
RAYLIB_FLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all: main

main: objects/main.o objects/generator.o
	$(GXX) -o main objects/main.o objects/generator.o $(RAYLIB_FLAGS)

objects/main.o: src/main.c
	$(GXX) -c -o objects/main.o src/main.c $(RAYLIB_FLAGS)

objects/generator.o: src/generator.c
	$(GXX) -c -o objects/generator.o src/generator.c $(RAYLIB_FLAGS)


clean:
	rm objects/*.o
	rm main

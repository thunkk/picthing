OBJS = main.cpp
OBJ_NAME = main

all : $(OBJS)
	g++ $(OBJS) -lSDL2 -lSDL2_ttf -o $(OBJ_NAME)
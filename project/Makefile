# Project: droneDelivery

CPP  = g++
CC   = gcc
BIN  = droneDelivery

OBJ  = main.o drone.o mesh.o scenario.o
LINKOBJ  = main.o drone.o mesh.o scenario.o
LIBS = -L/usr/X11R6 -lGL -lGLU -lSDL2_image -lSDL2 -lm -lSDL2_ttf
INCS = -I. -I/usr/X11R6/include
CXXINCS=#
CXXFLAGS = $(CXXINCS)
CFLAGS = $(INCS) 
#CFLAGS=`sdl2-config --cflags`
#LFLAGS=`sdl2-config --libs` -lSDL2_ttf
RM = rm -f

all: $(BIN)


clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

drone.o: drone.cpp
	$(CPP) -c drone.cpp -o drone.o $(CXXFLAGS)

mesh.o: mesh.cpp
	$(CPP) -c mesh.cpp -o mesh.o $(CXXFLAGS)

scenario.o: scenario.cpp
	$(CPP) -c scenario.cpp -o scenario.o $(CXXFLAGS)

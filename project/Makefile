# Project: droneDelivery

CPP  = g++
CC   = gcc
BIN  = droneDelivery

OBJ  = main.o drone.o mesh.o
LINKOBJ  = main.o drone.o mesh.o
LIBS = -L/usr/X11R6 -lGL -lGLU -lSDL2 -lm
INCS = -I. -I/usr/X11R6/include
CXXINCS=#
CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS)  
CDEB= #-DDEBUG
RM = rm -f

all: $(BIN)


clean:
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cpp
	$(CPP) $(CDEB) -c main.cpp -o main.o $(CXXFLAGS)

drone.o: drone.cpp
	$(CPP) $(CDEB) -c drone.cpp -o drone.o $(CXXFLAGS)

mesh.o: mesh.cpp
	$(CPP) $(CDEB) -c mesh.cpp -o mesh.o $(CXXFLAGS)

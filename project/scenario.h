//contiene le definizioni degli oggetti di scena non rientranti nella classe drone
#include <math.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector> // la classe vector di STL

#include "drone.h"
#include "point3.h"
#include "mesh.h"


void drawFloor();
void drawMuro ();
void drawCube(Drone drone);
void drawManifest (int x, int y, int z);
void drawSphere(double r, int lats, int longs);
void drawSky();

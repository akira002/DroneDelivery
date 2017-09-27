#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>

#include "drone.h"

float viewAlpha=20, viewBeta=40; // angoli che definiscono la vista
float eyeDist=5.0; // distanza dell'occhio dall'origine
int scrH=640, scrW=640; // altezza e larghezza viewport (in pixels)

Drone drone; // il nostro drone
int nstep=0; // numero di passi di FISICA fatti fin'ora
const int PHYS_SAMPLING_STEP=10; // numero di millisec che un passo di fisica simula

// disegna gli assi nel sist. di riferimento
void drawAxis(){
  const float K=0.10;
  glColor3f(0,0,1);
  glBegin(GL_LINES);
    glVertex3f( -1,0,0 );
    glVertex3f( +1,0,0 );

    glVertex3f( 0,-1,0 );
    glVertex3f( 0,+1,0 );

    glVertex3f( 0,0,-1 );
    glVertex3f( 0,0,+1.5 );
  glEnd();

  glBegin(GL_TRIANGLES);
    glVertex3f( 0,+1  ,0 );
    glVertex3f( K,+1-K,0 );
    glVertex3f(-K,+1-K,0 );

    glVertex3f( +1,   0, 0 );
    glVertex3f( +1-K,+K, 0 );
    glVertex3f( +1-K,-K, 0 );

    glVertex3f( 0, 0,+1.5 );
    glVertex3f( 0,+K,+1.5-K );
    glVertex3f( 0,-K,+1.5-K );
  glEnd();
}

// disegna un cubo rasterizzando quads
void drawCubeFill()
{
  glBegin(GL_QUADS);

    glNormal3f(  0,0,+1  );
    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );

    glNormal3f(  0,0,-1  );
    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );

    glNormal3f(  0,+1,0  );
    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );

    glNormal3f(  0,-1,0  );
    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );

    glNormal3f( +1,0,0  );
    glVertex3f( +1,+1,+1 );
    glVertex3f( +1,-1,+1 );
    glVertex3f( +1,-1,-1 );
    glVertex3f( +1,+1,-1 );

    glNormal3f( -1,0,0  );
    glVertex3f( -1,+1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( -1,+1,+1 );

  glEnd();
}

// disegna un cubo in wireframe
void drawCubeWire()
{
  glBegin(GL_LINE_LOOP); // faccia z=+1
    glVertex3f( +1,+1,+1 );
    glVertex3f( -1,+1,+1 );
    glVertex3f( -1,-1,+1 );
    glVertex3f( +1,-1,+1 );
  glEnd();

  glBegin(GL_LINE_LOOP); // faccia z=-1
    glVertex3f( +1,-1,-1 );
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,+1,-1 );
    glVertex3f( +1,+1,-1 );
  glEnd();

  glBegin(GL_LINES);  // 4 segmenti da -z a +z
    glVertex3f( -1,-1,-1 );
    glVertex3f( -1,-1,+1 );

    glVertex3f( +1,-1,-1 );
    glVertex3f( +1,-1,+1 );

    glVertex3f( +1,+1,-1 );
    glVertex3f( +1,+1,+1 );

    glVertex3f( -1,+1,-1 );
    glVertex3f( -1,+1,+1 );
  glEnd();
}

void drawCube()
{
  glColor3f(.9,.9,.9);
  drawCubeFill();
  glColor3f(0,0,0);
  drawCubeWire();
}

void drawFloor()
{
  const float S=2; // size
  const float H=-0.15; // altezza
  const int K=400; //disegna K x K quads

  // disegna un quad
  glBegin(GL_QUADS);
    glColor3f(0.7, 0.7, 0.7);
    glNormal3f(0,1,0);
    glVertex3d(-S, H, -S);
    glVertex3d(+S, H, -S);
    glVertex3d(+S, H, +S);
    glVertex3d(-S, H, +S);
  glEnd();
}

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win){

  // linee spesse
  glLineWidth(3);

  // settiamo il viewport
  glViewport(0, 0, scrW, scrH);

  // colore sfondo = nero
  glClearColor(1,1,1,1);

  // settiamo la matrice di proiezione
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluPerspective( 60, //fovy,
		((float)scrW) / scrH,//aspect Y/X,
		1,//zNear,
		100//zFar
  );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // riempe tutto lo screen buffer di pixel color sfondo
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  //drawAxis(); // disegna assi frame VISTA

  // settiamo matrice di vista
  glTranslatef(0,0,-eyeDist);
  glRotatef(viewBeta,  1,0,0);
  glRotatef(viewAlpha, 0,1,0);

  drawFloor(); // disegna il suolo

  //drawAxis(); // disegna assi frame MONDO

  // settiamo matrice di modellazione
  glScalef(.5,.5,.5);

  //drawAxis(); // disegna assi frame OGGETTO
  //drawCubeWire();

  drone.Render(); // disegna la macchina

  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate
  glFinish();

  // ho finito: buffer di lavoro diventa visibile
  SDL_GL_SwapWindow(win);
}

void redraw(){
  // ci automandiamo un messaggio che (s.o. permettendo)
  // ci fara' ridisegnare la finestra
  SDL_Event e;
  e.type=SDL_WINDOWEVENT;
  e.window.event=SDL_WINDOWEVENT_EXPOSED;
  SDL_PushEvent(&e);
}


int main(int argc, char* argv[])
{
SDL_Window *win;
SDL_GLContext mainContext;
Uint32 windowID;

  // inizializzazione di SDL
  SDL_Init( SDL_INIT_VIDEO );

  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  // facciamo una finestra di scrW x scrH pixels
  win=SDL_CreateWindow(argv[0], 0, 0, scrW, scrH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  //Create our opengl context and attach it to our window
  mainContext=SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
  //accendiamo le luci e coloriamo gli oggetti
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE); // rinormalizza le normali prima di usarle

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  bool done=0;
  while (!done) {

    SDL_Event e;

    // guardo se c'e' un evento:
    if (SDL_PollEvent(&e)) {
     // se si: processa evento
     switch (e.type) {
      case SDL_KEYDOWN:
        static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_UP, SDLK_DOWN};
        drone.controller.EatKey(e.key.keysym.sym, keymap , true);
        break;
      case SDL_KEYUP:
        drone.controller.EatKey(e.key.keysym.sym, keymap , false);
        break;
      case SDL_QUIT:
          done=1;
          break;
      case SDL_WINDOWEVENT:
         // dobbiamo ridisegnare la finestra
          if (e.window.event==SDL_WINDOWEVENT_EXPOSED)
            rendering(win);
          else{
           windowID = SDL_GetWindowID(win);
           if (e.window.windowID == windowID)  {
             switch (e.window.event)  {
                  case SDL_WINDOWEVENT_SIZE_CHANGED:  {
                     scrW = e.window.data1;
                     scrH = e.window.data2;
                     glViewport(0,0,scrW,scrH);
                     rendering(win);
                     //redraw(); // richiedi ridisegno
                     break;
                  }
             }
           }
         }
      break;

      case SDL_MOUSEMOTION:
        if (e.motion.state & SDL_BUTTON(1) ) {
          viewAlpha+=e.motion.xrel;
          viewBeta +=e.motion.yrel;
          if (viewBeta<-90) viewBeta=-90;
          if (viewBeta>+90) viewBeta=+90;
	  rendering(win);
//          redraw(); // richiedi ridisegno
        }
        break;

       case SDL_MOUSEWHEEL:
        if (e.wheel.y < 0 ) {
          // avvicino il punto di vista (zoom in)
          eyeDist=eyeDist*0.9;
          if (eyeDist<1) eyeDist = 1;
        };
        if (e.wheel.y > 0 ) {
          // allontano il punto di vista (zoom out)
          eyeDist=eyeDist/0.9;
         };
        break;
     }
    } else {
      // nessun evento: siamo IDLE

      Uint32 timeNow=SDL_GetTicks(); // che ore sono?
      bool doneSomething=false;
      int guardia=0; // sicurezza da loop infinito

      // finche' il tempo simulato e' rimasto indietro rispetto
      // al tempo reale...
      while (nstep*PHYS_SAMPLING_STEP < timeNow ) {
        drone.DoStep();
        nstep++;
        doneSomething=true;
        timeNow=SDL_GetTicks();
        if (guardia++>1000) {done=true; break;} // siamo troppo lenti!
      }

      if (doneSomething) {
        rendering(win);
//      redraw();
      }
      else {
        // tempo libero!!!
      }
    }
  }
SDL_GL_DeleteContext(mainContext);
SDL_DestroyWindow(win);
SDL_Quit ();
return (0);
}

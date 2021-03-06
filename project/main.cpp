#include <math.h>
#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#include "scenario.h"

#define CAMERA_BACK_DRONE 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_DRONE 2
#define CAMERA_MOUSE 3
#define CAMERA_TYPE_MAX 4

float viewAlpha=20, viewBeta=40; // angoli che definiscono la vista
float eyeDist=5.0; // distanza dell'occhio dall'origine
int scrH=750, scrW=750; // altezza e larghezza viewport (in pixels)
bool useWireframe=false;
bool useEnvmap=true;
bool useColors=false;
bool useShadow=true;
int cameraType=0;

Drone drone; // il nostro drone
int nstep=0; // numero di passi di FISICA fatti fin'ora
const int PHYS_SAMPLING_STEP=10; // numero di millisec che un passo di fisica simula

// Frames Per Seconds
const int fpsSampling = 3000; // lunghezza intervallo di calcolo fps
float fps=0; // valore di fps dell'intervallo precedente
int fpsNow=0; // quanti fotogrammi ho disegnato fin'ora nell'intervallo attuale
Uint32 timeLastInterval=0; // quando e' cominciato l'ultimo intervallo
Uint32 timeStartGame=0; //quando e' incominciata la partita
Uint32 actualTime; //tempo di gioco

int punteggio = 0; // punteggio del giocatore

//coordinate del cubo, servono per mappa
extern int pos_x;
extern int pos_z;

enum textquality {solid, shaded, blended}; //qualita del testo scritto
uint font_id = -1; //id della realtiva texture

//Funzione per stampare a schermo del testo
void SDL_GL_DrawText(TTF_Font *font,char fgR, char fgG, char fgB, char fgA,
              char bgR, char bgG, char bgB, char bgA, char *text, int x, int y,
	                    enum textquality quality)
{
	SDL_Color tmpfontcolor = {fgR,fgG,fgB,fgA};
	SDL_Color tmpfontbgcolor = {bgR, bgG, bgB, bgA};
	SDL_Surface *initial;
	SDL_Surface *intermediary;
	SDL_Rect location;
	int w,h;

/* Usa SDL_TTF per renderizzare il nostro testo */
	initial=NULL;
	if (quality == solid)
	  initial = TTF_RenderText_Solid(font, text, tmpfontcolor);
	else
	  if (quality == shaded)
	     initial = TTF_RenderText_Shaded(font, text, tmpfontcolor, tmpfontbgcolor);
          else
            if (quality == blended)
               initial = TTF_RenderText_Blended(font, text, tmpfontcolor);

/* Converte il testo renderizzato ad un formato conosciuto */
	w = initial->w;
	h = initial->h;

/* Allochiamo una nuova surface RGB */
	intermediary = SDL_CreateRGBSurface(0, w, h, 32,
		0x000000ff,0x0000ff00, 0x00ff0000,0xff000000);
/* Copiamo il contenuto dalla prima alla seconda surface */
        SDL_BlitSurface(initial, 0, intermediary, 0);

/* Diciamo a GL della nuova texture */
	glBindTexture(GL_TEXTURE_2D, font_id);
  //(	GLenum target,GLint level,GLint internalFormat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,	const GLvoid * data);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, intermediary->pixels );


/* GL_NEAREST looks horrible, if scaled... */
  //texture: Specifies the texture object name for glTextureParameter functions.
  //pname: Specifies the symbolic name of a single-valued texture parameter
  //param: For the scalar commands, specifies the value of pname.

  //The texture minifying (magnifying) function is used whenever the level-of-detail function
  // used when sampling from the texture determines that the texture should be minified (magnified)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

/* prepariamoci al render della texture */
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font_id);
	glColor3f(1.0f, 1.0f, 1.0f);

        if ( initial != NULL )
        {
         location.x = x;
         location.y = y;
        }

/* Disegnamo dei quadrati come location del testo */
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
			glVertex2f(location.x    , location.y);
		glTexCoord2f(1.0f, 1.0f);
			glVertex2f(location.x + w, location.y);
		glTexCoord2f(1.0f, 0.0f);
			glVertex2f(location.x + w, location.y + h);
		glTexCoord2f(0.0f, 0.0f);
			glVertex2f(location.x    , location.y + h);
	glEnd();

/*Disegnamo un contorno per i quadrati */
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_STRIP);
                glVertex2f((GLfloat)location.x-1, (GLfloat)location.y-1);
                glVertex2f((GLfloat)location.x + w +1, (GLfloat)location.y-1);
                glVertex2f((GLfloat)location.x + w +1, (GLfloat)location.y + h +1);
                glVertex2f((GLfloat)location.x-1    , (GLfloat)location.y + h +1);
                glVertex2f((GLfloat)location.x-1, (GLfloat)location.y-1);
        glEnd();

/* Bad things happen if we delete the texture before it finishes */
	glFinish();

/* return the deltas in the unused w,h part of the rect */
        location.w = initial->w;
	location.h = initial->h;

/* Clean up */
	glDisable(GL_TEXTURE_2D);
  //Use this function to free an RGB surface.
	SDL_FreeSurface(initial);
	SDL_FreeSurface(intermediary);
//	glColor3f(0.0f, 0.0f, 0.0f);
}

// setta le matrici di trasformazione in modo
// che le coordinate in spazio oggetto siano le coord
// del pixel sullo schemo (serve per disegnare in 2D sopra scena)
void  SetCoordToPixel(){
  //specifica quale matrice è la matrice corrente
  glMatrixMode(GL_PROJECTION);
  //sostituisce la matrice corrente con la matrice identità
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //moltiplica matrice corrente per matrice di traslazione
  glTranslatef(-1,-1,0);
  //moltiplica matrice corrente per matrice di scala
  glScalef(2.0/scrW, 2.0/scrH, 1);
}

bool LoadTexture(int textbind,char *filename){
  SDL_Surface *s = IMG_Load(filename);
  if (!s) return false;

  glBindTexture(GL_TEXTURE_2D, textbind);
  //MIP map è una raccolta ottimizzata di immagini bitmap precalcolate associate ad una texture principale
  //(ogni bitmap versione più piccola texture principale) -> aumenta velocita rendering, meno artefatti, più richiesta memoria
  gluBuild2DMipmaps(
    GL_TEXTURE_2D,
    GL_RGB,
    s->w, s->h,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    s->pixels
  );
  glTexParameteri(
  GL_TEXTURE_2D,
  GL_TEXTURE_MAG_FILTER,
  GL_LINEAR );
  //texture minifying function con uso di mipmap
  glTexParameteri(
  GL_TEXTURE_2D,
  GL_TEXTURE_MIN_FILTER,
  GL_LINEAR_MIPMAP_LINEAR );
  return true;
}

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
    glVertex3f( 0,0,+1 );
  glEnd();

  glBegin(GL_TRIANGLES);
    glVertex3f( 0,+1  ,0 );
    glVertex3f( K,+1-K,0 );
    glVertex3f(-K,+1-K,0 );

    glVertex3f( +1,   0, 0 );
    glVertex3f( +1-K,+K, 0 );
    glVertex3f( +1-K,-K, 0 );

    glVertex3f( 0, 0,+1 );
    glVertex3f( 0,+K,+1-K );
    glVertex3f( 0,-K,+1-K );
  glEnd();

}


// setto la posizione della camera
void setCamera(){

        double px = drone.px;
        double py = drone.py;
        double pz = drone.pz;
        double angle = drone.facing;
        double cosf = cos(angle*M_PI/180.0);
        double sinf = sin(angle*M_PI/180.0);
        double camd, camh, ex, ey, ez, cx, cy, cz;
        double cosff, sinff;

// controllo la posizione della camera a seconda dell'opzione selezionata
        switch (cameraType) {
        case CAMERA_BACK_DRONE:
                camd = 2.5;
                camh = 1.0;
                ex = px + camd*sinf;
                ey = py + camh;
                ez = pz + camd*cosf;
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                //definisce una trasformazione di vista (specifico posizione camera con posizione e orientamento, invece di sequenza rotate translate)
                //sposta la telecamera nel punto di osservazione della scena,considera 2 come centro della scena verso il quale è rivolto lo sguardo,
                // e 3 è il vettore che determina l’alto della camera
                gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
                break;
        case CAMERA_TOP_FIXED:
                camd = 0.5;
                camh = 0.55;
                angle = drone.facing + 40.0;
                cosff = cos(angle*M_PI/180.0);
                sinff = sin(angle*M_PI/180.0);
                ex = px + camd*sinff;
                ey = py + camh;
                ez = pz + camd*cosff;
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
                break;
        case CAMERA_TOP_DRONE:
                camd = 2.5;
                camh = 1.0;
                ex = px + camd*sinf;
                ey = py + camh;
                ez = pz + camd*cosf;
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                gluLookAt(ex,ey+5,ez,cx,cy,cz,0.0,1.0,0.0);
                break;
        case CAMERA_MOUSE:
                glTranslatef(0,0,-eyeDist);
                glRotatef(viewBeta,  1,0,0);
                glRotatef(viewAlpha, 0,1,0);
/*
printf("%f %f %f\n",viewAlpha,viewBeta,eyeDist);
                ex=eyeDist*cos(viewAlpha)*sin(viewBeta);
                ey=eyeDist*sin(viewAlpha)*sin(viewBeta);
                ez=eyeDist*cos(viewBeta);
                cx = px - camd*sinf;
                cy = py + camh;
                cz = pz - camd*cosf;
                gluLookAt(ex,ey,ez,cx,cy,cz,0.0,1.0,0.0);
*/
                break;
        }
}


//funzione che disegna una mappa 2D della scena
void drawMap(int scrH, int scrW) {
  /* calcolo delle coordinate reali dell'oggetto su mappa */
  //posizione del drone sulla mappa
  float map_posx;
  float map_posz;
  map_posx = ((50*drone.px)/71) + 50 + 20;
  map_posz = ((50*drone.pz)/71) + 50 + scrH-20-100;

  //posizione della scatola sulla mappa
  float map_cubex;
  float map_cubez;
  map_cubex = ((50*pos_x)/71) + 50 + 20;
  map_cubez = ((50*pos_z)/71) + 50 + scrH-20-100;

  /* disegno mappa */


  glColor3ub(210,210,210);
  glBegin(GL_POLYGON);
    glVertex2d(20,scrH -20 -100); //punto in basso a sx
    glVertex2d(20,scrH -20);
    glVertex2d(120,scrH -20);
    glVertex2d(120,scrH-20-100);
   glEnd();

   glColor3ub(0,0,0);
   glBegin(GL_LINE_LOOP);
     glVertex2d(20,scrH -20 -100);
     glVertex2d(20,scrH -20);
     glVertex2d(120,scrH-20);
     glVertex2d(120,scrH-20-100);
   glEnd();

   glColor3ub(0,0,0);
   glBegin(GL_LINE_LOOP);
     glVertex2d(20, scrH-70);
     glVertex2d(120, scrH-70);
    glEnd();

  /* disegno del cursore */
  glColor3ub(0,0,255);
  glBegin(GL_QUADS);
    glVertex2d(map_posx, map_posz + 3);
    glVertex2d(map_posx + 3, map_posz);
    glVertex2d(map_posx, map_posz - 3);
    glVertex2d(map_posx - 3, map_posz);
  glEnd();

  /* disegno del target */
  glColor3ub(255,0,0);
  glBegin(GL_QUADS);
    glVertex2d(map_cubex, map_cubez + 3);
    glVertex2d(map_cubex + 3, map_cubez);
    glVertex2d(map_cubex, map_cubez - 3);
    glVertex2d(map_cubex - 3, map_cubez);
  glEnd();
}

/* Esegue il Rendering della scena */
void rendering(SDL_Window *win, TTF_Font *font){

  // un frame in piu'!!!
  fpsNow++;

  glLineWidth(3); // linee larghe

  // settiamo il viewport
  glViewport(0,0, scrW, scrH);

  // colore sfondo = bianco
  glClearColor(1,1,1,1);


  // settiamo la matrice di proiezione
  //matrice di proiezione, definisce le proprietà della camera che vede gli oggetti nel frame di coordinate del nostro mondo. Tipicamente: fattore zoom, aspect ratio, piani di clipping
  //Tipicamente settata una volta, poi si usano le matrici di modellazione
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  //definisce una matrice di proiezione prospettica
  gluPerspective( 70, //fovy, (field of view, in gradi, nella direzione y)
		((float)scrW) / scrH,//aspect Y/X,
		0.2,//distanza del NEAR CLIPPING PLANE in coordinate vista
		1000  //distanza del FAR CLIPPING PLANE in coordinate vista
  );

  //matrice modelview (modellazione) definisce come gli oggetti sono trasformati (tralati, ruotati e scalati) nel frame di coordinate del nostro scenario
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();

  // riempe tutto lo screen buffer di pixel color sfondo, perché fa un clear dei buffer al valore presettato
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  //drawAxis(); // disegna assi frame VISTA

  // setto la posizione luce
  float tmpv[4] = {0,1,2,  0}; // ultima comp=0 => luce direzionale
                              // ultima comp=1 => i raggi escono in tutte le direzioni
  glLightfv(GL_LIGHT0, GL_POSITION, tmpv );

  // the ambient RGBA intensity of the light
  //float tmpp[4] = {3,3,3,  1};
  //glLightfv(GL_LIGHT1, GL_AMBIENT, tmpp);

  // GL_FLAT è il default: ogni poligono ha un colore
  // GL_SMOOTH: ogni punto della superficie del poligono ha un'ombreggiatura derivata
  //            dall'interpolazione delle normali ai vertici
  //glShadeModel(GL_FLAT);

  setCamera();


  //drawAxis(); // disegna assi frame MONDO

  static float tmpcol[4] = {1,1,1,  1};
  //specificano i parametri riguardanti i materiali, per il modello di illuminazione
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tmpcol);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

  //Se abilitato (e non c'è attivo vertex shader), usa i parametri di illuminazione correnti per computare i colori o gli indici dei vertici.
  //Altrimenti associa semplicemente il colore corrente ad ogni vertice.
  glEnable(GL_LIGHTING);

  // settiamo matrice di modellazione
  //drawAxis(); // disegna assi frame OGGETTO
  //drawCubeWire();

  drawSky(); // disegna il cielo come sfondo

  drawFloor(); // disegna il suolo
  drawMuro(); // disegna il muro
  //disegna due manifesti wanted
  drawManifest(35, 0, -12);
  drawManifest(-12, 0, 12);

  drone.Render(); // disegna il drone

  // disegna la scatola bersaglio
  drawCube(drone);

  // attendiamo la fine della rasterizzazione di
  // tutte le primitive mandate

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);

// disegnamo i fps (frame x sec) come una barra a sinistra.
// (vuota = 0 fps, piena = 100 fps)
// impostiamo le matrici per poter disegnare in 2D
  SetCoordToPixel();

  glBegin(GL_QUADS);
  float y=scrH*fps/100;
  float ramp=fps/100;
  glColor3f(1-ramp,0,ramp);
  glVertex2d(10,0);
  glVertex2d(10,y);
  glVertex2d(0,y);
  glVertex2d(0,0);
  glEnd();

  // disegna la mappa in alto a sinistra
  drawMap(scrH, scrW);

  char str[3];
  sprintf(str, "%d", punteggio);
  char text[] = "Milioni di dollari guadagnati: ";
  SDL_GL_DrawText(font, 0, 0, 0, 0, 210, 210, 210, 255, strcat(text, str), scrW-320, scrH-50, shaded);

  actualTime = (SDL_GetTicks() - timeStartGame) /1000;
  sprintf(str, "%u", actualTime);
  char text2[] = "Tempo: ";
  SDL_GL_DrawText(font, 0, 0, 0, 0, 210, 210, 210, 255, strcat(text2, str), scrW-320, scrH-75, shaded);
  //se abilitato, fa confronti di profondità ed aggiorna il buffer di profondità
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);

  //non ritorna finchè tutti gli effetti dei comandi GL chiamati in precedenza non sono completati
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

//disegna la schermata di game over
void gameOver(SDL_Window *win, TTF_Font *font, int scrH, int scrW) {
  // settiamo il viewport
  glViewport(0,0, scrW, scrH);

  // colore di sfondo (fuori dal mondo)
  glClearColor(0,0.4,0,1);

  // riempe tutto lo screen buffer di pixel color sfondo
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  SetCoordToPixel();

  glLineWidth(2);

  // conversione della variabile tempo a stringa
  char tempo[3];
  sprintf(tempo, "%u", actualTime);

  char stringa_punti[] = "Tempo impiegato: ";
  char g_over[] = "GAME OVER";
  char uscire[] = "Premi esc per uscire";

  int distanzaMargine = 73;

  SDL_GL_DrawText(font, 0, 0, 0, 0, 210, 210, 210, 255, strcat(stringa_punti, tempo), scrW/2-distanzaMargine, scrH/2+100, shaded);
  SDL_GL_DrawText(font, 0, 0, 0, 0, 210, 210, 210, 255, g_over, scrW/2-55, scrH/3+20, shaded);
  SDL_GL_DrawText(font, 0, 0, 0, 0, 210, 210, 210, 255, uscire, scrW/2-90, scrH/4+20, shaded);
  glFinish();

  SDL_GL_SwapWindow(win);
}

int main(int argc, char* argv[])
{
SDL_Window *win;
SDL_GLContext mainContext;
Uint32 windowID;
static int keymap[Controller::NKEYS] = {SDLK_a, SDLK_d, SDLK_w, SDLK_s, SDLK_UP, SDLK_DOWN};

  // inizializzazione di SDL
  SDL_Init( SDL_INIT_VIDEO);

  //inizializzo il contatore del tempo dall'inizio della partita
  timeStartGame = SDL_GetTicks();

  //Con libreria SDL_ttf si può gestire il disegno di testo su una
  //finestra grafica usando fonti truetype.
  //SDL_ttf deve essere inizializzato con
  if(TTF_Init() < 0) {
    fprintf(stderr, "Impossibile inizializzare TTF: %s\n",SDL_GetError());
    SDL_Quit();
    return(2);
  }

  TTF_Font *font;
  font = TTF_OpenFont ("FreeSans.ttf", 22);
  if (font == NULL) {
    fprintf (stderr, "Impossibile caricare il font.\n");
  }

  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  //si attiva il double buffer (si inizializza il color buffer, si rende la scena, si scambiano front e back buffer con la swap)
  //Tecnica per nascondere il frame buffer mentre viene riempito
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  // facciamo una finestra di scrW x scrH pixels
  win=SDL_CreateWindow(argv[0], 0, 0, scrW, scrH, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  //Create our opengl context and attach it to our window
  mainContext=SDL_GL_CreateContext(win);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  //glEnable(GL_LIGHT1);
  glEnable(GL_NORMALIZE); // opengl, per favore, rinormalizza le normali
                          // prima di usarle
  //glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW); // consideriamo Front Facing le facce ClockWise
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_POLYGON_OFFSET_FILL); // caro openGL sposta i
                                    // frammenti generati dalla
                                    // rasterizzazione poligoni
  glPolygonOffset(1,1);             // indietro di 1

  if (!LoadTexture(0,(char *)"./images/carbon.jpg")) return -1;
  if (!LoadTexture(1,(char *)"./images/sky_ok.jpg")) return -1;
  if (!LoadTexture(2,(char *)"./images/drugs.jpg")) return -1;
  if (!LoadTexture(3,(char *)"./images/money.jpg")) return -1;
  if (!LoadTexture(4,(char *)"./images/wanted.jpg")) return -1;
  if (!LoadTexture(5,(char *)"./images/asphalt2.jpg")) return -1;


  bool done=0;
  while (!done) {

    SDL_Event e;

    // guardo se c'e' un evento:
    if (SDL_PollEvent(&e)) {
     // se si: processa evento
     switch (e.type) {
      case SDL_KEYDOWN:
        drone.controller.EatKey(e.key.keysym.sym, keymap , true);
        if (e.key.keysym.sym==SDLK_F1) cameraType=(cameraType+1)%CAMERA_TYPE_MAX;
        if (e.key.keysym.sym==SDLK_F2) useWireframe=!useWireframe;
        if (e.key.keysym.sym==SDLK_F3) useEnvmap=!useEnvmap;
        if (e.key.keysym.sym==SDLK_F4) useColors=!useColors;
        if (e.key.keysym.sym==SDLK_F5) useShadow=!useShadow;
        break;
      case SDL_KEYUP:
        drone.controller.EatKey(e.key.keysym.sym, keymap , false);
        break;
      case SDL_QUIT:
          done=1;   break;
      case SDL_WINDOWEVENT:
         // dobbiamo ridisegnare la finestra
          if (e.window.event==SDL_WINDOWEVENT_EXPOSED)
            rendering(win, font);
          else{
           windowID = SDL_GetWindowID(win);
           if (e.window.windowID == windowID)  {
             switch (e.window.event)  {
                  case SDL_WINDOWEVENT_SIZE_CHANGED:  {
                     scrW = e.window.data1;
                     scrH = e.window.data2;
                     glViewport(0,0,scrW,scrH);
                     rendering(win, font);
                     //redraw(); // richiedi ridisegno
                     break;
                  }
             }
           }
         }
      break;

      case SDL_MOUSEMOTION:
        if (e.motion.state & SDL_BUTTON(1) & cameraType==CAMERA_MOUSE) {
          viewAlpha+=e.motion.xrel;
          viewBeta +=e.motion.yrel;
//          if (viewBeta<-90) viewBeta=-90;
          if (viewBeta<+5) viewBeta=+5; //per non andare sotto la macchina
          if (viewBeta>+90) viewBeta=+90;
          // redraw(); // richiedi un ridisegno (non c'e' bisongo: si ridisegna gia'
	               // al ritmo delle computazioni fisiche)
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
  }
    else {
      // nessun evento: siamo IDLE

      Uint32 timeNow=SDL_GetTicks(); // che ore sono?

      if (timeLastInterval+fpsSampling<timeNow) {
        fps = 1000.0*((float)fpsNow) /(timeNow-timeLastInterval);
        fpsNow=0;
        timeLastInterval = timeNow;
      }

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

      if (doneSomething)
      rendering(win, font);
      //redraw();

      if (punteggio == 5){
        printf("Hai messo da parte abbastanza dollari per una vita agiata\n");
        printf("Tempo impiegato %u\n", actualTime);
        done = true;

        int done1 = 0;

        // schermata di game over
        while(!done1) {
          if (SDL_PollEvent(&e)) {
            switch (e.type) {
              case SDL_KEYDOWN:
              if (e.key.keysym.sym==SDLK_ESCAPE) done1 = 1;
                break;
              case SDL_WINDOWEVENT:
                windowID = SDL_GetWindowID(win);
                if (e.window.windowID == windowID) {
                  switch (e.window.event)  {
                    case SDL_WINDOWEVENT_SIZE_CHANGED: {
                      scrW = e.window.data1;
                      scrH = e.window.data2;
                      glViewport(0,0,scrW,scrH);
                      break;
                    }
                  }
                }
            }
          } else { // disegno la schermata di game over
              gameOver(win, font, scrH, scrW);
          }
        }//fine while
      }//fine if punteggio

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

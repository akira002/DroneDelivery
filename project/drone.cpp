// drone.cpp
// implementazione dei metodi definiti in drone.h

#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <vector> // la classe vector di STL

#include "drone.h"
#include "point3.h"
#include "mesh.h"

// var globale di tipo mesh
Mesh droneChassis((char *)"./objects/drone.obj"); // chiama il costruttore
Mesh blade((char *)"./objects/blade.obj");


extern bool useEnvmap; // var globale esterna: per usare l'evnrionment mapping
extern bool useShadow; // var globale esterna: per generare l'ombra

// da invodronee quando e' stato premuto/rilasciato il tasto numero "keycode"
void Controller::EatKey(int keycode, int* keymap, bool pressed_or_released)
{
  for (int i=0; i<NKEYS; i++){
    if (keycode==keymap[i]) key[i]=pressed_or_released;
  }
}

// Funzione che prepara tutto per usare un env map
void SetupEnvmapTexture()
{
  // facciamo binding con la texture 0
  glBindTexture(GL_TEXTURE_2D,0);
  //Le immagini in questa texture sono tutte 2-dimensionali. Hanno altezza e larghezza, ma non profondità.
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_GEN_S); // abilito la generazione automatica delle coord texture S e T (proiettata su oggetto tramite sfera)
  glEnable(GL_TEXTURE_GEN_T);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
  //glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


  glColor3f(1,1,1); // metto il colore neutro (viene moltiplicato col colore texture, componente per componente)
  glDisable(GL_LIGHTING); // disabilito il lighting OpenGL standard (lo faccio con la texture)
}

// DoStep: facciamo un passo di fisica (a delta_t costante)
//
// Indipendente dal rendering.
//
// ricordiamoci che possiamo LEGGERE ma mai SCRIVERE (si usa EatKey)
// la struttura controller da DoStep
void Drone::DoStep(){
  // computiamo l'evolversi della macchina
  static int i=5;

  float vxm, vym, vzm; // velocita' in spazio macchina

  // da vel frame mondo a vel frame macchina
  float cosf = cos(facing*M_PI/180.0);
  float sinf = sin(facing*M_PI/180.0);
  vxm = +cosf*vx - sinf*vz;
  vym = vy;
  vzm = +sinf*vx + cosf*vz;

  // gestione dello sterzo
  if (controller.key[Controller::LEFT]) sterzo+=velSterzo;
  if (controller.key[Controller::RIGHT]) sterzo-=velSterzo;
  sterzo*=velRitornoSterzo; // ritorno a volante dritto

  if (py>0){//può volare solo se si alza da terra
    if (controller.key[Controller::ACC]) vzm-=accMax; // accelerazione in avanti
    if (controller.key[Controller::DEC]) vzm+=accMax; // accelerazione indietro
  }

  //gestione quota
  if (controller.key[Controller::UP]) py += velQuota;//guadagno quota
  if (controller.key[Controller::DOWN]) py -= velQuota;//perdo quota
  if (py < 0) py = 0; // il drone non può andare sottoterra


  // attriti (semplificando)
  vxm*=attritoX;
  vym*=attritoY;
  vzm*=attritoZ;

  // l'orientamento del drone segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  facing = facing - (vzm*grip)*sterzo;

  // rotazione mozzo ruote (costante)
  float da ; //delta angolo
  da = 9;
  mozzo+=da;

  // ritorno a vel coord mondo
  vx = +cosf*vxm + sinf*vzm;
  vy = vym;
  vz = -sinf*vxm + cosf*vzm;

  // posizione = posizione + velocita * delta t (ma delta t e' costante)
  px+=vx;
  py+=vy;
  pz+=vz;

  //setto i bordi dello scenario che il drone non può valicare
  if(pz >= 71) pz = 71;
  if(pz <= -71) pz = -71;
  if(px >= 71) px = 71;
  if(px <= -71) px = -71;
  if(py >= 45) py = 45;

  //setto i bordi del muro
  if (py <= 12 && pz<=3 && pz >= 0) pz = 3;
  if (py <= 12 && pz>=-3 && pz <= 0) pz = -3;
}

void Controller::Init(){
  for (int i=0; i<NKEYS; i++) key[i]=false;
}

void Drone::Init(){
  // inizializzo lo stato della macchina
  px=0; // posizione e orientamento
  facing = 180;
  py=0.0;
  pz=-10;

  mozzo=sterzo=0;   // stato
  vx=vy=vz=0;      // velocita' attuale
  // inizializzo la struttura di controllo
  controller.Init();

  //velSterzo=3.4;         // A
  velSterzo=2.4;         // A
  velRitornoSterzo=0.93; // B, sterzo massimo = A*B / (1-B)

  velQuota = 0.04;

  accMax = 0.0011;

  // attriti: percentuale di velocita' che viene mantenuta
  // 1 = no attrito
  // <<1 = attrito grande
  attritoZ = 0.991;  // piccolo attrito sulla Z (nel senso di rotolamento delle ruote)
  attritoX = 0.8;  // grande attrito sulla X (per non fare slittare la macchina)
  attritoY = 1.0;  // attrito sulla y nullo

  // Nota: vel max = accMax*attritoZ / (1-attritoZ)


  grip = 0.45; // quanto il facing macchina si adegua velocemente allo sterzo
}


// funzione che disegna tutti i pezzi della macchina
// (da invocarsi due volte: per la macchina, e per la sua ombra)
// (se usecolor e' falso, NON sovrascrive il colore corrente
//  e usa quello stabilito prima di chiamare la funzione)
void Drone::RenderAllParts(bool usecolor) const {

  // disegna lo chassis con una mesh
  glPushMatrix();
  if (!useEnvmap)
  {
    if (usecolor) glColor3f(1,0,0);     // colore rosso, da usare con Lighting
  }
  else {
    if (usecolor) SetupEnvmapTexture();
  }
  droneChassis.RenderNxV(); // rendering delle mesh drone usando normali per vertice
  if (usecolor) glEnable(GL_LIGHTING);

  //disabilito la texture per le eliche
  glDisable(GL_TEXTURE_2D);
  glColor3f(.4,.4,.4);

  // elica posteriore D
  glPushMatrix();
  glTranslatef( 0.14,0.04,0.142);
  glRotatef(mozzo, 0, 1, 0);
  // SONO NELLO SPAZIO elica
  //glScalef(0.1, raggioelicaP, raggioelicaP);
  //drawWheel();
  blade.RenderNxV();
  glPopMatrix();

  // elica posteriore S
  glPushMatrix();
  glTranslatef(-0.14,0.04,0.142);
  glRotatef(mozzo,0,1,0);
  //glScalef(0.1, raggioelicaP, raggioelicaP);
  //drawWheel();
  blade.RenderNxV();
  glPopMatrix();

  // elica anteriore D
  glPushMatrix();
  glTranslatef( 0.14,0.04,-0.142);
  //glRotatef(sterzo,0,1,0);
  glRotatef(mozzo,0,1,0);
  //glScalef(0.08, raggioelicaA, raggioelicaA);
  //drawWheel();
  blade.RenderNxV();
  glPopMatrix();

  // elica anteriore S
  glPushMatrix();
  glTranslatef(-0.14,0.04,-0.142);
  //glRotatef(sterzo,0,1,0);
  glRotatef(mozzo,0,1,0);
  //drawAxis();
  //glScalef(0.08, raggioelicaA, raggioelicaA);
  //drawWheel();
  blade.RenderNxV();
  glPopMatrix();

  }

// disegna a schermo
void Drone::Render() const{
  // sono nello spazio mondo

  //drawAxis(); // disegno assi spazio mondo
  glPushMatrix();

  glTranslatef(px,py,pz);
  glRotatef(facing, 0,1,0);

  // sono nello spazio MACCHINA
  //drawAxis(); // disegno assi spazio macchina

  RenderAllParts(true);

  // ombra!
  if(useShadow)
  {
    glTranslatef(0,-py,0); //traslo l'ombra sul terreno
    glScalef(1+py/20,1+py/20,1+py/20);//aumento la dimensione dell'ombra con l'altezza del drone
    glColor3f(0.4,0.4,0.4); // colore fisso
    glTranslatef(0,0.01,0); // alzo l'ombra di un epsilon per evitare z-fighting con il pavimento
    glScalef(1.01,0,1.01);  // appiattisco sulla Y, ingrandisco dell'1% sulla Z e sulla X
    glDisable(GL_LIGHTING); // niente lighing per l'ombra
    RenderAllParts(false);  // disegno il drone appiattito

    glEnable(GL_LIGHTING);
  }
  glPopMatrix();

  glPopMatrix();
}

// drone.cpp
// implementazione dei metodi definiti in drone.h

#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>

#include "drone.h"
#include "point3.h"
#include "mesh.h"

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

// variabili globali di tipo mesh
Mesh droneChassis((char *)"./objects/drone.obj");
Mesh blade((char *)"./objects/blade.obj");

// da invocare quando e' stato premuto/rilasciato il tasto numero "keycode"
void Controller::EatKey(int keycode, int* keymap, bool pressed_or_released)
{
  for (int i=0; i<NKEYS; i++){
    if (keycode==keymap[i]) key[i]=pressed_or_released;
  }
}

// DoStep: facciamo un passo di fisica (a delta-t costante)
//
// Indipendente dal rendering.
//
// ricordiamoci che possiamo LEGGERE ma mai SCRIVERE
// la struttura controller da DoStep
void Drone::DoStep(){
  // computiamo l'evolversi della macchina

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
  sterzo*=velRitornoSterzo; // ritorno a volante fermo
#ifdef DEBUG
  cout << sterzo << endl;
#endif
  //gestione accelerazione
  if (controller.key[Controller::ACC]) vzm-=accMax; // accelerazione in avanti
  if (controller.key[Controller::DEC]) vzm+=accMax; // accelerazione indietro

  //gestione quota
  if (controller.key[Controller::UP]) py += velQuota;//guadagno quota
  if (controller.key[Controller::DOWN]) py -= velQuota;//perdo quota
  if (py < 0) py = 0; // il drone non può andare sottoterra

  // attriti (semplificando)
  vxm*=attritoX;
  vym*=attritoY;
  vzm*=attritoZ;

  // l'orientamento della macchina segue quello dello sterzo
  // (a seconda della velocita' sulla z)
  facing = facing - (vzm*grip)*sterzo;

  // rotazione mozzo ruote (a seconda della velocita' sulla z)
  float da = 9; //delta angolo
  //ad ogni passo aumento l'angolo di rotazione delle eliche di 9 gradi
  mozzo+=da;

  // ritorno a vel coord mondo
  vx = +cosf*vxm + sinf*vzm;
  vy = vym;
  vz = -sinf*vxm + cosf*vzm;

  // posizione = posizione + velocita * delta t (ma e' delta t costante)
  px+=vx;
  py+=vy;
  pz+=vz;
}

void drawCube(); // questa e' definita altrove (quick hack)
void drawAxis(); // anche questa

// disegna una ruota come due cubi intersecati a 45 gradi
void drawWheel(){
  glPushMatrix();
  glScalef(1, 1.0/sqrt(2.0),  1.0/sqrt(2.0));
  drawCube();
  glRotatef(45,  1,0,0);
  drawCube();
  glPopMatrix();
}

void Controller::Init(){
  for (int i=0; i<NKEYS; i++) key[i]=false;
}

void Drone::Init(){
  // inizializzo lo stato della macchina
  px=py=pz=facing=0; // posizione e orientamento
  mozzo=sterzo=0;   // stato
  vx=vy=vz=0;      // velocita' attuale
  // inizializzo la struttura di controllo
  controller.Init();

  velQuota = 0.04;
  velSterzo=3.4;         // A
//  velSterzo=2.26;       // A
  velRitornoSterzo=0.93; // B, sterzo massimo = A*B / (1-B)

  accMax = 0.0011;
  //accMax = 0.0055;

  // attriti: percentuale di velocita' che viene mantenuta
  // 1 = no attrito
  // <<1 = attrito grande
  attritoZ = 0.991;  // piccolo attrito sulla Z (nel senso di rotolamento delle ruote)
  attritoX = 0.8;  // grande attrito sulla X (per non fare slittare la macchina)
  attritoY = 1.0;  // attrito sulla y nullo

  // Nota: vel max = accMax*attritoZ / (1-attritoZ)

  raggioRuotaA = 0.25;
  raggioRuotaP = 0.35;

  grip = 0.45; // quanto il facing macchina si adegua velocemente allo sterzo
}

// disegna carlinga composta da 1 cubo traslato e scalato
static void drawDrone(){

  glColor3f(1,0,0);

  // sono nel frame CAR
  glPushMatrix();

  // vado al frame pezzo_A
  glScalef(0.25 , 0.14 , 1);
  drawCube();

  // torno al frame CAR
  glPopMatrix();

/*
// disegna altri 3 cubi traslati escalati per carlinga
  // vado frame pezzo_B
  glPushMatrix();
  glTranslatef(0,-0.11,-0.95);
  glScalef(0.6, 0.05, 0.15);
  drawCube();
  glPopMatrix();

   // vado frame pezzo_C
  glPushMatrix();
  glTranslatef(0,-0.11,0);
  glScalef(0.6, 0.05, 0.3);
  drawCube();
  glPopMatrix();

  // vado frame pezzo_D
  glPushMatrix();
  glRotatef(-5,1,0,0);
  glTranslatef(0,+0.2,+0.95);
  glScalef(0.6, 0.05, 0.3);
  drawCube();
  glPopMatrix();
*/
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
  //Disegna drone con un parallelepipedo
  //drawDrone();

  //Disegna chassis drone con una mesh
  glPushMatrix();
  //glScalef(2,2,2); // patch: riscaliamo la mesh di 2x
  glTranslatef(0,0,0);
  glColor3f(1,0,0); // colore rosso
  droneChassis.Render();

  glColor3f(.4,.4,.4);

  // ruota posteriore D
  glPushMatrix();
  glTranslatef( 0.195,0.1,0.16);
  glRotatef(mozzo, 0, 1, 0);
  // SONO NELLO SPAZIO RUOTA
  //glScalef(0.1, raggioRuotaP, raggioRuotaP);
  //drawWheel();
  blade.Render();
  glPopMatrix();

  // ruota posteriore S
  glPushMatrix();
  glTranslatef(-0.195,0.1,0.16);
  glRotatef(mozzo,0,1,0);
  //glScalef(0.1, raggioRuotaP, raggioRuotaP);
  //drawWheel();
  blade.Render();
  glPopMatrix();

  // ruota anteriore D
  glPushMatrix();
  glTranslatef( 0.195,0.1,-0.16);
  //glRotatef(sterzo,0,1,0);
  glRotatef(mozzo,0,1,0);
  //glScalef(0.08, raggioRuotaA, raggioRuotaA);
  //drawWheel();
  blade.Render();
  glPopMatrix();

  // ruota anteriore S
  glPushMatrix();
  glTranslatef(-0.195,0.1,-0.16);
  //glRotatef(sterzo,0,1,0);
  glRotatef(mozzo,0,1,0);
  //drawAxis();
  //glScalef(0.08, raggioRuotaA, raggioRuotaA);
  //drawWheel();
  blade.Render();
  glPopMatrix();

  glPopMatrix();
}

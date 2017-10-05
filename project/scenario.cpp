#include "scenario.h"

// caricamento degli oggetti mesh da file
Mesh muro((char *)"./objects/wall.obj");

bool mustCreateNewCube = true; // devo creare un nuovo cubo
bool generatore = true; // devo inizializzare il seme
bool isDrugs = true;
int pos_x, pos_y, pos_z; // posizione del cubo

extern int punteggio;

void drawFloor()
{
  const float S=100; // size
  const float H=0;   // altezza
  const int K=150; //disegna K x K quads

  // disegna KxK quads
  glBegin(GL_QUADS);
    glColor3f(0.6, 0.6, 0.6); // colore uguale x tutti i quads
    glNormal3f(0,1,0);       // normale verticale uguale x tutti
    for (int x=0; x<K; x++)
    for (int z=0; z<K; z++) {
      float x0=-S + 2*(x+0)*S/K;
      float x1=-S + 2*(x+1)*S/K;
      float z0=-S + 2*(z+0)*S/K;
      float z1=-S + 2*(z+1)*S/K;
      glVertex3d(x0, H, z0);
      glVertex3d(x1, H, z0);
      glVertex3d(x1, H, z1);
      glVertex3d(x0, H, z1);
    }
  glEnd();
}

void drawMuro () {
        glPushMatrix();
        glColor3f(0.1,0.1,.1);
        glScalef(3.8, 1.0, 0.75);
        glTranslatef(0,0,0);
        //muro.RenderNxV();
        muro.RenderNxF();
        glPopMatrix();
}

void drawCube(Drone drone) {

  // se devo inizializzare il seme
  if(generatore){
    srand(time(NULL));
    generatore = false;
  }

  /* disegno del primo cubo */

  // se devo rigenerare il cubo
  if(mustCreateNewCube) {

    pos_y = 0; //le scatole stanno sempre appoggiate al terreno
    pos_x = (rand()%59+1)-30;
    if (isDrugs){
        do {
          pos_z = (rand()%59+1)-30;
        } while (!(pos_z>3)); //le scatole di droga devono sempre stare da un lato del muro
    } else {
      do {
        pos_z = (rand()%59+1)-30;
      } while (!(pos_z<-3)); //quelle di denaro devono sempre stare dall'altro
    }
    mustCreateNewCube = false;
    //printf("COORD1: %d %d %d\n", pos_x, pos_y, pos_z);
  }

  // disegno del cubo con una texture personale su tutti e sei i lati
  glPushMatrix();
  if (isDrugs){
    glBindTexture(GL_TEXTURE_2D, 2);
  }
  else {
    glBindTexture(GL_TEXTURE_2D, 3);
  }
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

  glTranslatef(pos_x,pos_y,pos_z);
  glTranslatef(2, 2, 2);
  //glRotatef(mozzo,1,1,0);
  glRotatef(0,1,1,0);
  glTranslatef(-2, -2, -2);

  glBegin(GL_QUADS);
      /* Front. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 1.0, 3.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 1.0, 3.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 3.0, 3.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 3.0, 3.0);

      /* Down. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 1.0, 1.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 1.0, 1.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 1.0, 3.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 1.0, 3.0);

      /* Back. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 3.0, 1.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 3.0, 1.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 1.0, 1.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 1.0, 1.0);

      /* Up. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 3.0, 3.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 3.0, 3.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 3.0, 1.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 3.0, 1.0);

      /* SideLeft. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 3.0, 1.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(1.0, 3.0, 3.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(1.0, 1.0, 3.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 1.0, 1.0);

      /* SideRight. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(3.0, 3.0, 1.0);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(3.0, 3.0, 3.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(3.0, 1.0, 3.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(3.0, 1.0, 1.0);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glLineWidth(1);

    glColor3f(0,0,0);
    glBegin(GL_LINE_LOOP);
      glVertex3f(1.0, 1.0, 3.0);
      glVertex3f(3.0, 1.0, 3.0);
      glVertex3f(3.0, 3.0, 3.0);
      glVertex3f(1.0, 3.0, 3.0);
    glEnd();

    glBegin(GL_LINE_LOOP);
      glVertex3f(1.0, 3.0, 1.0);
      glVertex3f(3.0, 3.0, 1.0);
      glVertex3f(3.0, 1.0, 1.0);
      glVertex3f(1.0, 1.0, 1.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(1.0, 1.0, 3.0);
    glVertex3f(1.0, 1.0, 1.0);

    glVertex3f(3.0, 1.0, 3.0);
    glVertex3f(3.0, 1.0, 1.0);

    glVertex3f(1.0, 3.0, 3.0);
    glVertex3f(1.0, 3.0, 1.0);

    glVertex3f(3.0, 3.0, 3.0);
    glVertex3f(3.0, 3.0, 1.0);
    glEnd();

    // se il drone ha caricato il cubo (ci è entrato)
    if (drone.px >= pos_x - 3 && drone.px <= pos_x + 3 &&
        drone.py >= pos_y - 3 && drone.py <= pos_y + 3 &&
        drone.pz >= pos_z - 4 && drone.pz <= pos_z + 4) {
          if (!isDrugs){//se sono stato pagato con del denaro
              punteggio++;
          }
          mustCreateNewCube = true;
          isDrugs = !isDrugs;
          printf("Punteggio: %d\n", punteggio);
    }

glPopMatrix();
}

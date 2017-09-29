#include "scenario.h"

bool mustCreateNewCube = true; // devo creare un nuovo cubo
bool generatore = true; // devo inizializzare il seme
bool isDrugs = true;
int pos_x, pos_y, pos_z; // posizione del cubo

extern int punteggio;

void drawCube(Drone drone) {

  // se devo inizializzare il seme
  if(generatore){
    srand(time(NULL));
    generatore = false;
  }

  /* disegno del primo cubo */

  // se devo rigenerare il cubo
  if(mustCreateNewCube) {
    pos_x = (rand()%59+1)-30;
    //pos_y = (rand()%29+1);
    pos_y = 0; //i cubi stanno sempre appoggiati al terreno
    pos_z = (rand()%59+1)-30;
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

#include "scenario.h"

// caricamento degli oggetti mesh da file
Mesh muro((char *)"./objects/wall.obj");

bool mustCreateNewCube = true; // devo creare un nuovo cubo
bool generatore = true; // devo inizializzare il seme
bool isDrugs = true;
int pos_x, pos_y, pos_z; // posizione del cubo

extern int punteggio;
extern bool useColors;
extern bool useWireframe;

void drawFloor()
{
  const float S=100; // size
  const float H=0;   // altezza
  const int K=150; //disegna K x K quads

  if(!useColors) {
    // disegno il terreno ripetendo una texture su di esso
    glBindTexture(GL_TEXTURE_2D, 5);
    glEnable(GL_TEXTURE_2D);
    //disabilito la generazione automatica delle coord texture S e T
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    //setto i parametri del texture environment
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE ); //tells the renderer to skip the current color and just use the texel colors
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //then if you make a call to glColorx() your textures will be filtered to that color too
  }

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
      if(!useColors) glTexCoord2f(0.0, 0.0);
      glVertex3d(x0, H, z0);
      if(!useColors) glTexCoord2f(1.0, 0.0);
      glVertex3d(x1, H, z0);
      if(!useColors) glTexCoord2f(1.0, 1.0);
      glVertex3d(x1, H, z1);
      if(!useColors) glTexCoord2f(0.0, 1.0);
      glVertex3d(x0, H, z1);
    }
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

void drawMuro () {
        glPushMatrix();
        glColor3f(0.1,0.1,.1);
        glScalef(3.8, 1.0, 0.75);
        glTranslatef(0,0,0);
        //muro.RenderNxV(); //render normali per vertice (gouraud shading)
        muro.RenderNxF(); //render normali per faccia (flat shading)
        glPopMatrix();
}

void drawManifest (int x, int y, int z){
  // disegno del manifesto con una texture personale su tutti e sei i lati
  if (!useColors){
    glBindTexture(GL_TEXTURE_2D, 4);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }

  glPushMatrix();
  glTranslatef(x, y, z);
  glTranslatef(0, -1, 0);
  glBegin(GL_QUADS);
      /* Front. */
      glTexCoord2f(0.0, 0.0);
      glVertex3f(1.0, 6.0, 1.2);
      glTexCoord2f(1.0, 0.0);
      glVertex3f(4.0, 6.0, 1.2);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(4.0, 1.0, 1.2);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(1.0, 1.0, 1.2);


      /* Down. */
      glVertex3f(1.0, 1.0, 1.0);
      glVertex3f(4.0, 1.0, 1.0);
      glVertex3f(4.0, 1.0, 1.2);
      glVertex3f(1.0, 1.0, 1.2);

      /* Back. */
      glTexCoord2f(1.0, 0.0);
      glVertex3f(1.0, 6.0, 1.0);
      glTexCoord2f(0.0, 0.0);
      glVertex3f(4.0, 6.0, 1.0);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(4.0, 1.0, 1.0);
      glTexCoord2f(1.0, 1.0);
      glVertex3f(1.0, 1.0, 1.0);

      /* Up. */
      glVertex3f(1.0, 6.0, 1.0);
      glVertex3f(4.0, 6.0, 1.0);
      glVertex3f(4.0, 6.0, 1.2);
      glVertex3f(1.0, 6.0, 1.2);

      /* SideLeft. */
      glVertex3f(1.0, 6.0, 1.0);
      glVertex3f(1.0, 6.0, 1.2);
      glVertex3f(1.0, 1.0, 1.2);
      glVertex3f(1.0, 1.0, 1.0);

      /* SideRight. */
      glVertex3f(4.0, 6.0, 1.0);
      glVertex3f(4.0, 6.0, 1.2);
      glVertex3f(4.0, 1.0, 1.2);
      glVertex3f(4.0, 1.0, 1.0);

    glEnd();
    glDisable(GL_TEXTURE_2D);


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
    pos_x = (rand()%69+1)-20;
    if (isDrugs){
        do {
          pos_z = (rand()%69+1)-20;
        } while (!(pos_z>3)); //le scatole di droga devono sempre stare da un lato del muro
    } else {
      do {
        pos_z = (rand()%69+1)-20;
      } while (!(pos_z<-3)); //quelle di denaro devono sempre stare dall'altro
    }
    mustCreateNewCube = false;
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
  //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glTranslatef(pos_x,pos_y -1,pos_z); //altrimenti il cubo fluttua

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
          //printf("Punteggio: %d\n", punteggio);
    }

glPopMatrix();
}

void drawSphere(double r, int lats, int longs) {
int i, j;
  for(i = 0; i <= lats; i++) {
     double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
     double z0  = sin(lat0);
     double zr0 =  cos(lat0);

     double lat1 = M_PI * (-0.5 + (double) i / lats);
     double z1 = sin(lat1);
     double zr1 = cos(lat1);

     glBegin(GL_QUAD_STRIP);
     for(j = 0; j <= longs; j++) {
        double lng = 2 * M_PI * (double) (j - 1) / longs;
        double x = cos(lng);
        double y = sin(lng);

//le normali servono per l'EnvMap
        glNormal3f(x * zr0, y * zr0, z0);
        glVertex3f(r * x * zr0, r * y * zr0, r * z0);
        glNormal3f(x * zr1, y * zr1, z1);
        glVertex3f(r * x * zr1, r * y * zr1, r * z1);
     }
     glEnd();
  }
}

void drawSky() {
int H = 100;

  if (useWireframe) {
    glDisable(GL_TEXTURE_2D);
    glColor3f(0,0,0);
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    drawSphere(100.0, 20, 20);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glColor3f(1,1,1);
    glEnable(GL_LIGHTING);
  }
  else
  {
        glBindTexture(GL_TEXTURE_2D,1);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP); // Env map
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP);
        //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1,1,1);
        glDisable(GL_LIGHTING);

     //   drawCubeFill();
        drawSphere(100.0, 20, 20);

        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
  }

}

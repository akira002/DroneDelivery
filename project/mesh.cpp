// file mesh.cpp
//
// Implementazione dei metodi di Mesh

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <GL/gl.h>

#include "point3.h"
#include "mesh.h"

void Mesh::ComputeNormalsPerFace()
{
  for (int i=0; i<f.size(); i++)
  {
    f[i].ComputeNormal();
  }
}

void Mesh::Render()
{
  // mandiamo tutti i triangoli a schermo
  //definisce la geometria 3D
  glBegin(GL_TRIANGLES);
  for (int i=0; i<f.size(); i++)
  {
    f[i].n.SendAsNormal(); // flat shading
    (f[i].v[0])->p.SendAsVertex();

    (f[i].v[1])->p.SendAsVertex();

    (f[i].v[2])->p.SendAsVertex();
  }
  glEnd();
}

void Mesh::ComputeBoundingBox(){
  if (!v.size()) return;
  bbmin=bbmax=v[0].p;
  for (int i=0; i<v.size(); i++){
    for (int k=0; k<3; k++) {
      if (bbmin.coord[k]>v[i].p.coord[k]) bbmin.coord[k]=v[i].p.coord[k];
      if (bbmax.coord[k]<v[i].p.coord[k]) bbmax.coord[k]=v[i].p.coord[k];
    }
  }
}

bool Mesh::LoadFromObj(char* filename)
{

  FILE* file = fopen(filename, "rt"); // apriamo il file in lettura
  if (!file) return false;


  //make a first pass through the file to get a count of the number
  //of vertices, normals, texcoords & triangles
  char buf[128];
  int nv,nf,nt;
  float x,y,z;
  int va,vb,vc,vd;
  int na,nb,nc,nd;
  int ta,tb,tc,td;

  nv=0; nf=0; nt=0;
  while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               // comment
            // eat up rest of line
            //legge una riga intera fino ad un newline (o EOF)
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':               // v, vn, vt
            switch(buf[1]) {
            case '\0':          // vertex
                // eat up rest of line
                fgets(buf, sizeof(buf), file);
                //incrementa il contatore del numero di vertici
                nv++;
                break;
            default:
                break;
            }
            break;
         case 'f':               // face
                fscanf(file, "%s", buf);
                // can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d
                if (strstr(buf, "//")) {//se ha come sottostringa "//"
                    // v//n
                    sscanf(buf, "%d//%d", &va, &na); //additional arguments each containing a pointer to allocated storage where the interpretation of the extracted characters is stored
                    fscanf(file, "%d//%d", &vb, &nb);
                    fscanf(file, "%d//%d", &vc, &nc);
                    nf++;
                    nt++;
                    while(fscanf(file, "%d//%d", &vd, &nd) > 0) {
                        nt++;
                    }
                } else if (sscanf(buf, "%d/%d/%d", &va, &ta, &na) == 3) {//returns the number of items in the argument list successfully filled
                    // v/t/n
                    fscanf(file, "%d/%d/%d", &vb, &tb, &nb);
                    fscanf(file, "%d/%d/%d", &vc, &tc, &nc);
                    nf++;
                    nt++;
                    while(fscanf(file, "%d/%d/%d", &vd, &td, &nd) > 0) {
                        nt++;
                    }
                 } else if (sscanf(buf, "%d/%d", &va, &ta) == 2) {//returns the number of items in the argument list successfully filled
                    // v/t
                    fscanf(file, "%d/%d", &vb, &tb);
                    fscanf(file, "%d/%d", &vc, &tc);
                    nf++;
                    nt++;
                    while(fscanf(file, "%d/%d", &vd, &td) > 0) {
                        nt++;
                    }
                } else {
                    // v
                    fscanf(file, "%d", &va);
                    fscanf(file, "%d", &vb);
                    nf++;
                    nt++;
                    while(fscanf(file, "%d", &vc) > 0) {
                        nt++;
                    }
                }
                break;

            default:
                // eat up rest of line
                fgets(buf, sizeof(buf), file);
                break;
        }
   }

//printf("dopo FirstPass nv=%d nf=%d nt=%d\n",nv,nf,nt);

  // allochiamo spazio per nv vertici
  v.resize(nv);

  // rewind to beginning of file and read in the data this pass
  rewind(file);

  //on the second pass through the file, read all the data into the
  //allocated arrays
  //abbiamo già gli array per vertici e facce allocati della giusta dimensione, ora li riempiamo coi dati
  nv = 0;
  nt = 0;
  while(fscanf(file, "%s", buf) != EOF) {
      switch(buf[0]) {
      case '#':               // comment
            // eat up rest of line
            fgets(buf, sizeof(buf), file);
            break;
      case 'v':               // v, vn, vt
            switch(buf[1]) {
            case '\0':          // vertex
                fscanf(file, "%f %f %f", &x, &y, &z);
                //memorizza un punto nell'array di punti
                v[nv].p = Point3( x,y,z );
                nv++;
                break;
            default:
                break;
            }
            break;
     case 'f':               // face
           fscanf(file, "%s", buf);
           // can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d
           if (strstr(buf, "//")) {
              // v//n
              sscanf(buf, "%d//%d", &va, &na);
              fscanf(file, "%d//%d", &vb, &nb);
              fscanf(file, "%d//%d", &vc, &nc);
              va--; vb--; vc--;
              Face newface( &(v[va]), &(v[vc]), &(v[vb]) ); // invoco il costruttore di faccia
              f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
              nt++;
              vb=vc;
              while(fscanf(file, "%d//%d", &vc, &nc) > 0) {
                 vc--;
                 Face newface( &(v[va]), &(v[vc]), &(v[vb]) ); // invoco il costruttore di faccia
                 f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                 nt++;
                 vb=vc;
              }
           } else if (sscanf(buf, "%d/%d/%d", &va, &ta, &na) == 3) {
                // v/t/n
                fscanf(file, "%d/%d/%d", &vb, &tb, &nb);
                fscanf(file, "%d/%d/%d", &vc, &tc, &nc);
                va--; vb--; vc--;
                Face newface( &(v[va]), &(v[vc]), &(v[vb]) ); // invoco il costruttore di faccia
                f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                nt++;
                vb=vc;
                while(fscanf(file, "%d/%d/%d", &vc, &tc, &nc) > 0) {
                   vc--;
                   Face newface( &(v[va]), &(v[vc]), &(v[vb]) ); // invoco il costruttore di faccia
                   f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                   nt++;
                   vb=vc;
                }
           } else if (sscanf(buf, "%d/%d", &va, &ta) == 2) {
                // v/t
                fscanf(file, "%d/%d", &va, &ta);
                fscanf(file, "%d/%d", &va, &ta);
                va--; vb--; vc--;
                Face newface( &(v[va]), &(v[vc]), &(v[vb]) ); // invoco il costruttore di faccia
                f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                nt++;
                vb=vc;
                while(fscanf(file, "%d/%d", &vc, &tc) > 0) {
                   vc--;
                   Face newface( &(v[va]), &(v[vc]), &(v[vb]) ); // invoco il costruttore di faccia
                   f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                   nt++;
                   vb=vc;
                }
           } else {
                // v
                sscanf(buf, "%d", &va);
                fscanf(file, "%d", &vb);
                fscanf(file, "%d", &vc);
                va--; vb--; vc--;
                Face newface( &(v[va]), &(v[vc]), &(v[vb]) ); // invoco il costruttore di faccia
                f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                nt++;
                vb=vc;
                while(fscanf(file, "%d", &vc) > 0) {
                   vc--;
                   Face newface( &(v[va]), &(v[vc]), &(v[vb]) ); // invoco il costruttore di faccia
                   f.push_back( newface ); // inserico la nuova faccia in coda al vettore facce
                   nt++;
                   vb=vc;
                }
            }
            break;

            default:
                // eat up rest of line
                fgets(buf, sizeof(buf), file);
                break;
    }
  }

//printf("dopo SecondPass nv=%d nt=%d\n",nv,nt);

  fclose(file);
  return true;
}

class Controller{
public:
  enum { LEFT=0, RIGHT=1, ACC=2, DEC=3, UP=4, DOWN=5, NKEYS=6};
  bool key[NKEYS];
  void Init();
  void EatKey(int keycode, int* keymap, bool pressed_or_released);
  void Joy(int keymap, bool pressed_or_released);
  Controller(){Init();} // costruttore
};


class Drone{

  void RenderAllParts(bool usecolor) const;
                         // disegna tutte le parti della macchina
                         // invocato due volte: per la car e la sua ombra

public:
  // Metodi
  void Init(); // inizializza variabili
  void Render() const; // disegna a schermo
  void DoStep(); // computa un passo del motore fisico
  Drone(){Init();} // costruttore

  Controller controller;

  // STATO DELLA MACCHINA
  // (DoStep fa evolvere queste variabili nel tempo)
  float px,py,pz,facing; // posizione e orientamento
  float mozzo, sterzo; // stato interno
  float vx,vy,vz; // velocita' attuale

  // STATS DELLA MACCHINA
  // (di solito rimangono costanti)
  float velSterzo, velRitornoSterzo, accMax, attrito,
        raggioRuotaA, raggioRuotaP, grip,
        attritoX, attritoY, attritoZ; // attriti
  float velQuota; // velocità di aumento o diminuzione quota


private:
  void DrawHeadlight(float x, float y, float z, int lightN, bool useHeadlight) const;
};

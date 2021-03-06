
/* 
 * File:   Inbetween.h
 * Author: danielfinestrat
 *
 * Created on 16 de mayo de 2017, 19:43
 */

#ifndef INBETWEEN_H
#define INBETWEEN_H

#include <vector>
#include "motorgrafico/VisibleBody.h"
#include "motorgrafico/resources.h"
#include "motorgrafico/Fondo.h"

using namespace std;

class Inbetween {
public:
    Inbetween(vector<Player*> jugadores, int maxPoints, int modoJuego, bool gameOver);
    void Update();
    void render();
    void renderCrowns();
    void renderFaces();
    void renderPunt();
    virtual ~Inbetween();

private:
    vector<VisibleBody*> playerVB;
    vector<VisibleBody*> coronaVB;
    vector<Texto*> puntuacionText;
    vector<int> puntuacion;
    Texto* modoJuegoTxt;
    Fondo* fondo;
    bool isRendered;

};

#endif /* INBETWEEN_H */


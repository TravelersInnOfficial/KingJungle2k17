
/* 
 * File:   Partida.h
 * Author: daniel
 *
 * Created on 26 de marzo de 2017, 13:13
 */

#ifndef PARTIDA_H
#define PARTIDA_H

#include <vector>
#include <set>

#include "motorfisico/Motorfisico.h"
#include "motorgrafico/Temporizador.h"
#include "Hud.h"
#include "Weaponspawner.h"
#include "Platform.h"
#include "Weapon.h"
#include "Bala.h"
#include "Explosion.h"
#include "PlayerJoystick.h"
#include "PlayerKeyboard.h"
#include "Mapa.h"
#include "Console.h"
#include "Trampa.h"
#include "TrapSpawner.h"
#include "IAController.h"
#include "Inbetween.h"
#include "Coin.h"
#include "Coinsspawner.h"

using namespace std;

#define screenWidth 1024
#define screenHeight 600

#define TIMESTEP 1.0f/15.f      //TIEMPO DE REFRESCO
#define VELITER 10              //NUMERO DE ITERACION POR TICK PARA CALCULAR LA VELOCIDAD
#define POSITER 10              //NUMERO DE ITERACION POR TICK PARA CALCULAR LA POSICION

class Motorgrafico;

struct checkLoadingLevel {
    bool loadingLevel;
    bool showingInbetween;
    bool firstTextPrepared;
    bool secondTextPrepared;
    bool thirdTextPrepared;
};

struct Ronda {
    int mapa;
    int modoJuego;
};

class Partida {
public:

    Mapa *mapa;
    Weaponspawner* factoriaArmas;
    TrapSpawner* factoriaTrampas;
    Coinsspawner* factoriaCoins;
    Console console;
    int indexMap;
    int maxPoints;

    InnerClock changeLevelClock;
    bool lastLevel;
    bool notFirstReset;
    bool finalLevelTextPrepared;
    bool gameisover;
    float timeBetweenReset;

    checkLoadingLevel loadingLevelStruct;
    Inbetween* inbetween;

    vector<Ronda> rondas;
    int rondaActual;

    vector<Weapon*> worldWeapons;
    set<Bala*> worldBullets;
    set<Explosion*> worldExplo;
    vector<Texto*> worldTexts;
    vector<Player*> worldPlayer;
    vector<Trampa*> worldTraps;
    vector<Controlador*> worldControlador;
    vector<Coin*> worldCoins;

    vector<Player*> players2Delete;
    vector<Platform*> platforms2Delete;
    vector<Weapon*> weapons2Delete;
    vector<Trampa*> traps2Delete;
    set<Bala*> bullets2Delete;
    set<Explosion*> explo2Delete;
    vector<Trampa*> traps2Break;
    vector<Coin*> coins2Delete;

    static Partida* getInstance();

    void Input(int &e);
    void Update();
    void Erase();
    void Render();

    void drawPlatforms();
    void drawPlayers();
    void drawWeapons();
    void drawBullets();
    void drawExplo();
    void drawTraps();
    void drawTexts();
    void drawMainText();
    void drawCoins();

    void eraseBullets();
    void erasePlayers();
    void eraseWeapons();
    void eraseExplo();
    void eraseTraps();
    void erasePlatforms();
    void eraseCoins();

    int findKeyboardControlador();
    int findControladorWithId(int id);
    void checkJoysticksConnected();
    void addPlayerJoystick(int id);
    void addPlayerKeyboard();
    void addPlayerIA();
    void respawn();

    void updatePlayers(Time frameTime);
    void updateBullets();
    void updateWeapons();
    void updateExplo();
    void updateTraps();
    void updateIA();
    void updateClock();
    void updateTexts();
    void inbetweenUpdate();
    void updateBeforeMap();
    void updateCoins(Time frameTime);

    void breakTraps();
    void ordenarJugadores();

    void cameraSetFinalTransform();

    bool getUsingKeyboard();
    void setUsingKeyboard(bool state);

    void setMaxPoints(int points);

    void fillRondasVector(int tipoRonda);
    void startTextBeforeLevel();
    void startGameOverInbetweener();
    void finishRound();
    void loadTextsNClock();

    void loadMap();
    void loadMap(string mapaStr);
    void loadFinalMap();

    virtual ~Partida();

private:

    Motorgrafico *mg;
    bool usingKeyboard;
    Hud *hud;

    Partida();
};

#endif /* PARTIDA_H */
/* 
 * File:   Partida.h
 * Author: daniel
 *
 * Created on 26 de marzo de 2017, 13:13
 */

#ifndef PARTIDA_H
#define PARTIDA_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "../Box2D/Box2D.h"
#include "Temporizador.h"
#include "Platform.h"
#include "Weapon.h"
#include "Bala.h"
#include "Explosion.h"
#include "PlayerJoystick.h"
#include "ContactListener.h"
#include "PlayerKeyboard.h"

using namespace std;

#define screenWidth 1024
#define screenHeight 600

#define TIMESTEP 1.0f/15.f      //TIEMPO DE REFRESCO
#define VELITER 10              //NUMERO DE ITERACION POR TICK PARA CALCULAR LA VELOCIDAD
#define POSITER 10              //NUMERO DE ITERACION POR TICK PARA CALCULAR LA POSICION

class Partida {
public:
    b2World *world;
    ContactListener myContactListener;
    sf::RenderWindow *window;
    
    sf::View *mainCamera;
    sf::View *hudCamera;

    vector<Platform*> worldPlatforms;
    vector<Weapon*> worldWeapons;
    set<Bala*> worldBullets;
    set<Explosion*> worldExplo;
    vector<PlayerJoystick> playerJoysticks;
    PlayerKeyboard* playerKeyboard;

    set<Bala*> bullets2Delete;
    vector<Player*> players2Delete;
    set<Explosion*> explo2Delete;
    
    Temporizador *temporizador;
    Clock frameClock;

    static Partida* getInstance();

    void Input();
    void Update();
    void Erase();
    void Render();

    void drawPlatforms();
    void drawPlayers();
    void drawWeapons();
    void drawBullets();
    void drawExplo();
    
    void eraseBullets();
    void erasePlayers();
    void eraseExplo();
    
    int findJoystickWithId(vector<PlayerJoystick> *playerJoysticks, int id);
    void checkJoysticksConnected();
    void addPlayerJoystick(vector<PlayerJoystick> *playerJoysticks, int id);
    void addPlayerKeyboard();
    void respawn();
    
    void updatePlayers(Time frameTime, vector<PlayerJoystick> *playerJoysticks);
    void updateBullets();
    void updateWeapons();
    void updateExplo();

    void cameraSetTransform();
    
    void loadMap();
    virtual ~Partida();

private:
    
    Partida();
    Partida(const Partida& orig);
    bool usingKeyboard;

};

#endif /* PARTIDA_H */

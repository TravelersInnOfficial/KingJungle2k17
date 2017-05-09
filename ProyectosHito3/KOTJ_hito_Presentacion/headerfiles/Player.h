
/* 
 * File:   Player.h
 * Author: amygwyllion
 *
 * Created on 21 de marzo de 2017, 8:00
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include "Entidad.h"
#include "../motorgrafico/headerfiles/Animation.h"
#include "../motorgrafico/headerfiles/SpriteAnimated.h"
#include "../motorfisico/headerfiles/Cuerpo.h"
#include "Weapon.h"

using namespace std;

class Player : public Entidad {
public:

    Player();
    virtual ~Player();
    void setStandAnimation();
    void setWalkingAnimation();
    void setJumpAnimation();
    void setFallAnimation();
    void setDuckAnimation();
    void setDeadAnimation();
    void playAnimation();
    Animation* getCurrentAnimation();
    void update(Time);
    SpriteAnimated& getPlayerSprite();
    void jump();
    bool isGrounded();
    bool updateCanJumpStateState();
    void changeDirection(int);
    void move();
    void duck(int);
    void die(int);
    bool isPlayerDead();
    void mock(); //TODO
    void shoot();
    void respawn();
    void eraseBody();
    void interact();
    void interact(Weapon* lastWeapon);
    void pause();
    bool getPaused();
    float getPositionX();
    float getPositionY();
    void setPosition(float, float);
    int getDirMoving();
    void setColor(int);
    bool hasWeapon();
    Weapon* getWeapon();
    bool isJumping();
    void checkMapBounds();
    void Contact(void* punt, string tipo);
    
    void give1Point();
    int getPoints();

private:
    Cuerpo *cuerpo;

    Animation standAnimation; //Animacion de estar quieto
    Animation walkingAnimation; //Animacion de andar
    Animation jumpAnimation; //Animacion de saltar
    Animation fallAnimation; //Animacion de caer
    Animation duckAnimation; //Animacion de agacharse
    Animation deadAnimation; //Animacion de muerte
    Animation* currentAnimation; //Animacion actual
    SpriteAnimated* playerSprite; //Sprite del jugador

    Weapon *weapon;

    int dirMoving;
    int dirLooking;

    bool updateCanJumpState;
    bool canJump;

    bool isDucking;
    bool isDead;
    bool paused;

    float fuerzaSalto;
    float fuerzaMovimiento;
    float velocidadMaxima;
    
    int points;
};

#endif /* PLAYER_H */


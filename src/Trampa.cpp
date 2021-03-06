
/* 
 * File:   Trampa.cpp
 * Author: youjair
 * 
 * Created on 26 de marzo de 2017, 18:36
 */

#include "Trampa.h"
#include "motorfisico/Motorfisico.h"
#include "Partida.h"
#include <stdio.h>
#include <PI.h>
#include <iostream>
#include <set>
#include <vector>

#define VELTRAP 3 //Velocidad que tendra la trampa

using namespace std;
using namespace sf;

Trampa::Trampa(float sizeX, float sizeY, float posX, float posY, int type, float angle, int tiempoDes, int tiempoAct) {
    tag = "Trampa";
    id = type;
    estado = false;
    roto = false;
    m_SizeX = sizeX;
    m_SizeY = sizeY;
    iniPosX = posX;
    iniPosY = posY;
    timeDes = tiempoDes;
    timeAct = tiempoAct;
    reloj.restartClock();
    difTime = 0;

    cuerpo = Motorfisico::getInstance()->crearCuerpo(posX + sizeX / 2, posY + sizeY / 2, sizeX, sizeY, angle, this);
    cuerpo->setCategoryBits(CATEGORY_TRAMPA);
    m_vBody = new VisibleBody(posX + sizeX / 2, posY + sizeY / 2, sizeX, sizeY, "", true);


    switch (id) {
        case 0: //Pinchos
            cuerpo->setType(2);
            cuerpo->setMaskBits(MASK_TRAMPA1); //Colisiona con jugadores
            cuerpo->setSensor(true);
            m_vBody->setTex(Resources::getInstance()->spikes);
            break;
        case 1: //Zona de muerte solida
            cuerpo->setType(0);
            cuerpo->setMaskBits(MASK_TRAMPA2); //Colisiona con balas y jugadores
            m_vBody->setTex(Resources::getInstance()->transparente);
            estado = true;
            break;
        case 2: //Zona de muerte atravesable
            cuerpo->setType(0);
            cuerpo->setMaskBits(MASK_TRAMPA1);
            cuerpo->setSensor(true);
            m_vBody->setTex(Resources::getInstance()->transparente);
            estado = true;

            break;
        case 3: //TNT
            cuerpo->setType(1);
            cuerpo->setMaskBits(MASK_TRAMPA2);
            m_vBody->setTex(Resources::getInstance()->tnt);
            break;
        case 4: //Nitroglicerina
            cuerpo->setType(1);
            cuerpo->setMaskBits(MASK_TRAMPA2);
            m_vBody->setTex(Resources::getInstance()->nitro);
            break;
        case 5: //Palanca
            cuerpo->setType(0);
            cuerpo->setMaskBits(MASK_TRAMPA1);
            cuerpo->setSensor(true);
            m_vBody->setTex("");
            break;
        case 6: //Placa de presion
            cuerpo->setType(0);
            cuerpo->setMaskBits(MASK_TRAMPA1);
            cuerpo->setSensor(true);
            m_vBody->setTex("");
            break;
    }
}

Trampa::Trampa(const Trampa& orig) {
}

Trampa::~Trampa() {
    cuerpo->Destruir();
    delete cuerpo;
    cuerpo = NULL;
    delete m_vBody;
    m_vBody = NULL;
}

void Trampa::activar() {
    int velY;

    if (!estado) {
        estado = true;
        switch (id) {
            case 0: //Pinchos
                velY = -VELTRAP;
                cuerpo->setVelocidadY(velY);
                break;
            case 3: //TNT
                if(roto == false){
                    roto = true;
                    
                    Partida *partida = Partida::getInstance();
                    partida->traps2Break.push_back(this);
                }
            case 4://Nitro
                if(roto == false){
                    roto = true;
                    
                    Partida *partida = Partida::getInstance();
                    partida->traps2Break.push_back(this);
                }
                //  case 5:
                // case 6:
            default:
                break;
        }


    }

}

void Trampa::desactivar() {
    int velY;



    if (estado) {
        estado = false;
        switch (id) {
            case 0: //pinchos
                velY = VELTRAP;
                cuerpo->setVelocidadY(velY);
                break;
                // case 5:
                // case 6:
            default:
                break;


        }
    }

}

void Trampa::romper(){
    Explosion *nueva = NULL;
    switch (id) {
        case 3: //TNT
            cuerpo->setMaskBits(MASK_INTANGIBLE);
            cuerpo->setType(2);
            cuerpo->setVelocidad(0,0);
            m_vBody->setTex(Resources::getInstance()->tntRoto);
            nueva = new Explosion(cuerpo->getPosicionX()* PPM, cuerpo->getPosicionY() * PPM, 2.0f, 0.04f, 0.5f);
            Partida::getInstance()->worldExplo.insert(nueva);
            break;
        case 4: //Nitro
            cuerpo->setMaskBits(MASK_INTANGIBLE);
            cuerpo->setType(2);
            cuerpo->setVelocidad(0,0);
            m_vBody->setTex(Resources::getInstance()->nitroRoto);
            nueva = new Explosion(cuerpo->getPosicionX()* PPM, cuerpo->getPosicionY() * PPM, 2.5f, 0.05f, 0.5f);
            Partida::getInstance()->worldExplo.insert(nueva);
            break;
        default:
            break;
    }
}

int Trampa::getID() {
    return id;
}

void Trampa::update() {
    float angle = cuerpo->getAngulo();
    float posX = cuerpo->getPosicionX();
    float posY = cuerpo->getPosicionY();
    float velY;

    m_vBody->setPos(posX*PPM, posY * PPM);
    m_vBody->setAngle(angle);

    reloj.restartClock();
    difTime += reloj.getDeltaTimeAsSeconds();

    if (estado == false && timeAct != -1 && difTime >= timeAct) {
        activar();
        difTime = 0;
    } else if (estado == true && timeDes != -1 && difTime >= timeDes) {
        desactivar();
        difTime = 0;
    }
    switch (id) {
        case 0:
            if (iniPosY >= posY * PPM + m_SizeY / 2 && cuerpo->getVelocidadY() < 0) {
                velY = 0;
                cuerpo->setVelocidadY(velY);
            } else if (iniPosY <= posY * PPM - m_SizeY / 2 && cuerpo->getVelocidadY() > 0) {
                velY = 0;
                cuerpo->setVelocidadY(velY);
            }
            break;
        default:
            break;
    }


}

bool Trampa::getEstado() {
    return estado;
}



Cuerpo* Trampa::getCuerpo() {
    return (this->cuerpo);
}

VisibleBody* Trampa::getVBody() {
    return (this->m_vBody);
}

void Trampa::Contact(void* punt, string tipo) {
    switch (id) {
        case 3: //TNT
            if (roto == false) { 
                if(tipo.compare("Bala")== 0 || tipo.compare("Explosion") == 0){
                    activar();
                }
            }
            break;
        case 4: //Nitro
            if (roto == false) {
                if(tipo.compare("Bala")== 0 || tipo.compare("Explosion") == 0 || tipo.compare("Player") == 0){
                    activar();
                }
            }
            break;
        default:
            break;
    }
}

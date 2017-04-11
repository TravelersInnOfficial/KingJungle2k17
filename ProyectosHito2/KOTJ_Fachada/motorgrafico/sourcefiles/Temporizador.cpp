/* 
 * File:   Temporizador.cpp
 * Author: daniel
 * 
 * Created on 27 de marzo de 2017, 13:05
 */

#include "../headerfiles/Temporizador.h"
#include "../../headerfiles/Partida.h"

Temporizador::Temporizador() {
}

Temporizador::Temporizador(int time, b2Vec2 position, int size) {

    chrono.getElapsedTime();
    chrono.restart();

    maxTime = (float) time;
    this->position = position;

    if (!font.loadFromFile("resources/fonts/bits.ttf")) {
        cerr << "No encuentro la fuente (Clock)" << endl;
    }

    timeBoardSec = new sf::Text();
    timeBoardSec->setFont(font);
    timeBoardSec->setCharacterSize(size);

    ostringstream stm;
    stm << maxTime;
    timeBoardSec->setString(stm.str());

    timeBoardMsec = new sf::Text();
    timeBoardMsec->setFont(font);
    timeBoardMsec->setCharacterSize(size / 2);
    timeBoardMsec->setString("0");
}

Temporizador::Temporizador(const Temporizador& orig) {
}

Temporizador::~Temporizador() {
}

void Temporizador::Update() {
    currentTime = chrono.getElapsedTime().asSeconds();

    float fSeconds = maxTime - currentTime;
    int iSeconds = maxTime - currentTime;
    int iMiliseconds = (iSeconds - fSeconds) * -100; //100 a 0
    iMiliseconds = iMiliseconds * 60 / 100; //60 a 0


    ostringstream stm;
    stm << iSeconds;
    string sSeconds = stm.str();
    stm.clear();
    stm.str("");
    stm << iMiliseconds;
    string sMiliseconds = stm.str();

    if (iSeconds < 10) sSeconds = "0" + sSeconds;

    if (iMiliseconds < 10) sMiliseconds = ":0" + sMiliseconds;
    else sMiliseconds = ":" + sMiliseconds;

    timeBoardSec->setString(sSeconds);
    timeBoardMsec->setString(sMiliseconds);

    setPosition();

    if (iSeconds <= 0 && iMiliseconds <= 0) restartGame();

}

void Temporizador::Draw(sf::RenderWindow *window) {
    window->draw(*timeBoardSec);
    window->draw(*timeBoardMsec);
}

void Temporizador::restartGame() {
    chrono.restart();
    Partida *partida = Partida::getInstance();
    partida->respawn();
}

void Temporizador::setPosition() {

    timeBoardSec->setPosition(position.x - (timeBoardSec->getGlobalBounds().width / 2), position.y);

    float mposX = timeBoardSec->getGlobalBounds().left + timeBoardSec->getGlobalBounds().width + (timeBoardMsec->getGlobalBounds().width/4);
    float mposY = timeBoardSec->getGlobalBounds().top + timeBoardSec->getGlobalBounds().height - (timeBoardMsec->getGlobalBounds().height * 1.675);

    timeBoardMsec->setPosition(mposX, mposY);

}

void Temporizador::setBasePosition(b2Vec2 *position) {
    this->position = *position;
}

void Temporizador::setScale(float newScaleX, float newScaleY){
    timeBoardSec->setScale(newScaleX, newScaleY);
    timeBoardMsec->setScale(newScaleX, newScaleY);
}
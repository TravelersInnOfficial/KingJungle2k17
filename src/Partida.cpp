
/* 
 * File:   Partida.cpp
 * Author: daniel
 * 
 * Created on 26 de marzo de 2017, 13:13
 */

#include "motorgrafico/Motorgrafico.h"
#include "Partida.h"
#include <algorithm>
#include <PI.h>

using namespace sf;

static Partida* instance;

Partida::Partida() {
    usingKeyboard = false;
    mapa = NULL;
    factoriaArmas = NULL;
    factoriaTrampas = NULL;
    factoriaCoins = NULL;
    lastLevel = false;
    indexMap = -1;
    rondaActual = 0;
    hud = Hud::getInstance();
    loadTextsNClock();
}

Partida* Partida::getInstance() {
    if (instance == NULL) instance = new Partida();
    return (instance);
}

void Partida::Input(int &e) {
    updateIA();
    Motorgrafico::getInstance()->eventListener(e);
}

void Partida::updateIA() {
    if(!gameisover){
        for (int i = 0; i < worldControlador.size(); i++) {
            Controlador* c = worldControlador.at(i);
            if (c->tipo.compare("IA") == 0) {
                c->update();
            }
        }
    }
}

void Partida::erasePlayers() {
    for (int i = 0; i < players2Delete.size(); i++) {
        players2Delete.at(i)->die(players2Delete.at(i)->getDirMoving());
        if (rondas.at(rondaActual).modoJuego == 0) finishRound();
    }
    players2Delete.clear();
}

void Partida::eraseWeapons() {
    for (int i = 0; i < weapons2Delete.size(); i++) {
        delete(weapons2Delete.at(i));
        weapons2Delete.at(i) = NULL;
    }
    weapons2Delete.clear();
}

void Partida::erasePlatforms() {
    for (int i = 0; i < platforms2Delete.size(); i++) {
        delete(platforms2Delete.at(i));
    }
    platforms2Delete.clear();
}

void Partida::eraseTraps() {
    for (int i = 0; i < traps2Delete.size(); i++) {
        delete(traps2Delete.at(i));
    }
    traps2Delete.clear();
}

void Partida::eraseBullets() {
    set<Bala*>::iterator itBala = bullets2Delete.begin();

    for (; itBala != bullets2Delete.end(); itBala++) {
        Bala* dyingBala = *itBala;
        if (dyingBala->explosion == true) {
            float posX = dyingBala->cuerpo->getPosicionX();
            float posY = dyingBala->cuerpo->getPosicionY();

            Explosion *nueva;
            nueva = new Explosion(posX * PPM, posY * PPM, 1.0f, 0.05f, 0.5f);
            worldExplo.insert(nueva);
            Motorgrafico::getInstance()->getMusicPlayer()->playSFX(Motorgrafico::getInstance()->getMusicPlayer()->explosion2);
        }
        worldBullets.erase(dyingBala);
        delete dyingBala;
        dyingBala = NULL;
    }
    bullets2Delete.clear();
}

void Partida::eraseExplo() {
    set<Explosion*>::iterator itExplo = explo2Delete.begin();
    set<Explosion*>::iterator itEnd = explo2Delete.end();

    for (; itExplo != itEnd; itExplo++) {
        Explosion* dyingExplo = *itExplo;
        worldExplo.erase(dyingExplo);
        delete dyingExplo;
        dyingExplo = NULL;
    }
    explo2Delete.clear();
}

void Partida::eraseCoins() {
    for (int i = 0; i < coins2Delete.size(); i++) {
        delete(coins2Delete.at(i));
    }
    coins2Delete.clear();
}

void Partida::Erase() {
    eraseBullets();
    erasePlayers();
    eraseExplo();
    eraseWeapons();
    erasePlatforms();
    eraseTraps();
    eraseCoins();

}

void Partida::Update() {
    Motorfisico::getInstance()->Update();
    Motorgrafico::getInstance()->updateWindow();

    mapa->Update();
    factoriaArmas->Update();
    factoriaCoins->Update();
    updateWeapons();
    updatePlayers(Motorgrafico::getInstance()->getFrameTime());
    updateBullets();
    updateExplo();
    updateClock();
    updateTraps();
    breakTraps();
    updateTexts();
    if (rondas.at(rondaActual).modoJuego == 1) updateCoins(Motorgrafico::getInstance()->getFrameTime());

    cameraSetFinalTransform();
}

void Partida::Render() {
    if (!loadingLevelStruct.showingInbetween) {
        Motorgrafico::getInstance()->clearWindow();

        Motorgrafico::getInstance()->setMainCameraView();
        mapa->drawBackground();
        drawPlayers();
        drawWeapons();
        drawTraps();
        if (rondas.at(rondaActual).modoJuego == 1) drawCoins();
        mapa->drawMap();
        drawBullets();
        drawExplo();
        drawTexts();

        Motorgrafico::getInstance()->setHudCameraView();
        if (notFirstReset || loadingLevelStruct.loadingLevel) drawMainText();
        hud->render();
        console.draw();

        Motorgrafico::getInstance()->drawTemporizador();
        Motorgrafico::getInstance()->displayWindow();
    } else inbetween->render();
}

void Partida::drawPlayers() {
    for (int i = 0; i < worldPlayer.size(); i++) {
        Motorgrafico::getInstance()->draw(worldPlayer.at(i)->getPlayerSprite());
    }
}

void Partida::drawWeapons() {
    for (int i = 0; i < worldWeapons.size(); i++) {
        if (worldWeapons.at(i) != NULL) Motorgrafico::getInstance()->draw((worldWeapons.at(i)->m_vBody->getShape()));
    }
}

void Partida::drawTraps() {
    for (int i = 0; i < worldTraps.size(); i++) {
        if (worldTraps.at(i) != NULL) Motorgrafico::getInstance()->draw((worldTraps.at(i)->getVBody()->getShape()));
    }
}

void Partida::drawBullets() {
    set<Bala*>::iterator itBala = worldBullets.begin();
    set<Bala*>::iterator endBala = worldBullets.end();
    for (; itBala != endBala; ++itBala) {

        Bala* renderBala = *itBala;
        Motorgrafico::getInstance()->draw(renderBala->getBodyShape()->getShape());
    }
}

void Partida::drawExplo() {
    set<Explosion*>::iterator itExplo = worldExplo.begin();
    set<Explosion*>::iterator endExplo = worldExplo.end();
    for (; itExplo != endExplo; ++itExplo) {

        Explosion* renderExplo = *itExplo;
        Motorgrafico::getInstance()->draw(renderExplo->getBodyShape()->getCircleShape());
    }
}

void Partida::drawTexts() {
    for (int i = 0; i < worldTexts.size() - 1; i++)
        if (worldTexts.at(i)->getTexto() != "") {

            Motorgrafico::getInstance()->draw(worldTexts.at(i)->getDrawable());
        }
}

void Partida::drawMainText() {
    Motorgrafico::getInstance()->draw(worldTexts.at(4)->getDrawable());
}

void Partida::drawCoins() {
    for (int i = 0; i < worldCoins.size(); i++) {
        if (worldCoins.at(i) != NULL) Motorgrafico::getInstance()->draw(worldCoins.at(i)->getCoinSprite());
    }
}

int Partida::findKeyboardControlador() {
    int index = -1;

    for (int i = 0; i < worldControlador.size(); i++) {
        if (worldControlador.at(i)->tipo.compare("Keyboard") == 0) {
            index = i;

            break;
        }
    }
    return index;
}

int Partida::findControladorWithId(int id) {
    int index = 0;

    for (int i = 0; i < worldControlador.size(); i++) {
        if (worldControlador.at(i)->tipo.compare("Joystick") == 0 && worldControlador.at(i)->id == id) {
            index = i;

            break;
        }
    }

    return index;
}

void Partida::checkJoysticksConnected() {
    Joystick joystickManager;
    for (int i = 0; i < 4; i++) {
        if (joystickManager.isConnected(i) && worldPlayer.size() < 4) {
            addPlayerJoystick(i);
        }
    }
}

void Partida::addPlayerJoystick(int id) {
    //Para saber si la tenemos que añador
    bool add = true;

    if (worldPlayer.size() < 4 && mapa->getIfFirstMap()) {
        //Comprobamos si existe el mando y actualizamos la condición
        for (int i = 0; i < worldControlador.size() && add; i++) {
            if (worldControlador.at(i)->tipo.compare("Joystick") == 0 && worldControlador.at(i)->id == id) add = false;
        }
        //Añadimos en funcion de la condición
        if (add) {
            PlayerJoystick* p = new PlayerJoystick(id);
            worldControlador.push_back(p);
        }
    }
}

void Partida::addPlayerKeyboard() {
    if (worldPlayer.size() < 4) {
        setUsingKeyboard(true);
        worldControlador.push_back(new PlayerKeyboard());
    }
}

void Partida::addPlayerIA() {
    if (worldPlayer.size() < 4 && mapa->getIfFirstMap()) worldControlador.push_back(new IAController());
}

void Partida::respawn() {
    //ordenamos los jugadores segun la puntuacion
    if (indexMap == 1) {
        ordenarJugadores();
    }
    for (int i = 0; i < worldPlayer.size(); i++) {

        Player* player = worldPlayer.at(i);

        vector<int> position = mapa->spawnPlayer.at(0);
        mapa->spawnPlayer.erase(mapa->spawnPlayer.begin());
        mapa->spawnPlayer.push_back(position);

        player->setPosition(position.at(0), position.at(1));
        player->respawn();

        factoriaArmas->deleteNotPossessedWeapons();
        factoriaArmas->cargarArmas();
    }
}

void Partida::updatePlayers(Time frameTime) {

    //cout << timeBetweenReset << endl;
    
    for (int i = 0; i < worldPlayer.size(); i++) {
        worldPlayer.at(i)->update(Motorgrafico::getInstance()->getFrameTime());
        if (rondas.at(rondaActual).modoJuego == 1) {
            int value = worldPlayer.at(i)->collidesWithCoin();
            Texto *plusCoin = worldTexts.at(i);
            worldPlayer.at(i)->getCoinsClock()->restartClock();
        worldPlayer.at(i)->addCoinDeltaTime();


            string valorMoneda;
            if (value != -1) {
                ostringstream stm;
                stm << "+" << value;
                string valorMoneda = stm.str();
                plusCoin->setTexto(valorMoneda);
                plusCoin->setColor(0, 200, 0);
                worldPlayer.at(i)->resetCoinsTime();
            }
            if (worldPlayer.at(i)->isPlayerDead()) {
                int valor = worldPlayer.at(i)->getDecreasedCoins();
                //cout << "DecreasedCoins --> " << valor << endl;
                //cout << "Monedero --> " << worldPlayer.at(i)->getMonedero() << endl;
                ostringstream stm;
                stm << "-" << valor;
                string monedasEliminadas = stm.str();
                plusCoin->setTexto(monedasEliminadas);
                plusCoin->setColor(200, 0, 0);
                worldPlayer.at(i)->resetCoinsTime();
                worldPlayer.at(i)->addRespawnDeltaTime();
                if (worldPlayer.at(i)->getRespawnTime() > 3.0) {
                    int pos = rand() % mapa->spawnPlayer.size();
                    vector<int> position = mapa->spawnPlayer.at(pos);

                    //worldPlayer.at(i)-> setPosition(position.at(0), position.at(1));
                    if (timeBetweenReset < 0.5) {
                        worldPlayer.at(i)-> setPosition(position.at(0), position.at(1));
                        worldPlayer.at(i)-> respawn();
                    }
                    worldPlayer.at(i)->resetRespawnTime();
                }
            } else {
                worldPlayer.at(i)->getRespawnClock()->restartClock();
            }
            if (worldPlayer.at(i)->getCoinsTime() > 1.0) {
                valorMoneda = "";
                plusCoin->setTexto(valorMoneda);
            }
        }
    }
}

void Partida::cameraSetFinalTransform() {
    Motorgrafico::getInstance()->cameraSetFinalTransform();
}

void Partida::updateWeapons() {
    for (int i = 0; i < worldWeapons.size(); i++) {
        if (worldWeapons.at(i) != NULL) worldWeapons.at(i)->update();
    }
}

void Partida::updateCoins(Time frameTime) {
    for (int i = 0; i < worldCoins.size(); i++) {
        if (worldCoins.at(i) != NULL) {
            worldCoins.at(i)->update(frameTime);
            if (worldCoins.at(i)->getInPossession()) {
                coins2Delete.push_back(worldCoins.at(i));
                worldCoins.at(i) = NULL;
            }
        }
    }
}

void Partida::updateTraps() {
    for (int i = 0; i < worldTraps.size(); i++) {
        if (worldTraps.at(i) != NULL) worldTraps.at(i)->update();
    }
}

void Partida::breakTraps() { //Rompe las trampas, se le llama en el update a falta de un lugar mas adecuado
    for (int i = 0; i < traps2Break.size(); i++) {
        if (traps2Break.at(i) != NULL) traps2Break.at(i)->romper();
    }
    traps2Break.clear();
}

void Partida::ordenarJugadores() {
    int j;
    Player* jugador;
    for (int i = 1; i < worldPlayer.size(); i++) {
        jugador = worldPlayer.at(i);
        j = i - 1;
        while ((j >= 0) && (jugador->getPoints() > worldPlayer.at(j)->getPoints())) {
            worldPlayer.at(j + 1) = worldPlayer.at(j);
            j--;
        }
        worldPlayer.at(j + 1) = jugador;
    }
}

void Partida::updateBullets() {
    set<Bala*>::iterator itBala = worldBullets.begin();
    set<Bala*>::iterator endBala = worldBullets.end();
    for (; itBala != endBala; ++itBala) {
        Bala* updateBala = *itBala;
        updateBala->Update_Shape();
    }

}

void Partida::updateExplo() {
    set<Explosion*>::iterator itExplo = worldExplo.begin();
    set<Explosion*>::iterator endExplo = worldExplo.end();
    for (; itExplo != endExplo; ++itExplo) {
        Explosion* updateBala = *itExplo;
        updateBala->Update();
    }
}

void Partida::setMaxPoints(int points) {
    maxPoints = points;
}

void Partida::updateClock() {
    //si no es la primera vez que se hace el reset del clock, se empieza a reiniciar el clock cada frame y se calcula el tiempo desde que empieza
    if (notFirstReset) {
        changeLevelClock.restartClock();
        timeBetweenReset += changeLevelClock.getDeltaTimeAsSeconds();
        //AQUI entra cuando es el mapa de seleccion y asi saltarse tanto la espera como dar puntos
        if (indexMap == -1 && finalLevelTextPrepared) {
            timeBetweenReset = 3;
            finalLevelTextPrepared = false;
        }
        if (finalLevelTextPrepared && timeBetweenReset > 1.0 && rondas.at(rondaActual).modoJuego == 0) {
            //se comprueba si hay solo 1 jugador vivo y se le da un punto
            int playerposition = -1;


            for (int i = 0; i < worldPlayer.size(); i++) {
                if (worldPlayer.at(i) != NULL && !worldPlayer.at(i)->isPlayerDead()) {
                    playerposition = i;
                }
            }
            if (playerposition != -1) {
                //aqui entre cuando solo hay una persona viva una vez ya pasado 1 segundol
                worldPlayer.at(playerposition)->give1Point();

                if (worldPlayer.at(playerposition)->getPoints() == maxPoints) {
                    gameisover = true;
                }

                finalLevelTextPrepared = false;
                Texto *plus1 = worldTexts.at(4);
                plus1->setTexto("+1");
                plus1->setPos(screenWidth / 2 + 30, screenHeight / 2 - 90);
                Motorgrafico::getInstance()->getMusicPlayer()->playSFX(Motorgrafico::getInstance()->getMusicPlayer()->coin2);
            }//aqui entra cuando la persona que ha matado al ultimo se suicida y no queda nadie vivo
            else {
                finalLevelTextPrepared = false;
                Texto *plus1 = worldTexts.at(4);
                plus1->setTexto("+0");
                plus1->setPos(screenWidth / 2 - 50, screenHeight / 2.5);
                Motorgrafico::getInstance()->getMusicPlayer()->playSFX(Motorgrafico::getInstance()->getMusicPlayer()->gameOver1);
            }
        }


        //cout << "Final Modo Monedas -- " << finalLevelTextPrepared << " -- " << timeBetweenReset << " -- " << rondas.at(rondaActual).modoJuego << endl;

        //para el modo monedas
        if (finalLevelTextPrepared && timeBetweenReset > 1.0 && rondas.at(rondaActual).modoJuego == 1) {
            int playerposition = -1;
            int maxMonedero = -1;
            for (int i = 0; i < worldPlayer.size(); i++) {
                if (worldPlayer.at(i) != NULL) {
                    if (worldPlayer.at(i)->getMonedero() > maxMonedero) {
                        playerposition = i;
                        maxMonedero = worldPlayer.at(i)->getMonedero();
                    } else if (worldPlayer.at(i)->getMonedero() == maxMonedero) {
                        maxMonedero = -1;
                    }
                }
            }
            if (playerposition != -1 && maxMonedero != -1) {
                worldPlayer.at(playerposition)->give1Point();

                if (worldPlayer.at(playerposition)->getPoints() == maxPoints) {
                    gameisover = true;
                }

                finalLevelTextPrepared = false;
                Texto *plus1 = worldTexts.at(4);
                plus1->setTexto("+1");
                plus1->setPos(screenWidth / 2 + 30, screenHeight / 2 - 90);
                Motorgrafico::getInstance()->getMusicPlayer()->playSFX(Motorgrafico::getInstance()->getMusicPlayer()->coin2);
            }//aqui entra cuando la persona que ha matado al ultimo se suicida y no queda nadie vivo
            else {
                finalLevelTextPrepared = false;
                Texto *plus1 = worldTexts.at(4);
                plus1->setTexto("+0");
                plus1->setPos(screenWidth / 2 - 50, screenHeight / 2.5);
                Motorgrafico::getInstance()->getMusicPlayer()->playSFX(Motorgrafico::getInstance()->getMusicPlayer()->gameOver1);
            }

        }
    }

    //cuando han pasado 4 segundos, se cambia de nivel
    if (timeBetweenReset > 4.0) {
        notFirstReset = false;
        timeBetweenReset = 0;
        worldTexts.at(4)->setTexto("");
        //cout << "se termina el juego?" << gameisover << endl;
        if (gameisover) {
            startGameOverInbetweener();
            loadFinalMap();
        } else
            loadMap();
    }
}

void Partida::updateTexts() {
    for (int i = 0; i < worldPlayer.size(); i++) {
        if (worldPlayer.at(i) != NULL && !worldPlayer.at(i)->isPlayerDead()) {
            worldTexts.at(i)->setPos(worldPlayer.at(i)->getPositionX() * PPM + 35, worldPlayer.at(i)->getPositionY() * PPM - 70);
        } else {

            worldTexts.at(i)->setTexto("");
        }
    }
}

void Partida::inbetweenUpdate() {
    changeLevelClock.restartClock();
    timeBetweenReset += changeLevelClock.getDeltaTimeAsSeconds();

    if (timeBetweenReset > 2) {
        loadingLevelStruct.loadingLevel = true;
        loadingLevelStruct.showingInbetween = false;
        timeBetweenReset = 0;
        Update();
        Update();
    }
}

void Partida::updateBeforeMap() {
    changeLevelClock.restartClock();
    timeBetweenReset += changeLevelClock.getDeltaTimeAsSeconds();

    if (gameisover) {
        Motorgrafico::getInstance()->getTemporizador()->stop(false);
        Motorgrafico::getInstance()->getTemporizador()->restart();
        loadingLevelStruct.loadingLevel = false;
    }

    if (loadingLevelStruct.firstTextPrepared && timeBetweenReset > 1) {
        Texto *plus1 = worldTexts.at(4);
        plus1->setTexto("GET");
        plus1->setPos(screenWidth / 2 - 40, screenHeight / 2 - 80);
        Motorgrafico::getInstance()->getMusicPlayer()->playSFX(Motorgrafico::getInstance()->getMusicPlayer()->coin1);
        loadingLevelStruct.firstTextPrepared = false;
    }//ready
    else if (loadingLevelStruct.secondTextPrepared && timeBetweenReset > 2) {
        Texto *plus1 = worldTexts.at(4);
        plus1->setTexto("READY");
        plus1->setPos(screenWidth / 2 - 80, screenHeight / 2 - 80);
        Motorgrafico::getInstance()->getMusicPlayer()->playSFX(Motorgrafico::getInstance()->getMusicPlayer()->coin2);
        loadingLevelStruct.secondTextPrepared = false;
    }//se reanuda la partida
    else if (loadingLevelStruct.thirdTextPrepared && timeBetweenReset > 3) {
        Motorgrafico::getInstance()->getTemporizador()->stop(false);
        Motorgrafico::getInstance()->getTemporizador()->restart();
        Texto *plus1 = worldTexts.at(4);
        plus1->setTexto("");
        plus1->setPos(screenWidth / 2 - 30, screenHeight / 2 - 90);
        Motorgrafico::getInstance()->getMusicPlayer()->playSFX(Motorgrafico::getInstance()->getMusicPlayer()->explosion1);
        loadingLevelStruct.thirdTextPrepared = false;
        loadingLevelStruct.loadingLevel = false;
        timeBetweenReset = 0;

    }
}

bool Partida::getUsingKeyboard() {
    return usingKeyboard;
}

void Partida::setUsingKeyboard(bool state) {
    usingKeyboard = state;
}

void Partida::startTextBeforeLevel() {
    loadingLevelStruct.loadingLevel = false;
    loadingLevelStruct.firstTextPrepared = true;
    loadingLevelStruct.secondTextPrepared = true;
    loadingLevelStruct.thirdTextPrepared = true;
    loadingLevelStruct.showingInbetween = true;

    timeBetweenReset = 0;
    Erase();
    inbetween = NULL;
    inbetween = new Inbetween(worldPlayer, maxPoints, rondas.at(rondaActual).modoJuego, false);

    Motorgrafico::getInstance()->getTemporizador()->stop(true);
}

void Partida::startGameOverInbetweener() {
    loadingLevelStruct.loadingLevel = false;
    loadingLevelStruct.firstTextPrepared = false;
    loadingLevelStruct.secondTextPrepared = false;
    loadingLevelStruct.thirdTextPrepared = false;
    loadingLevelStruct.showingInbetween = true;

    timeBetweenReset = 0;
    inbetween = NULL;
    inbetween = new Inbetween(worldPlayer, maxPoints, rondas.at(rondaActual).modoJuego, true);

    Motorgrafico::getInstance()->getTemporizador()->stop(true);
}

void Partida::fillRondasVector(int tipoRonda) {
    srand(time(NULL));
    rondas.clear();
    Mapa *mapa = new Mapa();
    int newIndexMap = -1;
    int newIndexModo = -1;
    int oldIndexMap = -1;
    int oldIndexModo = -1;

    for (int i = 0; i < 30; i++) {
        Ronda ronda;

        do {
            newIndexMap = rand() % mapa->mapas.size();
        } while (newIndexMap == oldIndexMap || newIndexMap == 0 || newIndexMap == 1);
        ronda.mapa = newIndexMap;

        if (tipoRonda != 1) {
            newIndexModo = tipoRonda - 2;
        } else {
            do {
                newIndexModo = rand() % 2;
            } while (newIndexModo == oldIndexModo);
        }

        ronda.modoJuego = newIndexModo;

        rondas.push_back(ronda);

        oldIndexMap = newIndexMap;
        oldIndexModo = newIndexModo;
        //cout<<rondas.at(i).modoJuego<<endl;
    }
}

void Partida::loadMap() {

    if (lastLevel) Motorgrafico::getInstance()->setGoToMenu(true);
    else {

        if (mapa != NULL) {
            delete(mapa);
            mapa = NULL;
        }

        if (factoriaArmas != NULL) {
            delete(factoriaArmas);
            factoriaArmas = NULL;
        }

        if (factoriaTrampas != NULL) {
            factoriaTrampas->borrarTrampas();
            delete(factoriaTrampas);
            factoriaTrampas = NULL;
        }

        if (factoriaCoins != NULL) {
            delete(factoriaCoins);
            factoriaCoins = NULL;
        }

        mapa = new Mapa();
        if (rondaActual > rondas.size() - 1) rondaActual = 0;
        mapa->leerMapa(mapa->mapas.at(rondas.at(rondaActual).mapa));
        indexMap = rondas.at(rondaActual).mapa;
        if (mapa->getIfFirstMap()) indexMap = -1;
        rondaActual++;

        factoriaArmas = new Weaponspawner();
        factoriaTrampas = new TrapSpawner();
        factoriaCoins = new Coinsspawner();

        Motorgrafico::getInstance()->getTemporizador()->restart();
        Motorgrafico::getInstance()->getTemporizador()->stop(false);

        for (int i = 0; i < worldPlayer.size(); i++) {
            worldPlayer.at(i)->resetMonedero();
            Texto *plusCoin = worldTexts.at(i);
            plusCoin->setTexto("");
        }
        respawn();

        if (!mapa->getIfFirstMap())
            startTextBeforeLevel();
    }
}

void Partida::loadMap(string mapaStr) {

    if (mapa != NULL) {
        delete(mapa);
        mapa = NULL;
    }

    if (factoriaArmas != NULL) {
        delete(factoriaArmas);
        factoriaArmas = NULL;
    }

    if (factoriaTrampas != NULL) {
        factoriaTrampas->borrarTrampas();
        delete(factoriaTrampas);
        factoriaTrampas = NULL;
    }
    if (factoriaCoins != NULL) {
        delete(factoriaCoins);
        factoriaCoins = NULL;
    }


    mapa = new Mapa();
    mapa->leerMapa(mapaStr);
    indexMap = rondas.at(rondaActual).mapa;
    if (mapa->getIfFirstMap()) {
        indexMap = -1;
        if (rondas.at(rondaActual).modoJuego != 0) rondas.at(rondaActual).modoJuego = 0;
    }

    factoriaArmas = new Weaponspawner();
    factoriaTrampas = new TrapSpawner();
    factoriaCoins = new Coinsspawner();

    Motorgrafico::getInstance()->getTemporizador()->restart();
    Motorgrafico::getInstance()->getTemporizador()->stop(false);

    for (int i = 0; i < worldPlayer.size(); i++) {
        worldPlayer.at(i)->resetMonedero();
        Texto *plusCoin = worldTexts.at(i);
        plusCoin->setTexto("");
    }
    respawn();

    if (!mapa->getIfFirstMap())
        startTextBeforeLevel();
}

void Partida::loadFinalMap() {
    indexMap = 1;
    lastLevel = true;
    rondas.at(rondaActual).modoJuego = 0;

    if (mapa != NULL) {
        delete(mapa);
        mapa = NULL;
    }

    if (factoriaArmas != NULL) {

        delete(factoriaArmas);
        factoriaArmas = NULL;
    }

    if (factoriaTrampas != NULL) {
        factoriaTrampas->borrarTrampas();
        delete(factoriaTrampas);
        factoriaTrampas = NULL;
    }

    mapa = new Mapa();
    mapa->leerMapa(mapa->mapaPodio);

    factoriaArmas = new Weaponspawner();
    Motorgrafico::getInstance()->getTemporizador()->restart();
    Motorgrafico::getInstance()->getTemporizador()->stop(false);

    for (int i = 0; i < worldPlayer.size(); i++) {
        worldPlayer.at(i)->resetMonedero();
        Texto *plusCoin = worldTexts.at(i);
        plusCoin->setTexto("");
    }
    respawn();

    VisibleBody *podioVB = new VisibleBody(352 + 128, 352 + 64, 256, 192, "./resources/sprites/podio.png", true);
    mapa->aditionalSprites.push_back(podioVB);
}

Partida::~Partida() {

}

void Partida::finishRound() {
    if (lastLevel) Motorgrafico::getInstance()->setGoToMenu(true);
    else {
        if (!notFirstReset) {

            int alivePeople = 0;
            for (int i = 0; i < worldPlayer.size(); i++) {
                if (worldPlayer.at(i) != NULL && !worldPlayer.at(i)->isPlayerDead())
                    alivePeople++;
            }
            if (alivePeople == 1) {
                Motorgrafico::getInstance()->getTemporizador()->stop(true);
                changeLevelClock.restartClock();
                notFirstReset = true;
                timeBetweenReset = 0;
                finalLevelTextPrepared = true;
            }
        }
    }
}

void Partida::loadTextsNClock() {
    changeLevelClock = InnerClock();
    changeLevelClock.restartClock();
    notFirstReset = false;
    gameisover = false;

    loadingLevelStruct.loadingLevel = false;
    loadingLevelStruct.showingInbetween = false;
    loadingLevelStruct.firstTextPrepared = false;
    loadingLevelStruct.secondTextPrepared = false;
    loadingLevelStruct.thirdTextPrepared = false;

    Texto *text;
    text = new Texto("", 60, Resources::getInstance()->myFont, 255, 255, 255);
    worldTexts.push_back(text);

    text = new Texto("", 60, Resources::getInstance()->myFont, 255, 255, 255);
    worldTexts.push_back(text);

    text = new Texto("", 60, Resources::getInstance()->myFont, 255, 255, 255);
    worldTexts.push_back(text);

    text = new Texto("", 60, Resources::getInstance()->myFont, 255, 255, 255);
    worldTexts.push_back(text);

    text = new Texto("", 80, Resources::getInstance()->myFont, 255, 255, 255);
    worldTexts.push_back(text);
}
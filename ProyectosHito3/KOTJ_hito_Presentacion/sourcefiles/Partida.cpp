
/* 
 * File:   Partida.cpp
 * Author: daniel
 * 
 * Created on 26 de marzo de 2017, 13:13
 */

#include "../motorgrafico/headerfiles/Motorgrafico.h"
#include "../headerfiles/Partida.h"

using namespace sf;

static Partida* instance;

Partida::Partida() {
    usingKeyboard = false;
    mapa = NULL;
    factoriaArmas = NULL;
    factoriaTrampas = NULL;
    indexMap = -1;
    checkJoysticksConnected();
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
    for (int i = 0; i < worldControlador.size(); i++) {
        Controlador* c = worldControlador.at(i);
        if (c->tipo.compare("IA") == 0) {
            c->update();
        }
    }
}

void Partida::erasePlayers() {
    for (int i = 0; i < players2Delete.size(); i++) {
        players2Delete.at(i)->die(players2Delete.at(i)->getDirMoving());
        finishRound();
    }
    players2Delete.clear();
}

void Partida::eraseWeapons() {
    for (int i = 0; i < weapons2Delete.size(); i++) {
        delete(weapons2Delete.at(i));
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

void Partida::Erase() {
    eraseBullets();
    erasePlayers();
    eraseExplo();
    eraseWeapons();
    erasePlatforms();
    eraseTraps();

}

void Partida::Update() {

    Motorfisico::getInstance()->Update();
    Motorgrafico::getInstance()->updateWindow();

    mapa->Update();
    factoriaArmas->Update();
    updateWeapons();
    updatePlayers(Motorgrafico::getInstance()->getFrameTime());
    updateBullets();
    updateExplo();
    updateClock();
    updateTraps();
    //updateTexts();

    cameraSetTransform();
}

void Partida::Render() {
    Motorgrafico::getInstance()->clearWindow();
    Motorgrafico::getInstance()->setMainCameraView();

    mapa->drawBackground();
    drawBullets();
    drawPlayers();
    //drawTexts(1);
    drawWeapons();
    drawExplo();
    drawTraps();
    mapa->drawMap();

    Motorgrafico::getInstance()->setHudCameraView();
    drawTexts(0);
    console.draw();

    Motorgrafico::getInstance()->drawTemporizador();
    Motorgrafico::getInstance()->displayWindow();
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

void Partida::drawTexts(int n) {
    if (n == 0 && notFirstReset)
        Motorgrafico::getInstance()->draw(worldTexts.at(4)->getDrawable());
    else if (n == 1) {
        for (int i = 0; i < worldTexts.size() - 1; i++)
            if (worldTexts.at(i)->getTexto() != "") {
                Motorgrafico::getInstance()->draw(worldTexts.at(i)->getDrawable());
                //cout << "se pinta el texto nº " << i << "y tiene dentro " << worldTexts.at(i)->getTexto() << endl;
            }
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

    if (worldPlayer.size() < 4) {
        //Comprobamos si existe el mando y actualizamos la condición
        for (int i = 0; i < worldControlador.size() && add; i++) {
            if (worldControlador.at(i)->tipo.compare("Joystick") == 0 && worldControlador.at(i)->id == id) add = false;
        }

        //Añadimos en funcion de la condición
        if (add) {
            PlayerJoystick* p = new PlayerJoystick(id);
            worldControlador.push_back(p);
        }
    } else {
        //cout << "hay demasiados jugadores" << endl;
    }
}

void Partida::addPlayerKeyboard() {
    if (worldPlayer.size() < 4) {
        setUsingKeyboard(true);
        worldControlador.push_back(new PlayerKeyboard());
    }
}

void Partida::addPlayerIA() {
    if (worldPlayer.size() < 4 && !mapa->getIfFirstMap()) worldControlador.push_back(new IAController());
}

void Partida::respawn() {
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
    for (int i = 0; i < worldPlayer.size(); i++) {
        worldPlayer.at(i)->update(Motorgrafico::getInstance()->getFrameTime());
    }
}

void Partida::updateWeapons() {
    for (int i = 0; i < worldWeapons.size(); i++) {
        if (worldWeapons.at(i) != NULL) worldWeapons.at(i)->update();
    }
}

void Partida::updateTraps() {
    for (int i = 0; i < worldTraps.size(); i++) {
        if (worldTraps.at(i) != NULL) worldTraps.at(i)->update();

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

void Partida::updateClock() {
    //si no es la primera vez que se hace el reset del clock, se empieza a reiniciar el clock cada frame y se calcula el tiempo desde que empieza
    if (notFirstReset) {
        bool gameisover = false;

        changeLevelClock.restartClock();
        timeBetweenReset += changeLevelClock.getDeltaTimeAsSeconds();
        if (finalLevelTextPrepared && timeBetweenReset > 1.0) {
            //se comprueba si hay solo 1 jugador vivo y se le da un punto
            int playerposition = -1;


            for (int i = 0; i < worldPlayer.size(); i++) {
                if (worldPlayer.at(i) != NULL && !worldPlayer.at(i)->isPlayerDead()) {
                    playerposition = i;
                }
            }
            if (playerposition != -1) {
                //aqui entre cuando solo hay una persona viva una vez ya pasado 1 segundo
                worldPlayer.at(playerposition)->give1Point();
                if (worldPlayer.at(playerposition)->getPoints() == 10) {
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
        //cuando han pasado 4 segundos, se cambia de nivel
        if (timeBetweenReset > 4.0) {
            notFirstReset = false;
            timeBetweenReset = 0;
            worldTexts.at(4)->setTexto("");
            if (gameisover)
                loadFinalMap();
            else
                loadMap();
        }
    }
}

void Partida::updateTexts() {
    for (int i = 0; i < worldPlayer.size(); i++) {
        if (worldPlayer.at(i) != NULL && !worldPlayer.at(i)->isPlayerDead()) {
            worldTexts.at(i)->setTexto("1");
            worldTexts.at(i)->setPos(worldPlayer.at(i)->getPositionX(), worldPlayer.at(i)->getPositionY());
        } else {
            worldTexts.at(i)->setTexto("");
        }
    }
}

void Partida::cameraSetTransform() {
    Motorgrafico::getInstance()->cameraSetTransform();
}

bool Partida::getUsingKeyboard() {
    return usingKeyboard;
}

void Partida::setUsingKeyboard(bool state) {
    usingKeyboard = state;
}

void Partida::loadMap() {

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
    mapa->leerMapa(mapa->getRandomMap());

    factoriaArmas = new Weaponspawner();
    factoriaTrampas = new TrapSpawner();
    Motorgrafico::getInstance()->getTemporizador()->restart();
    Motorgrafico::getInstance()->getTemporizador()->stop(false);

    respawn();
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

    mapa = new Mapa();
    mapa->leerMapa(mapaStr);

    factoriaArmas = new Weaponspawner();
    factoriaTrampas = new TrapSpawner();
    Motorgrafico::getInstance()->getTemporizador()->restart();
    Motorgrafico::getInstance()->getTemporizador()->stop(false);

    respawn();
}

void Partida::loadFinalMap() {

    if (mapa != NULL) {
        delete(mapa);
        mapa = NULL;
    }

    if (factoriaArmas != NULL) {
        delete(factoriaArmas);
        factoriaArmas = NULL;
    }



    mapa = new Mapa();
    //mapa->leerMapa(mapa->mapaPodio);
    mapa->leerMapa(mapa->mapaCueva);

    VisibleBody *podioVB = new VisibleBody(192, 160, 16, 16, "./resources/sprites/podio.png", true);
    mapa->aditionalSprites.push_back(podioVB);

    factoriaArmas = new Weaponspawner();
    Motorgrafico::getInstance()->getTemporizador()->restart();
    Motorgrafico::getInstance()->getTemporizador()->stop(false);

    respawn();
}

Partida::~Partida() {

}

void Partida::finishRound() {
    if (!notFirstReset) {
        //cout << "trying to finish this" << endl;

        int alivePeople = 0;
        for (int i = 0; i < worldPlayer.size(); i++) {
            if (worldPlayer.at(i) != NULL && !worldPlayer.at(i)->isPlayerDead())
                alivePeople++;
        }

        if (alivePeople == 1) {
            changeLevelClock.restartClock();
            notFirstReset = true;
            timeBetweenReset = 0;
            finalLevelTextPrepared = true;
        }
    }
}

void Partida::loadTextsNClock() {
    changeLevelClock = InnerClock();
    changeLevelClock.restartClock();
    notFirstReset = false;
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
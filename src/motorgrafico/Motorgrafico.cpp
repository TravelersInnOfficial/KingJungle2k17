
/* 
 * File:   Motorgrafico.cpp
 * Author: amygwyllion
 * 
 * Created on 11 de abril de 2017, 20:17
 */

#include "Motorgrafico.h"

static Motorgrafico* instance;

Motorgrafico* Motorgrafico::getInstance() {
    if (instance == NULL) instance = new Motorgrafico();
    return (instance);
}

Motorgrafico::Motorgrafico() {
    window = new sf::RenderWindow(sf::VideoMode(screenWidth, screenHeight), "KingOfTheJungle 2k17 Turbo Power Edition", sf::Style::Titlebar | sf::Style::Close);
    setFramerateLimitOn(60);
    mainCamera = new sf::View(sf::FloatRect(0, 0, screenWidth, screenHeight));
    hudCamera = new sf::View(sf::FloatRect(0, 0, screenWidth, screenHeight));
    cameraVelocity = 0.1f;
    setMainCameraView();
    setHudCameraView();
    goToMenuNow = false;
    contControles = 0;
}

void Motorgrafico::setFramerateLimitOn(int framerate) {
    window->setFramerateLimit(60);
}

void Motorgrafico::setMainCameraView() {
    window->setView(*mainCamera);
}

void Motorgrafico::setHudCameraView() {
    window ->setView(*hudCamera);
}

void Motorgrafico::cameraSetFinalTransform() {

    bool firstMap = Partida::getInstance()->mapa->getIfFirstMap();

    if (firstMap) {
        float posX = 0;
        float posY = 0;
        float total = 0;

        vector< vector<int> > esquinas = partida->mapa->getEsquinas();
        for (int i = 0; i < esquinas.size(); i++) {
            vector<int> esquina = esquinas.at(i);
            posX += esquina.at(0) * MPP;
            posY += esquina.at(1) * MPP;
            total++;
        }

        posX = posX / total;
        posY = posX / total;

        finalCameraPosition = Vector2f(posX * PPM, posY * 2.1 * PPM);
        finalCameraSize = Vector2f(screenWidth * 1.7, screenHeight * 1.7);

        lerpCameraTransform();
        setMainCameraView();

    } else {

        int total = 0;
        float posX = 0;
        float posY = 0;

        float newSize = screenWidth;
        float minSize = screenWidth;

        float ratio = screenWidth / (float) screenHeight;
        float maxDifferenceX = 0;
        float maxDifferenceY = 0;

        //Posición
        for (int i = 0; i < partida->worldPlayer.size(); i++) {
            if (!partida->worldPlayer.at(i)->isPlayerDead()) {
                posX += partida->worldPlayer.at(i)->getPositionX();
                posY += partida->worldPlayer.at(i)->getPositionY();
                total++;
            }
        }

        if (total != 0) {
            posX = posX / total;
            posY = posY / total;
            finalCameraPosition = Vector2f(posX * PPM, posY * PPM);
        }

        //Zoom
        for (int i = 0; i < partida->worldPlayer.size(); i++) {
            if (!partida->worldPlayer.at(i)->isPlayerDead()) {
                float currentPosX = partida->worldPlayer.at(i)->getPositionX();
                if (abs(currentPosX - posX) > maxDifferenceX) maxDifferenceX = abs(currentPosX - posX);

                float currentPosY = partida->worldPlayer.at(i)->getPositionY();
                if (abs(currentPosY - posY) > maxDifferenceY) maxDifferenceY = abs(currentPosY - posY);
            }
        }

        if (maxDifferenceX > maxDifferenceY * ratio) {
            maxDifferenceX += 3.5;
            newSize = abs(maxDifferenceX * 2 * PPM);
            if (newSize < minSize) newSize = minSize;
            finalCameraSize = Vector2f(newSize, newSize / ratio);

        } else {
            maxDifferenceY += (3.5 / ratio);
            newSize = abs(maxDifferenceY * 2 * PPM);
            if (newSize < minSize / ratio) newSize = minSize / ratio;
            finalCameraSize = Vector2f(newSize * ratio, newSize);
        }

        //Poner la cámara
        lerpCameraTransform();
        setMainCameraView();
    }
}

void Motorgrafico::lerpCameraTransform() {
    Vector2f cameraPos = mainCamera->getCenter();
    Vector2f deltaPos = Vector2f(finalCameraPosition.x - cameraPos.x, finalCameraPosition.y - cameraPos.y);
    Vector2f newPos = Vector2f(cameraPos.x + deltaPos.x * cameraVelocity, cameraPos.y + deltaPos.y * cameraVelocity);
    mainCamera->setCenter(newPos.x, newPos.y);

    Vector2f cameraSize = mainCamera->getSize();
    Vector2f deltaSize = Vector2f(finalCameraSize.x - cameraSize.x, finalCameraSize.y - cameraSize.y);
    Vector2f newSize = Vector2f(cameraSize.x + deltaSize.x * cameraVelocity, cameraSize.y + deltaSize.y * cameraVelocity);
    mainCamera->setSize(newSize.x, newSize.y);
}

void Motorgrafico::eventListener(int &e) {

    if (goToMenuNow) {
        goToMenuNow = false;
        e = 1;
    } else {

        switch (e) {
            case 2:
                while (window->pollEvent(event)) {
                    //cout << event.key.code << endl;
                    switch (event.type) {
                        case sf::Event::Closed:
                            window->close();
                            break;

                        case sf::Event::JoystickConnected:
                            if (partida->getInstance()->indexMap == -1)
                                partida->addPlayerJoystick(event.joystickConnect.joystickId);
                            break;

                        case sf::Event::JoystickMoved:
                            if (event.joystickMove.axis == Joystick::X) {
                                partida->worldControlador.at(partida->findControladorWithId(event.joystickMove.joystickId))->checkAxisX(event.joystickMove.position);
                            }
                            if (event.joystickMove.axis == Joystick::Y) {
                                partida->worldControlador.at(partida->findControladorWithId(event.joystickMove.joystickId))->checkAxisY(event.joystickMove.position);

                            }
                            break;

                        case sf::Event::JoystickButtonPressed:
                            partida->worldControlador.at(partida->findControladorWithId(event.joystickButton.joystickId))->pressUpdateState(event.joystickButton.button);
                            break;

                        case sf::Event::JoystickButtonReleased:
                            partida->worldControlador.at(partida->findControladorWithId(event.joystickButton.joystickId))->releaseUpdateState(event.joystickButton.button);
                            for (int i = 0; i < partida->worldPlayer.size(); i++) {
                                if (partida->worldPlayer.at(i)->getPaused()) {
                                    e = 5;
                                }
                            }
                            break;

                        case sf::Event::KeyPressed:
                            if (partida->getUsingKeyboard() == true) {
                                partida->worldControlador.at(partida->findKeyboardControlador())->pressUpdateState(event.key.code);
                            }

                            switch (event.key.code) {
                                case sf::Keyboard::F2:
                                    if (partida->getUsingKeyboard() == false && partida->getInstance()->indexMap == -1)
                                        partida->addPlayerKeyboard();
                                    break;

                                case Keyboard::F3:
                                    if (partida->getInstance()->indexMap == -1)
                                        partida->addPlayerIA();
                                    break;

                                case Keyboard::Tab:
                                    partida->console.toggleConsole();
                                    break;

                                case Keyboard::Return:
                                    partida->console.send();
                                    break;

                                case Keyboard::BackSpace:
                                    partida->console.deleteChar();
                                    break;

                                case Keyboard::Up:
                                    partida->console.getLastCommand();
                                    break;
                            }
                            break;

                        case sf::Event::KeyReleased:
                            if (event.key.code == sf::Keyboard::Escape) {
                                e = 1;
                                getMusicPlayer()->playSound(getMusicPlayer()->menuMusic);
                            }
                            if (partida->getUsingKeyboard() == true && event.key.code == 16) {
                                e = 5;
                            }

                            if (partida->getUsingKeyboard() == true) partida->worldControlador.at(partida->findKeyboardControlador())->releaseUpdateState(event.key.code);
                            break;

                        case sf::Event::TextEntered:
                            textEnteredConsole(event);
                            break;
                    }
                }
                break;
                
            case 6:
                while (window->pollEvent(event)) {
                    switch (event.type) {
                        case Event::KeyReleased:
                            
                            if (contControles == 0) {renderizarInstrucciones2();}
                            if (contControles == 1) {contControles = -1; e = 1;}
                            contControles++;
                            break;
                    }
                }
                break;
        }
    }
}

void Motorgrafico::eventListener(int &e, Menu* menu) {


    if (goToMenuNow) {
        goToMenuNow = false;
        e = 1;
    } else {
        switch (e) {
            case 0:
                window->close();
                break;
            case 1:
                while (window->pollEvent(event)) {
                    switch (event.type) {
                        case Event::Closed:
                            e = 0;
                            window->close();
                            break;

                        case Event::JoystickMoved:
                            if (event.joystickMove.axis == Joystick::Y) {
                                if (event.joystickMove.position > 10) {
                                    menu->goDown();
                                }

                                if (event.joystickMove.position < -10) {
                                    menu->goUp();
                                }
                            }
                            break;
                        case Event::JoystickButtonPressed:
                            if (menu->getCurrentOption() == 0) e = 2;
                            if (menu->getCurrentOption() == 1) e = 3;
                            if (menu->getCurrentOption() == 2) e = 4;
                            if (menu->getCurrentOption() == 3) e = 6;
                            if (menu->getCurrentOption() == 4) e = 0;
                            break;
                        case Event::KeyPressed:
                            switch (event.key.code) {
                                case Keyboard::Down:
                                    menu->goDownKeyboard();
                                    break;

                                case Keyboard::Up:
                                    menu->goUpKeyboard();
                                    break;
                            }
                            break;

                        case Event::KeyReleased:

                            if (event.key.code == 58) {
                                if (menu->getCurrentOption() == 0) e = 2;
                                if (menu->getCurrentOption() == 1) e = 3;
                                if (menu->getCurrentOption() == 2) e = 4;
                                if (menu->getCurrentOption() == 3) e = 6;
                                if (menu->getCurrentOption() == 4) e = 0;
                            }
                            if (event.key.code == sf::Keyboard::Escape) {
                                e = 0;
                                window->close();
                            }

                    }
                }
                break;

            case 3:
                while (window->pollEvent(event)) {
                    switch (event.type) {
                        case Event::Closed:
                            e = 0;
                            window->close();
                            break;

                        case Event::JoystickMoved:
                            if (event.joystickMove.axis == Joystick::Y) {
                                if (event.joystickMove.position > 10) {
                                    menu->goDown();
                                }

                                if (event.joystickMove.position < -10) {
                                    menu->goUp();
                                }
                            }

                            if (event.joystickMove.axis == Joystick::X) {
                                if (event.joystickMove.position > 10) {
                                    if (menu->getCurrentOption() == 0) menu->increaseRounds();
                                    if (menu->getCurrentOption() == 1) menu->increaseTime();
                                    if (menu->getCurrentOption() == 2) menu->increaseMode();
                                }

                                if (event.joystickMove.position < -10) {
                                    if (menu->getCurrentOption() == 0) menu->decreaseRounds();
                                    if (menu->getCurrentOption() == 1) menu->decreaseTime();
                                    if (menu->getCurrentOption() == 2) menu->decreaseMode();
                                }
                            }
                            break;

                        case Event::KeyPressed:
                            switch (event.key.code) {
                                case Keyboard::Down:
                                    menu->goDownKeyboard();
                                    break;

                                case Keyboard::Up:
                                    menu->goUpKeyboard();
                                    break;
                            }
                            break;
                        case Event::JoystickButtonPressed:
                            if (menu->getCurrentOption() == 3) e = 1;
                            break;

                        case Event::KeyReleased:
                            if (event.key.code == sf::Keyboard::Right) {
                                if (menu->getCurrentOption() == 0) menu->increaseRounds();
                                if (menu->getCurrentOption() == 1) menu->increaseTime();
                                if (menu->getCurrentOption() == 2) menu->increaseMode();
                            }

                            if (event.key.code == sf::Keyboard::Left) {
                                if (menu->getCurrentOption() == 0) menu->decreaseRounds();
                                if (menu->getCurrentOption() == 1) menu->decreaseTime();
                                if (menu->getCurrentOption() == 2) menu->decreaseMode();
                            }

                            if (event.key.code == 58) {
                                if (menu->getCurrentOption() == 3) e = 1;
                            }
                            if (event.key.code == sf::Keyboard::Escape) {
                                e = 0;
                                window->close();
                            }
                            break;

                    }
                }
                break;

            case 4: //opciones del juego
                while (window->pollEvent(event)) {
                    switch (event.type) {
                        case Event::Closed:
                            e = 0;
                            window->close();
                            break;

                        case Event::JoystickMoved:
                            if (event.joystickMove.axis == Joystick::Y) {
                                if (event.joystickMove.position > 10) {
                                    menu->goDown();
                                }

                                if (event.joystickMove.position < -10) {
                                    menu->goUp();
                                }
                            }
                            
                            if (event.joystickMove.axis == Joystick::X) {
                                    if (event.joystickMove.position > 10) {
                                        if (menu->getCurrentOption() == 0) {
                                            menu->increaseMusicVol();
                                            
                                            getMusicPlayer()->setMusicVolume(1);

                                        }

                                        if (menu->getCurrentOption() == 1) {
                                            menu->increaseFXVol();
                                            getMusicPlayer()->setSFXVolume(1);
                                            getMusicPlayer()->playSFX(getMusicPlayer()->shot);
                                        }

                                    }
                                }

                                if (event.joystickMove.axis == Joystick::X) {
                                    if (event.joystickMove.position < -10) {
                                        if (menu->getCurrentOption() == 0) {
                                            menu->decreaseMusicVol();
                                            getMusicPlayer()->setMusicVolume(-1);


                                        }
                                        if (menu->getCurrentOption() == 1) {
                                            menu->decreaseFXVol();
                                            getMusicPlayer()->setSFXVolume(-1);
                                            getMusicPlayer()->playSFX(getMusicPlayer()->shot);

                                        }
                                    }
                                }
                            break;

                        case Event::KeyPressed:
                            switch (event.key.code) {
                                case Keyboard::Down:
                                    menu->goDownKeyboard();
                                    break;

                                case Keyboard::Up:
                                    menu->goUpKeyboard();
                                    break;

                            }
                            break;

                        case Event::JoystickButtonPressed:
                            if (menu->getCurrentOption() == 2) e = 1;
                            break;

                        case Event::KeyReleased:

                            if (event.key.code == sf::Keyboard::Right) {
                                if (menu->getCurrentOption() == 0) {
                                    menu->increaseMusicVol();
                                    getMusicPlayer()->setMusicVolume(1);

                                }

                                if (menu->getCurrentOption() == 1) {
                                    menu->increaseFXVol();
                                    getMusicPlayer()->setSFXVolume(1);
                                    getMusicPlayer()->playSFX(getMusicPlayer()->shot);
                                }
                            }

                            if (event.key.code == sf::Keyboard::Left) {
                                if (menu->getCurrentOption() == 0) {
                                    menu->decreaseMusicVol();
                                    getMusicPlayer()->setMusicVolume(-1);


                                }
                                if (menu->getCurrentOption() == 1) {
                                    menu->decreaseFXVol();
                                    getMusicPlayer()->setSFXVolume(-1);
                                    getMusicPlayer()->playSFX(getMusicPlayer()->shot);

                                }
                            }

                            if (event.key.code == 58) {

                                if (menu->getCurrentOption() == 2) e = 1;
                            }

                            if (event.key.code == sf::Keyboard::Escape) e = 1;
                            break;
                    }
                }

                break;
            case 5: //pausa dentro de partida
                while (window->pollEvent(event)) {
                    switch (event.type) {
                        case Event::Closed:
                            e = 0;
                            window->close();
                            break;

                        case Event::JoystickMoved:
                            if (event.joystickMove.axis == Joystick::Y) {
                                if (event.joystickMove.position > 10) {
                                    menu->goDown();
                                }

                                if (event.joystickMove.position < -10) {
                                    menu->goUp();
                                }
                            }
                            break;

                        case Event::KeyPressed:
                            switch (event.key.code) {
                                case Keyboard::Down:
                                    menu->goDown();
                                    break;

                                case Keyboard::Up:
                                    menu->goUp();
                                    break;
                            }
                            break;

                        case Event::JoystickButtonPressed:
                            if (menu->getCurrentOption() == 0) {
                                for (int i = 0; i < Partida::getInstance()->worldPlayer.size(); i++) {
                                    if (Partida::getInstance()->worldPlayer.at(i) != NULL) {
                                        if (Partida::getInstance()->worldPlayer.at(i)->getPaused()) {
                                            Partida::getInstance()->worldPlayer.at(i)->pause();
                                        }
                                    }
                                }
                                e = 2;
                            }
                            if (menu->getCurrentOption() == 1) {
                                getMusicPlayer()->stopSound(getMusicPlayer()->battleMusic);
                                getMusicPlayer()->playSound(getMusicPlayer()->menuMusic);
                                e = 1;
                            }
                            if (menu->getCurrentOption() == 2) e = 0;
                            break;


                        case Event::KeyReleased:

                            if (event.key.code == 58) {
                                if (menu->getCurrentOption() == 0) {
                                    for (int i = 0; i < Partida::getInstance()->worldPlayer.size(); i++) {
                                        if (Partida::getInstance()->worldPlayer.at(i) != NULL) {
                                            if (Partida::getInstance()->worldPlayer.at(i)->getPaused()) {
                                                Partida::getInstance()->worldPlayer.at(i)->pause();
                                            }
                                        }
                                    }
                                    e = 2;
                                }
                                if (menu->getCurrentOption() == 1) {
                                    getMusicPlayer()->stopSound(getMusicPlayer()->battleMusic);
                                    getMusicPlayer()->playSound(getMusicPlayer()->menuMusic);
                                    e = 1;
                                }
                                if (menu->getCurrentOption() == 2) e = 0;
                            }
                            if (event.key.code == sf::Keyboard::Escape) {
                                e = 2;
                            }
                    }
                }

                break;
        }
    }
}

void Motorgrafico::clearWindow() {
    window->clear(sf::Color::Black);
}

void Motorgrafico::updateWindow() {
    resetFrameTime();
    setHudCameraView();
    updateTemporizador();
    setMainCameraView();
}

void Motorgrafico::displayWindow() {
    window->display();
}

void Motorgrafico::createMusicPlayer() {
    mplayer = new MusicPlayer();
}

MusicPlayer* Motorgrafico::getMusicPlayer() {
    return mplayer;
}

template <class T1> void Motorgrafico::draw(T1& object) {
    window->draw(object);
}

void Motorgrafico::draw(SpriteAnimated &object) {
    window->draw(object);
}

void Motorgrafico::draw(sf::RectangleShape object) {
    window->draw(object);
}

void Motorgrafico::draw(sf::RectangleShape &object) {
    window->draw(object);
}

void Motorgrafico::draw(sf::CircleShape object) {
    window->draw(object);
}

void Motorgrafico::draw(sf::Text object) {
    window->draw(object);
}

void Motorgrafico::draw(Sprite &object) {
    window->draw(object);
}

sf::RenderWindow *Motorgrafico::getRenderWindow() {
    return window;
}

void Motorgrafico::updateTemporizador() {
    temporizador->Update();
}

void Motorgrafico::drawTemporizador() {
    temporizador->Draw(window);
}

void Motorgrafico::resetFrameTime() {
    frameTime = frameClock.restart();
}

sf::Time Motorgrafico::getFrameTime() {
    return frameTime;
}

float Motorgrafico::getCameraPositionX() {
    return (mainCamera->getCenter().x);
}

float Motorgrafico::getCameraPositionY() {
    return (mainCamera->getCenter().y);
}

Temporizador* Motorgrafico::getTemporizador() {
    return (this->temporizador);
}

void Motorgrafico::textEnteredConsole(Event even) {
    if (partida->console.getConsoleInScreen() && even.text.unicode < 128 && (even.text.unicode != 13 && even.text.unicode != 9 && even.text.unicode != 8)) {
        std::string s = "";
        s += static_cast<char> (even.text.unicode);
        partida->console.write(s);
    }
}

void Motorgrafico::createPartida(int tiempo) {
    temporizador = new Temporizador(tiempo, screenWidth / 2, 0, 40);
    partida = Partida::getInstance();
}

Partida* Motorgrafico::getPartida() {
    return partida;
}

void Motorgrafico::deletePartida() {
    delete partida;
    partida = NULL;
}

int Motorgrafico::getWindowHeight() {
    return window->getSize().y;
}

int Motorgrafico::getWindowWidth() {
    return window->getSize().x;
}

void Motorgrafico::setGoToMenu(bool go) {
    goToMenuNow = go;
}

void Motorgrafico::renderizarInstrucciones() {
    
    fondoMenu = new Fondo(Resources::getInstance()->fondoMenu, false);
    mando = new Fondo(Resources::getInstance()->mando, false);
    texto = new Texto("Pulsa un boton para continuar", 20, Resources::getInstance()->menuFont, 100, 0, 0);
    fondoMenu->setPosition(512,300);
    mando->setPosition(512,300);
    texto->setPos(50,550);
    
    window->clear();
    fondoMenu->Render();
    mando->Render();
    draw(texto->getDrawable());
    window->display();
    
}

void Motorgrafico::renderizarInstrucciones2() {
    fondoMenu = new Fondo(Resources::getInstance()->fondoMenu, false);
    teclado = new Fondo(Resources::getInstance()->teclado, false);
    texto = new Texto("Pulsa un boton para continuar", 20, Resources::getInstance()->menuFont, 100, 0, 0);
    fondoMenu->setPosition(512,300);
    teclado->setPosition(512,300);
    texto->setPos(50,550);
    
    window->clear();
    fondoMenu->Render();
    teclado->Render();
    draw(texto->getDrawable());
    window->display();
    
}


Motorgrafico::~Motorgrafico() {
}


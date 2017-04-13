/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Console.h
 * Author: jesus
 *
 * Created on 5 de abril de 2017, 16:02
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string.h>
#include <functional>
//#include "Partida.h"

#define screenWidth 1024
#define screenHeight 600

class Console {
public:

    Console();
    Console(const Console& orig);

    /**
     * Activa y desactiva la consola 
     * @param toggle: si toggle es true muestra la consola, si es false la oculta
     */
    void toggleConsole();

    /**
     * Manda el comando para ejecutarlo
     */
    void send();

    /**
     * Escribe en la consola
     * @param evento con el texto escrito
     */
    void write(sf::Event even);

    /**
     * Funcion que se llama al pulsar la tecla de borrar
     */
    void deleteChar();
    
    
    void loadFunctions();
    
    /**
     * Dibuja la consola por pantalla
     * @param window: Puntero a window que dibuja a la consola
     */
    void draw(sf::RenderWindow *window);

    virtual ~Console();
private:

    bool consoleInScreen;       /**Comprueba si esta la consola en pantalla o no*/
    sf::RectangleShape console; /**Es la forma de la consola*/
    sf::Text *consoleText;      /**Texto que se muestra en la consola*/
    std::string consoleString;  /**String que se va concatenando continuamente y se asigna al texto drawable*/
    std::string commandString;  /**String que contiene el valor del comando actual*/
    int nlines;
    std::map<std::string, std::function<void (int)>> *functionMap;  /**HashMap de las funciones de la consola*/
    
    sf::Font *font;
};

#endif /* CONSOLE_H */

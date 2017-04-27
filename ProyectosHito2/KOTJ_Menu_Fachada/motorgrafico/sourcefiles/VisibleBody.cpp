/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VisibleBody.cpp
 * Author: pedro
 * 
 * Created on 11 de abril de 2017, 17:07
 */

#include "../headerfiles/VisibleBody.h"
#include <math.h>

VisibleBody::VisibleBody(float x, float y, float w, float h, std::string path) {
    
    posX = x;
    posY = y;
    height = h;
    width = w;
    texPath = path;
    angle = 0;
   
    rectangle.setPosition(posX, posY);
    rectangle.setSize(sf::Vector2f(width, height));
    rectangle.setOrigin(sf::Vector2f(width/2, height/2));
    rectangle.setScale(1,1);

    tex = Resources::getInstance();
    
    rectangle.setTexture(&tex->getTexture(texPath));
}

void VisibleBody::updateBody() {
    rectangle.setOrigin(width/2, height/2);
    rectangle.setPosition(posX, posY);
    rectangle.setRotation(angle);
}

void VisibleBody::setPos(float x, float y) {
    posX = x;
    posY = y;
}

void VisibleBody::setDim(float w, float h) {
    height = h;
    width = w;
}

void VisibleBody::setTex(std::string path) {
    texPath = path;
    rectangle.setTexture(&tex->getTexture(texPath));
}

void VisibleBody::setScale(int sX, int sY) {
    rectangle.setScale(sX, sY);
}

void VisibleBody::setAngle(float a) {
    angle = a;
}

float VisibleBody::getPosX() {
    return posX;
}

float VisibleBody::getPosY() {
    return posY;
}

float VisibleBody::getHeight() {
    return height;
}

float VisibleBody::getWidth() {
    return width;
}

std::vector<float> VisibleBody::getPos() {
    std::vector<float> vec(2);
    vec[1] = posX;
    vec[2] = posY;
    return vec;
}

std::vector<float> VisibleBody::getDim() {
    std::vector<float> vec(2);
    vec[1] = width;
    vec[2] = height;
    return vec;
}

std::string VisibleBody::getTexPath() {
    return texPath;
}

sf::RectangleShape VisibleBody::getShape() {
    return rectangle;
}

sf::FloatRect VisibleBody::getBounds() {
    return rectangle.getGlobalBounds();
}


VisibleBody::~VisibleBody() {
}

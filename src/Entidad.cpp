/* 
 * File:   Entidad.cpp
 * Author: daniel
 * 
 * Created on 26 de marzo de 2017, 13:57
 */

#include "Entidad.h"

Entidad::Entidad() {
}

Entidad::Entidad(const Entidad& orig) {
}

void Entidad::Render() {
}

string Entidad::getTag(){
    return tag;
}

void Entidad::Contact(void* punt, string tipo) {
}
void Entidad::Contact(void* punt, string tipo, int id) {
}

Entidad::~Entidad() {
}



/* 
 * File:   Mapa.cpp
 * Author: danielfinestrat
 * 
 * Created on 28 de abril de 2017, 10:42
 */

#include "../headerfiles/Mapa.h"
#include "../motorgrafico/headerfiles/Motorgrafico.h"


Mapa::Mapa(){ }

Mapa::~Mapa() { }

void Mapa::leerMapa(string mapa) {
    
    const char *cstr = mapa.c_str();
    TiXmlDocument doc(cstr);
    doc.LoadFile();

    TiXmlElement* map = doc.FirstChildElement("map");
    
    guardarCapas(map);
    guardarObj(map);
}

void Mapa::guardarCapas(TiXmlElement* map) {
    int _width = 0;
    int _height = 0;
    int _numLayers = 0;
    
    //Leemos el valor de X e Y del Mapa
    map->QueryIntAttribute("width", &_width);
    map->QueryIntAttribute("height", &_height);
    
    //Leemos el número de layers que hay
    TiXmlElement* layer = map->FirstChildElement("layer");
    while (layer) {
        _numLayers++;
        layer = layer->NextSiblingElement("layer");
    }
    
    //Creamos la matriz donde irá la información
    _tilemap = new int**[_numLayers];
    for (int i = 0; i < _numLayers; i++) {
        _tilemap[i] = new int*[_height];
        for (int j = 0; j < _height; j++) {
            _tilemap[i][j] = new int[_width];
            for (int k = 0; k < _width; k++) {
                _tilemap[i][j][k] = 0;
            }
        }
    }
    
    //Leemos las matrices
    TiXmlElement *tile;
    layer = map->FirstChildElement("layer");
    tile = layer->FirstChildElement("data")->FirstChildElement("tile");


    for (int l = 0; l < _numLayers; l++) {
        if (l != 0) {
            layer = layer->NextSiblingElement("layer");
            tile = layer->FirstChildElement("data")->FirstChildElement("tile");
        }
        for (int y = 0; y < _height; y++) {
            for (int x = 0; x < _width; x++) {
                tile->QueryIntAttribute("gid", &_tilemap[l][y][x]);
                //Avanzo al siguiente tag
                tile = tile->NextSiblingElement("tile");
            }
        }

    }
    cargarTiles(_height, _width);
}

void Mapa::guardarObj(TiXmlElement* map) {
    int _numLayers = 0;

    //Leemos el número de layers que hay
    TiXmlElement* layer = map->FirstChildElement("objectgroup");
    while (layer) {
        _numLayers++;
        layer = layer->NextSiblingElement("objectgroup");
    }

    //Leemos las matrices
    TiXmlElement *object;
    TiXmlElement *rozamiento;
    layer = map->FirstChildElement("objectgroup");
    object = layer->FirstChildElement("object");

    int _width = 0;
    int _height = 0;
    int _sizeX = 0;
    int _sizeY = 0;
    string tipo;

    for (int l = 0; l < _numLayers; l++) {
        if (l != 0) {
            layer = layer->NextSiblingElement("objectgroup");
            object = layer->FirstChildElement("object");
        }
        
        while (object) {
            tipo = object->Attribute("name");
            if (tipo.compare("platform") == 0) {
                int _roz;
                rozamiento = object->FirstChildElement("properties")->FirstChildElement("property");
                object->QueryIntAttribute("x", &_width);
                object->QueryIntAttribute("y", &_height);
                object->QueryIntAttribute("width", &_sizeX);
                object->QueryIntAttribute("height", &_sizeY);
                rozamiento->QueryIntAttribute("value", &_roz);
                
                Platform *plataforma = new Platform(_sizeX, _sizeY, (float) _width + _sizeX / 2.0f, (float) _height + _sizeY / 2.0f, ((float) _roz) / 10.0f);
                worldPlatforms.push_back(plataforma);
            }
            else if (tipo.compare("player") == 0) {
                object->QueryIntAttribute("x", &_width);
                object->QueryIntAttribute("y", &_height);
                vector<int> posicion;
                posicion.push_back(_width);
                posicion.push_back(_height);
                
                spawnPlayer.push_back(posicion);
            }
            else if (tipo.compare("esquina") == 0) {
                object->QueryIntAttribute("x", &_width);
                object->QueryIntAttribute("y", &_height);
                vector<int> posicion;
                posicion.push_back(_width);
                posicion.push_back(_height);
                
                esquinasMapa.push_back(posicion);
            }
            object = object->NextSiblingElement("object");
        }
    }

}

void Mapa::cargarTiles(int y, int x) {
    for (int j = 0; j < y; j++) {
        for (int k = 0; k < x; k++) {
            int miTile = _tilemap[0][j][k];
            if (miTile != 0) {
                int ancho = (miTile % 44) - 1;
                int altura = (miTile / 44);
                
                VisibleBody *newTile = new VisibleBody(ancho * 32, altura * 32, 32, 32, Resources::getInstance()->tiles, false);
                newTile->setPos(k * 32, j * 32);
                newTile->setRect(ancho * 32, altura * 32, 32, 32);
                map_sprites.push_back(newTile);
                
            }
        }
    }
}

void Mapa::drawMap() {
    for (int i = 0; i < map_sprites.size(); i++) {
        VisibleBody *body = map_sprites.at(i);
        Motorgrafico::getInstance()->draw(body->getShape());
    }
}

vector< vector<int> > Mapa::getEsquinas(){ return(esquinasMapa); }
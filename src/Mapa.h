
/* 
 * File:   Mapa.h
 * Author: danielfinestrat
 *
 * Created on 28 de abril de 2017, 10:42
 */

#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <string>
#include <cstdlib>
#include "Platform.h"
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include "motorgrafico/VisibleBody.h"
#include "motorgrafico/Fondo.h"

using namespace std;

class Mapa {
public:

    const string mapaPrueba = "./resources/mapas/mapaPrueba.tmx";
    const string mapaSelva = "./resources/mapas/mapaSelva.tmx";
    const string mapaCueva = "./resources/mapas/mapaCueva.tmx";
    const string mapaHielo = "./resources/mapas/mapaHielo.tmx";
    const string mapaSeleccion = "./resources/mapas/mapaSeleccion.tmx";
    const string mapaPodio = "./resources/mapas/mapaPodio.tmx";
    vector<string> mapas;
    
    int ***_tilemap;
    int xTiles;
    int yTiles;
    vector< vector<int> > spawnPlayer;
    
    Mapa();
    virtual ~Mapa();

    void Update();
    void leerMapa(string mapa);
    void cargarXML();
    void cargarTiles(int y, int x);
    void guardarFondo(TiXmlElement* map);
    void guardarCapas(TiXmlElement* map);
    void guardarObj(TiXmlElement* map);
   
    string getRandomMap();
    bool getIfFirstMap();
    vector< vector<int> > getEsquinas();
    vector< vector<int> > getSpawnArmas();
    vector< vector<float> > getSpawnTrampas();
    vector< vector<int> > getSpawnPlayer();
    vector< vector<int> > getSpawnCoins();
    vector<VisibleBody*> aditionalSprites;
    
    void drawMap();
    void drawBackground();
    
private:
    string fondostr;
    Fondo* fondo;
    bool firstMap;
    
    vector<VisibleBody*> map_sprites;
    vector< vector<int> > spawnArmas;
    vector< vector<int> > esquinasMapa;
    vector< vector<float> > spawnTrampas;
    vector<Platform*> worldPlatforms;
    vector< vector<int> > spawnCoins;
    
};

#endif /* MAPA_H */


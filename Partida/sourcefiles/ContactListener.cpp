
/* 
 * File:   ContactListener.cpp
 * Author: daniel
 * 
 * Created on 26 de marzo de 2017, 20:41
 */

#include "../headerfiles/ContactListener.h"
#include "../headerfiles/Partida.h"

void ContactListener::BeginContact(b2Contact* contact) {

    Partida *partida = Partida::getInstance();

    void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
    void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();

    Entidad* entidadA = static_cast<Entidad*> (bodyUserDataA);
    Entidad* entidadB = static_cast<Entidad*> (bodyUserDataB);

    cout << "BOOM colision - " << entidadA->getTag() << " - " << entidadB->getTag() << endl;

    //Colision con Balas
    if (entidadA->getTag().compare("Bala") == 0 || entidadB->getTag().compare("Bala") == 0) {
        if (entidadA->getTag().compare("Bala") == 0) {
            Bala* bala = static_cast<Bala*> (bodyUserDataA);

            if (entidadB->getTag().compare("Player") == 0) {
                Player* player = static_cast<Player*> (bodyUserDataB);
                int dir = -1;
                if (bala->m_pBody->GetPosition().x < player->getPosition().x) dir = 1;
				player->changeDirection(dir);
				partida->players2Delete.push_back(player);
            }

            partida->bullets2Delete.insert(bala);
            
        } else if (entidadB->getTag().compare("Bala") == 0) {
            Bala* bala = static_cast<Bala*> (bodyUserDataB);

            if (entidadA->getTag().compare("Player") == 0) {
                Player* player = static_cast<Player*> (bodyUserDataA);
                int dir = -1;
                if (bala->m_pBody->GetPosition().x < player->getPosition().x) dir = 1;
				player->changeDirection(dir);
				partida->players2Delete.push_back(player);
            }

            partida->bullets2Delete.insert(bala);
        }
    }


}

//void ContactListener::EndContact(b2Contact* contact){}






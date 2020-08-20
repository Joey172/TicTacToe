#pragma once
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <string>
#include "./../game/Tile.h"
using sf::Uint8;
using namespace std;
enum SERVER_STATUS {
  ERROR = 99,
  WAITING_ON_CONNECTIONS,
  READY_TO_START,
};

class Client : public sf::TcpSocket {
public:
  Uint8 id;
  string name;
};

//enum Tile {
//  NONE = 0, CROSS, CIRCLE, TRIANGLE,
//};

struct Point8u {
  Uint8 x;
  Uint8 y;
};

//struct play_t {
//  Point8u majorPos;
//  Point8u minorPos;
//  Tile tile;
//  Uint8 wasFreePlay;
//};

struct gameState_t {
  Tile turn;
} ;
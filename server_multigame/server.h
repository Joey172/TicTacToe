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
constexpr Uint8 NULL_GAME = 0;
class Client : public sf::TcpSocket {
public:
  Tile turn = Tile::NONE;
  Uint8 gId = 0;
  string name = "";
  Uint8 gameId = NULL_GAME;
};

struct Point8u {
  Uint8 x;
  Uint8 y;
};


struct gameState_t {
  Tile turn;
} ;
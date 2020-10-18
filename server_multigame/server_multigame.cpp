#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include "server.h"
#include "./../game/GameSettings.h"
#include "./../game/ttt_util.h"

#include "./../game/History.h"

typedef list<Client*> ClientList;
//typedef std::map<std::string, game_t*> GameMap;
typedef std::string GameCode;



struct game_t {
  Uint8 id = NULL_GAME;
  string name = "";
  History *history = nullptr;
  settings_t settings = g_defaultSettings;
  list<Client*> clients;
  Tile currentTurn = Tile::NONE;
  gameStatus_t status;
};

std::vector<game_t*> games;

settings_t g_settings = g_defaultSettings;

std::map<std::string, game_t*> g_gameMap;
Uint8 g_numPlayers = 0;
Uint8 g_connection_cntr = 1;

list<Client*> connections;
list<Client*> lobby;

// When a player plays a move, that play gets forwarded to all other users.
void HandlePlayMade(game_t *game, Client *clientThatPlayed, sf::Packet move) {
  play_t play;
  const char* data = (char*)move.getData();

  move
    >> (Uint8&)play.majorPos.x >> (Uint8&)play.majorPos.y
    >> (Uint8&)play.minorPos.x >> (Uint8&)play.minorPos.y
    >> (Uint8&)play.wasFreePlay;

  play.majorPos.x = data[0];
  play.majorPos.y = data[1];
  play.minorPos.x = data[2];
  play.minorPos.y = data[3];

  cout << "pos: ";
  cout << 'a' + char(play.majorPos.y) << play.majorPos.x <<
          'a' + char(play.minorPos.y) << play.minorPos.x << endl;

  for (auto client : game->clients) {
    if (client == clientThatPlayed)
      continue;
    client->send(move);
  }
  if(game->history != nullptr)
    game->history->logPlay(play);
  game->currentTurn = IncrementTurn(game->currentTurn, game->settings.numPlayers);
}

//Update each clients list of connected players.
void SendPlayerList(game_t *game) {
  if (game == nullptr) return;
  sf::Packet AllPlayerInfo;
  AllPlayerInfo << (Uint8)DATA_CONTENTS::ALL_PLAYER_INFO 
                << g_numPlayers;

  for (auto client : game->clients) {
    AllPlayerInfo << client->turn << client->name.c_str();
  }

  for (auto client : game->clients) {
    client->send(AllPlayerInfo);
  }
}
void SendJoinConfirm(Client* client) {
  sf::Packet joinConfirm;
  joinConfirm << (Uint8)MODE_JOIN_GAME;
  client->send(joinConfirm);
}
void SendGameList(Client* client) {
  sf::Packet gameList;
  gameList << (Uint8)ALL_GAME_INFO;
  gameList << (Uint8)games.size();

  for (auto& game : games) {
    gameList << (Uint8)game->id;
    gameList << game->settings.board.width << game->settings.board.height << game->settings.numPlayers;
    gameList << game->status;
  }
  client->send(gameList);
}
//update client's currently selected game settings.
void SendGameSettings(game_t *game, Client *client) {
  sf::Packet GameSettings;
  GameSettings 
    << (Uint8)DATA_CONTENTS::SETTINGS_CHANGE << game->settings.board.width << game->settings.board.height << game->settings.numPlayers;

  cout << "sending game settings to " << client->name << endl;
  client->send(GameSettings);
}
// send the start game signal to each client, forcing the game to start for them.
void StartGame(game_t *game) {
  for (auto client : game->clients) {
    sf::Packet packy_boi;
    packy_boi << (Uint8)DATA_CONTENTS::START_GAME;
    client->send(packy_boi);
  }
}
// tell the client what their id is. their id is the same as which tile they play as.
void SendID(Client *client) {
  sf::Packet clientID;
  clientID << (Uint8)DATA_CONTENTS::PLAYER_ID << client->turn;

  cout << "sending ID to " << client->name << endl;
  client->send(clientID);
}
/*
Handles the reciept of packets
*/
bool handlePacket (game_t* game, Client* client, sf::Packet& packet) {
  size_t dataSize = packet.getDataSize();
  Uint8* data = new Uint8[dataSize];
  bool valid = false;
  Uint8 contents = DATA_CONTENTS::INVALID;

  packet >> contents;
  cout << " processing packet (size: " << dataSize << "b, type: " << hex << (int)contents << ")..." << endl;
  memcpy(data, packet.getData(), dataSize);
  { // display data as hex
    cout << "data(hex):";
    for (size_t i = 0; i < dataSize; i++) cout << hex << (int)data[i] << " ";
    cout << endl;
  }
  { // display data as ascii characters
    cout << "data(str):";
    for (size_t i = 0; i < dataSize; i++) cout << data[i];
    cout << endl;
  }

  switch (contents) {
  case DATA_CONTENTS::REQUEST_SETTINGS:
    SendGameSettings(game, client);
    break;
  case DATA_CONTENTS::NEW_NAME:
    packet >> client->name;
    cout << " - \'" << client->name << "' has joined the server!" << endl;
    break;
  case DATA_CONTENTS::PLAY_LOCATION:
    cout << "play location" << endl;
    HandlePlayMade(game, client, packet);
    break;
  case DATA_CONTENTS::START_GAME:
    StartGame(game);
    break;
  case DATA_CONTENTS::SETTINGS_CHANGE:
    packet >> g_settings.board.width >> g_settings.board.height >> g_settings.numPlayers;
    for (auto client : game->clients) {
      SendGameSettings(game, client);
    }
    break;
  case DATA_CONTENTS::MODE_CREATE_GAME:
    cout << "New created by " << client->name << "!" << endl;

    games.push_back(new game_t);
    cout << "Game ID: " << games.size() - 1;
    games.back()->clients.push_back(client);
    games.back()->id = games.size() - 1;
    games.back()->history = new History();
    games.back()->history->Init("Game" + to_string(games.size() - 1));

    client->gameId = games.size() - 1;

    break;
  case DATA_CONTENTS::MODE_JOIN_GAME: {
    Uint8 gameId;
    packet >> gameId;

    if (gameId > games.size()) {
      break;
    }
    lobby.remove(client);
    cout << "Client " << client->name << " joining game " << gameId << endl;
    games[gameId]->clients.push_back(client);
    SendJoinConfirm(client);
    break;
  }
  }
  delete[] data;
  return true;
}
bool handleNewClient(Client* client) {
  cout << " - connection established with client " << client->getRemoteAddress() << endl;

  //if (disconnectedIDs.empty()) {
  //  client->turn = (Tile)g_connection_cntr;
  //  g_connection_cntr++;
  //}
  //else {
  //  client->turn = disconnectedIDs.front();
  //  disconnectedIDs.pop();
  //}
  cout << " - " << client->name << "given id " << client->turn << endl;
  SendID(client);
  SendPlayerList(nullptr);
  return true;
}
int main() {
	bool running = true;
	sf::SocketSelector selector;
	sf::TcpListener listener;
  queue<Uint8> disconnectedIDs;
	SERVER_STATUS status = SERVER_STATUS::WAITING_ON_CONNECTIONS;
  game_t dummyGame;
  game_t dummyGame1;
  game_t dummyGame2;
  games.push_back(&dummyGame);
  games.push_back(&dummyGame1);
  games.push_back(&dummyGame2);
  dummyGame1.id = 1;
  dummyGame2.id = 2;
	listener.listen(4967);
	selector.add(listener);

	while (true) {
		cout << "Waiting..." << endl;
		if (selector.wait()) {
			if (selector.isReady(listener)) { // listener ready
        Client *client = new Client();
        bool valid = true;
				cout << "establishing connection.." << endl;
        valid = (listener.accept(*client) == sf::Socket::Done);
				if (valid) { // Accept new connection
          sf::Packet name_packet, mode_packet;
          
          g_numPlayers += 1;
          
          client->receive(name_packet);
          handlePacket(nullptr, client, name_packet);

          SendGameList(client);
          selector.add(*client);
          connections.push_back(client);
          
          lobby.push_back(client);
          handleNewClient(client);
				} 
        if (!valid) {
					cout << "connection failed" << endl;
					delete client;
				}
			}
			else { // a client is ready
				for (auto client : connections) {
					if (selector.isReady(*client)) {
            cout << "recieving data from " << client->name << "(gid " << (unsigned)client->gameId << ")..." << endl;
            if (client->gameId > games.size()) {
              cout << "invalid gameid, skipping data" << endl;
              continue; //Invalid ID.
            }
						
						sf::Packet packet;
            sf::Socket::Status packet_status;
            packet_status = client->receive(packet);

						if (packet_status==sf::Socket::Done) {
							handlePacket(games[client->gameId], client, packet);
						}
            else if (packet_status == sf::Socket::Disconnected) {
              cout << client->name << " has disconnected!" << endl;

              disconnectedIDs.push(client->turn);
              selector.remove(*client);
              connections.remove(client);

              delete client;
              break;
            }
						else {
							cout << " - error." << endl;
						}
					}
				}
			}
		}
	}
	system("PAUSE");
	return 0;
}

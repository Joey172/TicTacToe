#include <iostream>
#include <cstring>
#include <vector>
#include <queue>
#include <list>

#include "server.h"
#include "./../game/GameSettings.h"

// Dummy game class to enable history.
#include "./../game/History.h"

typedef list<Client*> ClientList;

// 3x3, 2p, online
settings_t g_settings = g_defaultSettings;

gameState_t g_state = { Tile::CROSS };
History g_GameHistory;
Uint8 g_numPlayers = 0;
play_t g_play;
Uint8 g_connection_cntr = 1;

list<Client*> connections;

// When a player plays a move, that play gets forwarded to all other users.
void PlayMade(Client *clientThatPlayed, sf::Packet move) {
  move
    >> (Uint8&)g_play.majorPos.x >> (Uint8&)g_play.majorPos.y
    >> (Uint8&)g_play.minorPos.x >> (Uint8&)g_play.minorPos.y
    >> (Uint8&)g_play.wasFreePlay;

  cout << "pos: ";
  cout << 'a' + char(g_play.majorPos.y) << g_play.majorPos.x <<
          'a' + char(g_play.minorPos.y) << g_play.minorPos.x << endl;

  for (auto client : connections) {
    if (client == clientThatPlayed) continue;
    client->send(move);
  }
  g_state.turn = Tile(1 + (g_state.turn % g_numPlayers));
}

//Update each clients list of connected players.
void SendPlayerList() {
  sf::Packet AllPlayerInfo;
  AllPlayerInfo << (Uint8)DATA_CONTENTS::ALL_PLAYER_INFO 
                << g_numPlayers;

  for (auto client : connections) {
    AllPlayerInfo << client->id << client->name.c_str();
  }

  for (auto client : connections) {
    client->send(AllPlayerInfo);
  }
}

//update client's currently selected game settings.
void SendGameSettings(Client *client) {
  sf::Packet GameSettings;
  GameSettings << (Uint8)DATA_CONTENTS::SETTINGS_CHANGE << g_settings.board.width << g_settings.board.height << g_settings.numPlayers;

  cout << "sending game settings to " << client->name << endl;
  client->send(GameSettings);
}
// send the start game signal to each client, forcing the game to start for them.
void StartGame(list<Client*> &connections) {
  for (auto client : connections) {
    sf::Packet packy_boi;
    packy_boi << (Uint8)DATA_CONTENTS::START_GAME;
    client->send(packy_boi);
  }
}
// tell the client what their id is. their id is the same as which player they are.
void SendID(Client *client) {
  sf::Packet clientID;
  clientID << (Uint8)DATA_CONTENTS::PLAYER_ID << client->id;

  cout << "sending ID to " << client->name << endl;
  client->send(clientID);
}

bool handlePacket (Client *client, sf::Packet &packet) {
  size_t dataSize = packet.getDataSize();
  Uint8 *data = new Uint8[dataSize];
  
  Uint8 contents = DATA_CONTENTS::INVALID;
  
  packet >> contents;
  cout << " processing packet (size: " << dataSize << "b, type: " << hex << (int)contents << ")..." << endl;
  
  memcpy(data, packet.getData(), dataSize);
  
  { // display data as hex
    cout << "data(hex):";
    for (size_t i = 0; i < dataSize; i++) {
      cout << hex << (int)data[i] << " ";
    }
    cout << endl;
  }
  { // display data as ascii characters
    cout << "data(str):";
    for (size_t i = 0; i < dataSize; i++) {
      cout << data[i];
    }
    cout << endl;
  }
  
  switch (contents) {
  case DATA_CONTENTS::REQUEST_SETTINGS: SendGameSettings(client); break;
  case DATA_CONTENTS::NEW_NAME:
    packet >> client->name;
    cout << " - \'" << client->name << "' has joined the server!" << endl;
    break;
  case DATA_CONTENTS::PLAY_LOCATION:
    cout << "play location" << endl;
    PlayMade(client, packet);
    break;
  case DATA_CONTENTS::START_GAME: 
    StartGame(connections);
    break;
  case DATA_CONTENTS::SETTINGS_CHANGE:
    packet >> g_settings.board.width >> g_settings.board.height
      >> g_settings.numPlayers;
    for (auto client : connections) {
      SendGameSettings(client);
    }
    break;
  }

  delete[] data;
	return true;
}

int main() {
	bool running = true;
	sf::SocketSelector selector;
	sf::TcpListener listener;
  queue<Uint8> disconnectedIDs;
	SERVER_STATUS status = SERVER_STATUS::WAITING_ON_CONNECTIONS;

	listener.listen(4967);
	selector.add(listener);

	while (true) {
		cout << "Waiting..." << endl;
		if (selector.wait()) {
			if (selector.isReady(listener)) { // listener ready
        Client *client = new Client();
        g_numPlayers += 1;
				cout << "establishing connection.." << endl;
				if (listener.accept(*client) == sf::Socket::Done) {
          sf::Packet name_packet;
					cout << " - connection established with client " << client->getRemoteAddress() << endl;
					connections.push_back(client);
          client->receive(name_packet);
          handlePacket(client, name_packet);
					selector.add(*client);
          
          if (disconnectedIDs.empty()) {
            client->id = g_connection_cntr;
            g_connection_cntr++;
          }
          else {
            client->id = disconnectedIDs.front();
            disconnectedIDs.pop();
          }
          cout << " - " << client->name << "given id " << client->id << endl;
				} else {
					cout << " - connection failed" << endl;
					delete client;
				}
        SendID(client);
        SendPlayerList();
			}
			else { // one of the clients ready, check em all
				for (auto client : connections) {
					if (selector.isReady(*client)) {
						cout << "recieving data from " << client->name << "..." << endl;
						sf::Packet packet;
            sf::Socket::Status packet_status;
            packet_status = client->receive(packet);

						if (packet_status==sf::Socket::Done) {
							handlePacket(client, packet);
						}
            else if (packet_status == sf::Socket::Disconnected) {
              cout << client->name << " has disconnected!" << endl;

              disconnectedIDs.push(client->id);
              selector.remove(*client);
              connections.remove(client);
              delete client;

              // must break, because client is removed from the
              // connections list we are iterating over
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

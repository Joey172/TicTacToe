#include "OnlinePlay.h"
#include <iostream>
#include <memory.h>

using namespace std;
using sf::Uint8;

void Server::_SendHandler()
{
  sf::Packet sendData;
  m_connection->setBlocking(true);

  if (m_signals.send.moveAvailable) {
    cout << "sending play..." << endl;
    sendData << (Uint8)DATA_CONTENTS::PLAY_LOCATION
      << (Uint8)m_play.majorPos.x << (Uint8)m_play.majorPos.y
      << (Uint8)m_play.minorPos.x << (Uint8)m_play.minorPos.y
      << (Uint8)m_play.wasFreePlay;
    m_connection->send(sendData);
    m_signals.send.moveAvailable = false;
  }
  else if (m_signals.send.startGame) {
    cout << "START_GAME SENT" << endl;
    sendData << (Uint8)DATA_CONTENTS::START_GAME;
    m_connection->send(sendData);
    m_signals.send.startGame = false;
  }
  else if (m_signals.send.requestSettings) {
    cout << "requesting game settings..." << endl;
    sendData << (Uint8)DATA_CONTENTS::REQUEST_SETTINGS;
    m_connection->send(sendData);
    m_signals.send.requestSettings = false;
  }
  else if (m_signals.send.settingsAvailable) {
    sendData << (Uint8)DATA_CONTENTS::SETTINGS_CHANGE
      << m_changedSettings.board.width << m_changedSettings.board.height << m_changedSettings.numPlayers;

    m_connection->send(sendData);
    m_signals.send.settingsAvailable = false;
  }
  
  m_connection->setBlocking(false);
}

void Server::_RecieveHandler()
{
  sf::Packet response;
  Uint8 *data;
  size_t dataSize;

  sf::Uint8 responseContent = DATA_CONTENTS::INVALID;

  m_connection->setBlocking(false);

  sf::Socket::Status status = m_connection->receive(response);
  if (status != sf::Socket::Done) {
    if (status == sf::Socket::Disconnected) {
      m_connected = false;
      cout << "Disconnected From Server" << endl;
    }
    return;
  } 
  response.getData();
  cout << "Recieved response From Server" << endl;
  // display raw data as hex
  dataSize = response.getDataSize();
  data = new Uint8[dataSize];
  memcpy(data, response.getData(), dataSize);
  cout << " - data(hex):";
  for (size_t i = 0; i < dataSize; i++) {
    cout << hex << (int)data[i] << " ";
  }
  cout << endl;
  delete[] data;
  cout << dec;

  // handle response 
  response >> responseContent;
  switch (responseContent) {
  case DATA_CONTENTS::SETTINGS_CHANGE:
    cout << "[L] Settings Changed" << endl;
    response >> m_receivedSettings.board.width >> m_receivedSettings.board.height 
      >> m_receivedSettings.numPlayers;
    cout << " - Settings received: " << (int)m_receivedSettings.board.width << 'x' << (int)m_receivedSettings.board.height << ", " << (int)m_receivedSettings.numPlayers << endl;
    m_receivedSettings.onlineGame = true;
    m_signals.receive.settingsAvailable = true;
    break;
  case DATA_CONTENTS::PLAYER_ID: {
    Uint8 assignedID = 0;
    response >> assignedID;
    cout << "[L] Client ID (turn) assigned: " << (int)assignedID << endl;
    m_assignedTurn = assignedID;
    break;
  }
  case DATA_CONTENTS::START_GAME:
    cout << "[L] START_GAME Received!" << endl;
    m_signals.receive.startGame = true;
    break;
  case DATA_CONTENTS::PLAY_LOCATION:
    cout << "play location" << endl;
    Uint8 mjx, mjy, mnx, mny, fp;
    response >> mjx >> mjy >> mnx >> mny >> fp;
    
    m_play.majorPos.x = mjx; m_play.majorPos.y = mjy;
    m_play.minorPos.x = mnx; m_play.minorPos.y = mny;

    m_play.wasFreePlay = fp!=0;
    cout << "pos:" << endl;
    cout << 'a' + char(m_play.majorPos.y) << m_play.majorPos.x <<
            'a' + char(m_play.minorPos.y) << m_play.minorPos.x << endl;
    m_signals.receive.moveAvailable = true;
    break;
  case DATA_CONTENTS::ALL_PLAYER_INFO:
    response >> m_numPlayers;
    for (Uint8 i = 0; i < m_numPlayers; i++) {
      OnlinePlayer player;
      response >> player.id >> player.name;
      m_players.push_back(player);
      m_signals.receive.namesAvailable = true;
    }
    break;
  default: case DATA_CONTENTS::INVALID: 
    cout << "[L] Invalid response" << endl; 
    break;
  }

}

void Server::_Listener()
{
  // process menu responses
  cout << "[L] listener thread launched" << endl;
  cout << "[L] awaiting server responses." << endl;

  while (Connected()) {
    _SendHandler();
    _RecieveHandler();
    // nap time
    sf::sleep(sf::milliseconds(60));
  }

  cout << "Listener closing..." << endl;
}

Server::Server() : 
  m_listeningThread(&Server::_Listener, this)
{
}


Server::~Server()
{
}

void Server::SendSettingsChange(settings_t newSettings)
{
  m_signals.send.settingsAvailable = true;
  m_changedSettings = newSettings;
}

bool Server::NewSettingsAvailable()
{
  return m_signals.receive.settingsAvailable;
}

settings_t Server::GetSettings()
{
  return m_receivedSettings;
}

bool Server::Connect(sf::IpAddress &ip, string name)
{
  sf::Packet name_packet, mode_packet;

  if (ip == sf::IpAddress::None) {
    cout << "no ip" << endl;
    return false;
  }
  if (name.empty()) {
    name = "no_name";
  }
  m_connection.reset(new sf::TcpSocket);
  cout << "connecting to " << ip << "..." << endl;
  switch (m_connection->connect(ip, 4967)) {
  case sf::Socket::Status::Done:
    cout << " - succesfully connected." << endl;
    m_connected = true;
    
    name_packet << (sf::Uint8)DATA_CONTENTS::NEW_NAME << name.c_str();
    m_connection->send(name_packet);

    mode_packet << (sf::Uint8)DATA_CONTENTS::MODE_CREATE_GAME;
    m_connection->send(mode_packet);

    m_listeningThread.launch();
    m_signals.send.requestSettings = true;
    break;
  case sf::Socket::Status::Partial: case sf::Socket::Status::Disconnected: case sf::Socket::Status::Error: default:
    cout << " - connection failed... whatcha gonna do, cry about it?" << endl;
    m_connected = false;
    return false;
    break;
  }
  return true;
}

void Server::Disconnect()
{
  m_connection->disconnect();
}

bool Server::Connected()
{
  return m_connected;
}

bool Server::PlayerListAvailable()
{
  bool ret = m_signals.receive.namesAvailable;
  m_signals.receive.namesAvailable = false;
  return ret;
}

std::vector<OnlinePlayer> Server::GetPlayers()
{
  return m_players;
}



int Server::GetAssignedTurn()
{
  return m_assignedTurn;
}

int Server::GetCurrentTurn()
{
  return 0;
}


void Server::SendStartGame()
{
  m_signals.send.startGame = true;
}

bool Server::StartGame()
{
  return m_signals.receive.startGame;
}

void Server::PlayMove(play_t move)
{
  m_play = move;
  m_signals.send.moveAvailable = true;
}

bool Server::MoveAvailable()
{
  bool ret = m_signals.receive.moveAvailable;

  if (m_signals.receive.moveAvailable) {
    m_signals.receive.moveAvailable = false;
  }
  return ret;
}

play_t Server::GetPlay()
{
  return m_play;
}

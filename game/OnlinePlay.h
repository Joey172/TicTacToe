#pragma once
#include <vector>
#include <string>
#include <list>
#include <SFML/Network.hpp>
#include <memory>
#include <thread>
#include "GameSettings.h"
#include "History.h"
struct OnlinePlayer {
  sf::Uint8 id;
  std::string name;
};
struct remoteGame_t {
  sf::Uint8 id;
  settings_t settings;
  gameStatus_t status;
};
class Server {
public:
  enum Status {
    WAITING_FOR_PLAYERS,
    READY_TO_START,
    GAME_IN_PROGRESS
  };
  struct lobby_t {
    std::list<remoteGame_t> games;
  };
protected:
  lobby_t m_lobby;
  remoteGame_t *m_joinGame;
  bool m_joining = false;
  bool m_joinSuccess = false;
  sf::Uint8 m_joinID;
  // settings recieved by server. only changed by receipt of SETTINGS_CHANGE
  settings_t m_receivedSettings = g_defaultSettings;
  // proposed settings change.
  settings_t m_changedSettings = g_defaultSettings;

  sf::Uint8 m_numPlayers = 0;
  std::vector<OnlinePlayer> m_players;
  std::shared_ptr<sf::TcpSocket> m_connection;
  sf::Thread m_listeningThread;
  Status m_status = WAITING_FOR_PLAYERS;

  int m_assignedTurn = 0;
  bool m_connected = false;

  play_t m_play;

  struct signals_t {
    struct {
      bool success = false;
      bool clientReady = false;
      bool moveAvailable = false;
      bool requestSettings = false;
      bool settingsAvailable = false;
      bool startGame = false;
      bool namesAvailable = false;
      bool gameListAvailable = false;
      bool joinGame = false;
      bool joinGameFail = false;
    } send, receive;
  } m_signals;
  // handles sending all data to server. runs on listener thread
  void _SendHandler();
  // handles recieving all data from server. runs on listener thread
  void _RecieveHandler();
  // listener thread. waits for server activity
  void _Listener();
public:
  Server();
  ~Server();

  // Attempt to connect to the given ip. returns true if succesful
  bool Connect(sf::IpAddress &ip, std::string name);
  void Disconnect();
  // return true if connected to a server.
  bool Connected();

  bool PlayerListAvailable();
  bool GameListAvailable();

  lobby_t *GetLobby();
  bool JoinGame(sf::Uint8 id);
  std::vector<OnlinePlayer> GetPlayers();

  // tell server to change the settings of the game.
  void SendSettingsChange(settings_t newSettings);
  // return true if new settings have been sent by server
  bool NewSettingsAvailable();

  // Get most recent settings sent from server.
  settings_t GetSettings();

  // return the turn number assigned to client.
  int GetAssignedTurn();
  // returns the game turn that the server is currently on
  int GetCurrentTurn();

  void SendStartGame();
  bool StartGame();

  
  void PlayMove(play_t);
  bool MoveAvailable();
  play_t GetPlay();
  //returns a vector containing the names of all players
  std::vector<std::string> GetPlayerNames();
};
// Information about the game recieved from the remote server.

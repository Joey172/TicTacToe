#pragma once
#include <SFML\Config.hpp>
#include <string>
#include <vector>
// These structures are common to both game and server.
struct settings_t {
	struct {
		sf::Uint8 width;
		sf::Uint8 height;
	} board;
	sf::Uint8 numPlayers;
  //bool allowUndo;
  //bool doReplay;
	bool onlineGame;
};
	

bool operator==(const settings_t& lhs, const settings_t& rhs);
bool operator!=(const settings_t& lhs, const settings_t& rhs);
// 3x3, 2p, not online
constexpr settings_t g_defaultSettings = { { 3,3 }, 2, false };

enum DATA_CONTENTS : sf::Uint8 {
  INVALID = 99,
  // menu commands
	SETTINGS_CHANGE   = 0x31, // sent to notify setting change.
  REQUEST_SETTINGS  = 0x32, // sent to request current settings
	START_GAME        = 0x35, // sent to indicate the game is ready to start
  PLAYER_ID         = 0x34, // data will contain player id
  NEW_NAME          = 0x38, // data will contain string
  ALL_PLAYER_INFO   = 0x81, // data will contain # of players and the id/name of each player
  // game commands
	PLAY_LOCATION     = 0x77, // data will contain play_t.
	HOVER_LOCATION     = 0x78, // data will contain play_t.

};
#include "Tile.h"
#include "GameSettings.h"

// Returns the next tile in the turn order after currentTile.
Tile IncrementTurn(Tile currentTurn, unsigned numPlayers);
// Returns the previous tile in the turn order before currentTile.
Tile DecrementTurn(Tile currentTurn, unsigned numPlayers);

settings_t StringToGameSettings(std::string str);
std::string GameSettingsToString(settings_t settings);

std::string WindowsOnlyOpenFileDialog();
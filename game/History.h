#pragma once

#include <vector>
#include <fstream>
#include <string>

#include "Tile.h"
#include "GameSettings.h"

//class Game;
using namespace std;


class History {
  vector<play_t> m_history;
  fstream m_saveFile;
  settings_t m_settings = g_defaultSettings;

  bool m_doneReading = false;
  unsigned m_fileSize;
  unsigned m_numMoves = 0;
  unsigned m_currMove = 0;
public:
  bool Init(string fileName);
  void logPlay(play_t play);
  bool LoadFromFile(string FileName);

  unsigned GetNumMoves();
  unsigned GetCurrMove();
  play_t GetMove(unsigned index);

  play_t GetLastPlay();
  void RemoveLastPlay();

  History();
  ~History();
};


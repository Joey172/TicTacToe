#include "History.h"
#include <iostream>
#include <string>
#include "ttt_util.h"
using namespace std;

#define _CRT_SECURE_NO_WARNINGS

struct hist_entry {
  char mjrBoard[2] = { 'a', '0' };
  char mnrBoard[2] = { 'a', '0' };
};

void History::logPlay(play_t play)
{
  char str[7];
  m_history.push_back(play);

  if (m_saveFile.is_open()) {
    // convert play to chess-like notationn and save to text file.

    unsigned n = sprintf_s(str, 7, "%c%i%c%i ",
                           'a' + char(play.majorPos.y), play.majorPos.x,
                           'a' + char(play.minorPos.y), play.minorPos.x);

    m_saveFile << str << endl;
    cout << n;
    m_saveFile.flush();

    flush(m_saveFile);
    if (m_saveFile.fail()) {
      char buff[50];
      strerror_s(buff, 50, errno);
      cout << "Error: " << buff;
    }
  }
  printf("%c%i %c%i\n", // cout doesn't work for some reason..
         'a' + char(play.majorPos.y), play.majorPos.x,
         'a' + char(play.minorPos.y), play.minorPos.x);

  //cout << ('a' + char(play.majorPos.x)) << '0' + char(play.majorPos.y) << " ";
  //cout << ('a' + char(play.minorPos.x)) << play.minorPos.y << endl;
}

bool History::LoadFromFile(string fileName)
{


  char mjrBoard[2] = { 'a', '0' };
  char mnrBoard[2] = { 'a', '0' };


  if (!m_saveFile.is_open()) {
    m_saveFile.open("CURRENT_GAME.TicTacToe", std::ios::in | std::ios::binary);
    m_saveFile.ignore(std::numeric_limits<std::streamsize>::max());
    m_fileSize = m_saveFile.gcount();
    m_saveFile.clear();   //  Since ignore will have set eof.
    m_saveFile.seekg(0, std::ios_base::beg);
    m_saveFile.close();
    m_numMoves = m_fileSize / 6;
    cout << m_numMoves << " number of moves to load" << endl;
    m_saveFile.open("CURRENT_GAME.TicTacToe", fstream::in);
  }
  Tile currTurn = Tile::CROSS;
  while (!m_saveFile.eof()) {
    play_t loadedMove = { {-1,-1},{-1,-1},Tile::NONE, false };
    string moveEntry;
    unsigned x = 0;
    unsigned y = 0;
    bool correctMove = true; // only play move if valid
    m_saveFile >> moveEntry;
    if (moveEntry == "") {
      return true;
    }
    loadedMove.majorPos.x = moveEntry[1];
    loadedMove.majorPos.y = moveEntry[0];
    loadedMove.minorPos.x = moveEntry[3];
    loadedMove.minorPos.y = moveEntry[2];


    
    this->logPlay(loadedMove);
       //mjrBoard[0] = moveEntry[1];
      //mjrBoard[1] = moveEntry[0];
      //mnrBoard[0] = moveEntry[3];
      //mnrBoard[1] = moveEntry[2];

      //x = mnrBoard[0] - '0';
      //y = mnrBoard[1] - 'a';
      //if (m_pGame) {
      //  correctMove = m_pGame->GetActiveBoard()->Play(m_pGame->GetTurn(), x, y);
      //}
      //if (!correctMove) {
      //
      //}
      //else {
      //  turn = Tile(turn + 1);
      //  m_currMove += 1;
      //}
    IncrementTurn(currTurn, m_settings.numPlayers);
  }
  m_saveFile.close();
  return false;
}

bool History::Init(string fileName)
{
  m_saveFile.close();
  m_currMove = 0;

  if (!fileName.empty()) {
    int options = fstream::out;
    options |= fstream::trunc;
    m_saveFile.open(fileName, options);
  }
  return true;
}

unsigned History::GetNumMoves()
{
  return m_numMoves;
}

unsigned History::GetCurrMove()
{
  return m_currMove;
}

play_t History::GetMove(unsigned index)
{
  return m_history[index];
}

play_t History::GetLastPlay()
{
  return m_history.back();
}

void History::RemoveLastPlay()
{
  if (!m_history.empty()) {
    m_history.pop_back();
  }
}

History::History()
{
}


History::~History()
{
}

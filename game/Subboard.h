#pragma once
#include "Tile.h"
#include "SFML/Window.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
using namespace std;
class Game;

extern unsigned g_boardWidth, g_boardHeight;
//extern const unsigned g_boardWidth, g_boardHeight;
class Subboard {
  //Tile m_board[g_boardWidth][g_boardHeight];

  //index individual tiles with [x][y]
  vector<vector<Tile>> m_board;
  float m_width=500, m_height=300;
  sf::Vector2f m_pos = { 0,0 }; // position of top left corner relative to (0,0) of window
  sf::Vector2i m_gamePos = { -1,-1 }; // 
  Point m_hoverPos;
  Game *m_pGame;
  unsigned m_winCounts[4] = { 0 };
  float m_tileSize;
  float m_oldTileSize=1;
  float m_scale = 1;
  float m_spacing;
  Tile m_owner = NONE;
  std::vector<std::pair<sf::Vector2u, sf::Vector2u>> m_winEndPoints;
public:
  void Draw(sf::RenderWindow *pWin);
  bool Play(Tile, unsigned x, unsigned y);
  bool HandleClicks(sf::Vector2f mousePos, Tile turn);
  void Clear();
  Tile GetOwner();
  void SetPos(sf::Vector2f);
  sf::Vector2f GetPos();
  void SetDim(sf::Vector2f);
  sf::Vector2f GetDim();

  float GetTileSize() const;
  float GetSpacing() const;
  bool isFull();
  void setGamePtr(Game *pGame);
  void Update();
  void WinCondition();
  void RemoveTile(unsigned x, unsigned y);
  Point GetHoverPos();
  Subboard(sf::Vector2f pos, sf::Vector2f dim);
  Subboard();
  ~Subboard();
};


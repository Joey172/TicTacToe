#include "Subboard.h"
#include <iostream>
#include <algorithm>
#include "Game.h"
using namespace std;

void Subboard::Draw(sf::RenderWindow * pWin)
{
  if (m_oldTileSize != m_tileSize) {

  }
  m_oldTileSize = m_tileSize;
  m_tileSize = m_width / g_boardWidth;
  m_spacing = g_boardWidth;
  sf::RectangleShape sqrboi; // a very sqr boi.
  m_scale = .9*m_tileSize/47;
  // Draw winner underlay.
  {
    if (m_owner != NONE ) {
      sqrboi.setTexture(m_pGame->GetTileGraphic(m_owner));
      sqrboi.setPosition(m_pos);
      sqrboi.setSize({ m_width, m_height });
      sqrboi.setFillColor(sf::Color(255, 255, 255, 64)); // make transparent
      pWin->draw(sqrboi);
    }
  }
  // undo transperency
  sqrboi.setFillColor(sf::Color(255, 255, 255, 255));
  
  // Draw gridlines
  {
    sf::RectangleShape gridLine;
    switch (m_owner) {
      case CROSS:    gridLine.setFillColor(sf::Color::Green); break;
      case CIRCLE:   gridLine.setFillColor(sf::Color::Red); break;
      case TRIANGLE: gridLine.setFillColor(sf::Color::Blue); break;
    }
      // vertical gridlines
    gridLine.setSize({ 2.0f, m_width * (g_fboardHeight/g_fboardWidth) });
    for (unsigned x = m_tileSize; x < m_tileSize*g_boardWidth - 7; x += m_tileSize) {
      gridLine.setPosition(m_pos + sf::Vector2f({ float(x), 0.0f }));
      pWin->draw(gridLine);
    }
      // hori gridlines
    gridLine.setSize({ m_width, 2.0f });
    for (unsigned y = m_tileSize; y < m_tileSize*g_fboardHeight - 7; y += m_tileSize) {
      gridLine.setPosition(m_pos + sf::Vector2f({ 0.0f, float(y) }));
      pWin->draw(gridLine);
    }
  }
  
  // Draw tiles
  { 

    sqrboi.setSize({ m_tileSize - 2* m_spacing, m_tileSize - 2* m_spacing });
    for (unsigned y = 0; y < g_boardHeight; y++) {
      for (unsigned x = 0; x < g_boardWidth; x++) {
        if (m_board[x][y] == NONE) {
          continue;
        }
        else {
          sf::Sprite tileBoi;

          tileBoi.setTexture(*m_pGame->GetTileGraphic(m_board[x][y]));
          sqrboi.setTexture(m_pGame->GetTileGraphic(m_board[x][y]));
          
          //tileBoi.setSize({ m_tileSize - 2 * m_spacing, m_tileSize - 2 * m_spacing });
          sf::Vector2f pos = m_pos + sf::Vector2f({ x*(m_width / g_fboardWidth), y*(m_width / g_fboardWidth) });
          float scale = m_tileSize / m_oldTileSize;
          tileBoi.setScale(m_scale, m_scale);
          pos += {float(m_spacing), float(m_spacing)};
          tileBoi.setColor({ 255,0,0 });
          tileBoi.setPosition(pos);
          pWin->draw(tileBoi);
        }
      }
    }
  }

  // Draw win lines
  { 
    for (auto points : m_winEndPoints) {
      sf::RectangleShape winLine;
      winLine.setSize({ m_tileSize * 2.0f, 2.0f });
      float x1 = points.first.x * m_tileSize + m_tileSize / 2;
      float y1 = points.first.y * m_tileSize + m_tileSize / 2;
      float x2 = points.second.x * m_tileSize + m_tileSize / 2;
      float y2 = points.second.y * m_tileSize + m_tileSize / 2;
      winLine.setOrigin(0, 1);
      winLine.setRotation(atan2f(x2 - x1, y2 - y1) * (180 / 3.14159) + 90);
      winLine.setPosition(m_pos + sf::Vector2f{ x1, y1 });

      pWin->draw(winLine);
    }
  }

  // Draw hover tile.
  {
    unsigned x = this->GetHoverPos().x, y= this->GetHoverPos().y;
    bool inRange = !(x < 0 || x > g_boardWidth - 1 || y < 0 || y > g_boardHeight - 1);
    if (!inRange) {
      return;
    }
    bool freePlay   = m_pGame->IsFreePlayAllowed();
    bool hoverBoard = (this == m_pGame->GetActiveBoard() || freePlay);
    bool tileClear  =  m_board[x][y] == NONE;

    if (inRange && tileClear && hoverBoard) {
      sqrboi.setTexture(m_pGame->GetTileGraphic(m_pGame->GetTurn()));
      sqrboi.setSize({ m_tileSize - 2 * m_spacing, m_tileSize - 2 * m_spacing });
      sf::Vector2f pos = m_pos + sf::Vector2f({ x*(m_width / g_fboardWidth), y*(m_width / g_fboardWidth) });
      pos += {float(m_spacing), float(m_spacing)};
      sqrboi.setPosition(pos);
      sqrboi.setFillColor(sf::Color(255, 255, 255, 64));
      pWin->draw(sqrboi);
    }
  }
}

bool Subboard::Play(Tile play, unsigned x, unsigned y)
{
  if (x > g_boardWidth - 1 || y > g_boardHeight - 1) {
    return false;
  }
  if (m_board[x][y] == NONE && (this == m_pGame->GetActiveBoard() || m_pGame->IsFreePlayAllowed())) {
    m_board[x][y] = play;
    if (m_pGame->IsFreePlayAllowed()) {
      m_pGame->AllowFreePlay(false);
    }

    m_pGame->SetActiveBoard({ x,y });

    if (m_pGame->GetActiveBoard()->isFull()) {
      m_pGame->AllowFreePlay(true);
    }

    m_pGame->IncrementTurn();
    return true;
  }
  return false;
}

bool Subboard::HandleClicks(sf::Vector2f mousePos, Tile turn)
{
  bool success = false;
  sf::Vector2f pos;
  if (mousePos.x < m_pos.x || mousePos.x > m_pos.x + m_width) {
    return false;
  }
  if (mousePos.y < m_pos.y || mousePos.y > m_pos.y + m_height) {
    return false;
  }

  pos.x = mousePos.x - m_pos.x;
  pos.y = mousePos.y - m_pos.y;

  pos.x /= (m_width / g_boardWidth);
  pos.y /= (m_height / g_boardHeight);

 
 // cout << "X: " << unsigned(mousePos.x) << "Y: " << unsigned(mousePos.y) << endl;
 // cout << "X: " << unsigned(pos.x) << "Y: " << unsigned(pos.y) << endl;
  if (success = Play(turn, unsigned(pos.x), unsigned(pos.y))) {
    
    //cout << char('a' + char(pos.y)) << char('1' + char(pos.x));
  }
  return success;
}

void Subboard::Clear()
{
  for (unsigned y = 0; y < g_boardHeight; y++) {
    for (unsigned x = 0; x < g_boardWidth; x++) {
      m_board[x][y] = NONE;
    }
  }
}

Tile Subboard::GetOwner()
{
  return m_owner;
}

void Subboard::SetPos(sf::Vector2f pos)
{
  m_pos = pos;
}

sf::Vector2f Subboard::GetPos()
{
  return m_pos;
}

void Subboard::SetDim(sf::Vector2f dim)
{
  m_width = dim.x;
  m_height = dim.y;
}

sf::Vector2f Subboard::GetDim()
{
  return { m_width, m_height };
}

float Subboard::GetTileSize() const
{
  return m_tileSize;
}

float Subboard::GetSpacing() const
{
  return m_spacing;
}

bool Subboard::isFull()
{
  for (auto row : m_board) {
    for (auto tile : row) {
      if (tile == NONE) {
        return false;
      }
    }
  }
  return true;
}

void Subboard::setGamePtr(Game * pGame)
{
  m_pGame = pGame;
}

void Subboard::Update()
{
  // Figure out hover tile.
  sf::Vector2f pos = m_pGame->m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_pGame->m_window), m_pGame->m_gameView);

  if (pos.x < m_pos.x || pos.x > m_pos.x + m_width || pos.y < m_pos.y || pos.y > m_pos.y + m_height) {
    m_hoverPos = { 99, 99 };
  } else {

    pos.x -= m_pos.x;
    pos.y -= m_pos.y;
    pos.x /= (m_width / g_boardWidth);
    pos.y /= (m_height / g_boardHeight);

    m_hoverPos = { unsigned(pos.x), unsigned(pos.y) };
  }
  // check for 3's in a row.
  WinCondition();
}

void Subboard::WinCondition()
{
  // Returns true if the 3 tiles are the same.
  auto isWin = [](Tile a, Tile b, Tile c) {
    return (a == b) && (a == c) && (a != Tile::NONE);
  };
  Tile a, b, c;
  for (unsigned i = 1; i < 4; i++) {
    m_winCounts[i] = 0;
  }
  // coloumn win condition
  for (unsigned x = 0; x < g_boardWidth; x++) {
    a = m_board[x][0]; b = m_board[x][1]; c = m_board[x][2];
    m_winCounts[a] += isWin(a, b, c);
    if (isWin(a, b, c)) m_winEndPoints.push_back({ {x,0}, {x,2} });
  }
  // row win condition
  for (unsigned y = 0; y < g_boardHeight; y++) {
    for (unsigned x = 0; x < g_boardWidth -2; x++) {
      a = m_board[x][y]; b = m_board[x+1][y]; c = m_board[x+2][y];
      m_winCounts[a] += isWin(a, b, c);
      if (isWin(a, b, c)) m_winEndPoints.push_back({ {x,y}, {x+2,y} });
    }
  }
  // diag tl to br win condition
  for (unsigned x = 0; x < g_boardWidth - 2; x++) {
    a = m_board[x][0]; b = m_board[x + 1][1]; c = m_board[x + 2][2];
    m_winCounts[a] += isWin(a, b, c);
    if (isWin(a, b, c)) m_winEndPoints.push_back({ {x,0}, {x+2,2} });
  }
  // diag tl to br win condition
  for (unsigned x = 2; x < g_boardWidth; x++) {
    a = m_board[x][0]; b = m_board[x - 1][1]; c = m_board[x - 2][2];
    m_winCounts[a] += isWin(a, b, c);
    if (isWin(a, b, c)) m_winEndPoints.push_back({ {x,0}, {x-2,2} });
  }

  m_owner = NONE;

  if ((m_winCounts[CROSS] > m_winCounts[CIRCLE]) && (m_winCounts[CROSS] > m_winCounts[TRIANGLE])) {
    m_owner = CROSS;
  }
  if ((m_winCounts[TRIANGLE] > m_winCounts[CROSS]) && (m_winCounts[TRIANGLE] > m_winCounts[CIRCLE])) {
    m_owner = TRIANGLE;
  }
  if ((m_winCounts[CIRCLE] > m_winCounts[CROSS]) && (m_winCounts[CIRCLE] > m_winCounts[TRIANGLE])) {
    m_owner = CIRCLE;
  }
}

void Subboard::RemoveTile(unsigned x, unsigned y)
{
  m_board[x][y] = NONE;
}

Point Subboard::GetHoverPos()
{

  return m_hoverPos;
}

Subboard::Subboard(sf::Vector2f pos, sf::Vector2f dim)
{
  m_pos = pos;
  m_width = dim.x;
  m_height = dim.y;
  Clear();
}
Subboard::Subboard()
{
  m_board.resize(g_boardWidth);
  for (auto &row : m_board) {
    row.resize(g_boardHeight);
  }
  Clear();
}

Subboard::~Subboard()
{
}
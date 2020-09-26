#pragma once
#include <vector>
#include <list>
#include <memory>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Subboard.h"
#include "History.h"
#include "GUI/GameButton.h"
#include "GameSettings.h"
#include "OnlinePlay.h"

const sf::Vector2f SCREEN_SIZE = { 300, 50 };

// float representation of the number of tiles wide the board and each subboard is
extern float g_fboardWidth;
// float representation of the number of tiles tall the board and each subboard is
extern float g_fboardHeight;
// ratio of board width to height
extern float g_widthByHeight;
extern sf::Font *g_tahoma;

class Game {
private:
  settings_t m_settings;
  sf::View m_view;
  unsigned m_width = 600, m_height = 600;
  const float m_spacing = 20;
  const float m_toolbarHeight = 38; // px
  float m_gameHeight, m_gameWidth;
  sf::Text m_score, m_timer;

  bool m_isOnline;

  shared_ptr<Server> m_server;

  Tile m_turn;
  Tile m_prevTurn ;
  Tile m_nextTurn;
  Tile m_victor = NONE;

  Point m_activeBoard = { 0,0 };
  Subboard* m_hoverBoard;
  Point m_hover;

  //The Undo button
  GameButton m_btnUndo;
  GameButton m_btnBack;
  GameButton m_txtConfirm;
  GameButton m_btnYes, m_btnNo;
  bool m_exit = false;

  vector <GameButton*> m_uiButtons;


  vector<shared_ptr<sf::Texture>> m_tileTextures;


  vector <OnlinePlayer> m_players;
  OnlinePlayer *m_currPlayer;
  vector <GameButton> m_btnPlayers;

  bool m_mouseClick = false;
  bool m_running = false;

  bool m_freePlay = true;
  // index individual boards by [x][y]
  vector<vector<Subboard>> m_subboards;
public:
  History m_hist;
  bool m_replayInProgress = false;
  std::shared_ptr<sf::RenderWindow> m_window;
  sf::View m_gameView;
  sf::View m_toolBarView;

  Game(std::shared_ptr<sf::RenderWindow>, vector<shared_ptr<sf::Texture>> tileTextures, const settings_t gameSettings, shared_ptr<Server> server);
  ~Game();

  void Restart();
  void Resize(unsigned w, unsigned h);
  void SetActiveBoard(Point boardPos);
  Subboard* GetActiveBoard();
  Subboard* GetHoverBoard();

  void Init(bool newgame);
  void HandleInput();
  void Update(sf::Time dt);
  void Draw();
  void Exit();

  void Play(Point mjrPos, Point mnrPos);
  void IncrementTurn();
  void DecrementTurn();
  Tile GetTurn();
  sf::Texture* GetTileGraphic(Tile tile);
  //
  void AllowFreePlay(bool);
  bool IsFreePlayAllowed(void);
  //true if it's this clients turn in online mode.
  bool OnlinePlayAllowed();

  /*
  take out the trash
  */
  void Cleanup();

  /*
  Creates asteroids randomaly scattered about.
  */
  bool IsRunning();
};


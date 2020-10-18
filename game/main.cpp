// ULTRAULTIMATE3PTICTACTOE.cpp : Defines the entry point for the console application.
//

#include <SFML/Window.hpp>

#include "Game.h"
#include "Menu.h"
#include <iostream>
#include <stack>
#include <memory>
#include <vector>
#include "GameSettings.h"
#include "OnlinePlay.h"
#include "inih.h"
#include "ttt_util.h"
using namespace std;

settings_t g_settings = g_defaultSettings;
// number of tiles wide the board and each subboard is
unsigned g_boardWidth = 5;
// number of tiles tall the board and each subboard is
unsigned g_boardHeight = 3;

float g_fboardWidth = float(g_boardWidth);
float g_fboardHeight = float(g_boardHeight);
float g_widthByHeight = g_boardWidth / g_boardHeight;
unsigned g_numPlayers = 2;
sf::Font *g_tahoma;

#if defined WIN32 && defined NDEBUG
int WinMain()
#else
int main()
#endif

{
  settings_t gameSettings = g_defaultSettings;
  vector<shared_ptr<sf::Texture>> tileTextures;

  std::shared_ptr<sf::RenderWindow> 
    mainWindow( new sf::RenderWindow({ 300, 300 }, "Tic Tac Toe"));

  
  //Load font
  g_tahoma = new sf::Font();
  g_tahoma->loadFromFile("./assets/font.ttf");

  // load tile textures
  // insert blank tile.
  tileTextures.push_back(make_shared<sf::Texture>());
  int i = 1;
  bool tileLoaded = true;
  while (tileLoaded) {
    shared_ptr<sf::Texture> tileTexture
      = make_shared<sf::Texture>();

    tileLoaded = tileTexture->loadFromFile("./assets/p" + to_string(i++) + ".png");
    if (tileLoaded) {
      tileTextures.push_back(tileTexture);
    }
  }
  cout << "Loaded " << i - 2 << " tiles" << endl;

  sf::Image icon;
  if (icon.loadFromFile("./assets/icon.ico")) {
    mainWindow->setIcon(47, 47, icon.getPixelsPtr());
  }



  shared_ptr<Server> server;
  server.reset(new Server);

  sf::Time dt = sf::seconds(0);
  sf::Clock timer;

  timer.restart();

  bool runMenu = true;

  Menu menu(mainWindow);
  Menu::Event menuEvent = Menu::Event::NONE;
  menu.Init(server);
  do { 
    mainWindow->setSize({ 300,300 });
    //mainWindow->setView(mainWindow->getDefaultView());

    while (runMenu) { // MENU LOOP
      menuEvent = Menu::Event::NONE;
      menu.HandleInput();
      menu.Update(dt);
      menu.Draw();

      menuEvent = menu.m_event;

      switch (menuEvent) {
      case Menu::Event::NONE: default:
        break;
      case Menu::Event::RESUME_GAME: case Menu::Event::NEW_GAME: {
        gameSettings = menu.GetSettings();
        /////////////////////////////////////////////////
        // TODO get rid of globals
        g_boardWidth = gameSettings.board.width;
        g_boardHeight = gameSettings.board.height;
        g_fboardWidth = float(gameSettings.board.width);
        g_fboardHeight = float(gameSettings.board.height);
        g_widthByHeight = g_boardWidth / g_boardHeight;
        g_numPlayers = gameSettings.numPlayers;
        /////////////////////////////////////////////////
        runMenu = false;
        break;
      }
      }

      sf::sleep(sf::milliseconds(16) - timer.getElapsedTime());
      dt = timer.restart();
    }


    Game game(mainWindow,tileTextures, gameSettings, server);

    // for to do replay
    bool newGame = menuEvent == Menu::Event::NEW_GAME;
    game.Init(newGame);


    while (game.IsRunning()) { // game loop
      game.HandleInput();
      game.Update(dt);
      game.Draw();

      sf::sleep(sf::milliseconds(16) - timer.getElapsedTime());
      dt = timer.restart();
    }
    runMenu = false;
    if (mainWindow->isOpen()) { // if the window is open,  go back to menu.
      runMenu = true;
    }
  } while (runMenu);
  delete g_tahoma;
  return 0;
}

bool operator==(const settings_t& lhs, const settings_t& rhs)
{
  bool ret = (lhs.board.width == rhs.board.width)
    && (lhs.board.height == rhs.board.height)
    && (lhs.numPlayers == rhs.numPlayers)
    && (lhs.onlineGame == rhs.onlineGame);
  return ret;
}

bool operator!=(const settings_t& lhs, const settings_t& rhs)
{
  bool ret = !(lhs == rhs);
  return ret;
}

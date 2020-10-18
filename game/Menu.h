#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <stack>
#include <map>
#include "GUI/GameButton.h"
#include "OnlinePlay.h"
using namespace std;

struct screen_t {
  screen_t *Parent = nullptr;
  std::map<string, std::shared_ptr<GameButton>> buttons;
};
enum ScreenState {
  MainScreen,
  OnlinePlaySelection,
  SingleServerConnect,
  MultiServerConnect,
  Lobby,
  PreGameLobby
};

class Menu {
protected:
    bool m_running;
    settings_t m_settings = g_defaultSettings;
    std::shared_ptr<Server> m_server;
    std::shared_ptr<sf::RenderWindow> m_window;
    vector<GameButton*> m_btns;
    std::map<string, GameButton> m_buttons;
    std::map<ScreenState, screen_t> m_screens;
    ScreenState m_currScreen;

    vector <OnlinePlayer> m_players;
    vector <GameButton> m_btnPlayers;
    list <GameButton> m_gameListBtn;
    stack<ScreenState> m_screenHist;
    void PushScreen(ScreenState nextScreen);
    ScreenState PopScreen();
    screen_t* CurrentScreen();

    Menu* m_currMenu;
public:
     Menu(std::shared_ptr<sf::RenderWindow> win);
    static void InitWindow();

    virtual void Init(std::shared_ptr<Server> server);
    //setsup the current screen.
    virtual void InitScreen(ScreenState screen);
    virtual void HandleInput();
    virtual void Update(sf::Time dt);
    //runs the exit procedure for the current screen
    //virtual void UninitScreen();
    virtual void Draw();
    
    bool IsRunning() {
        return m_running;
    }
    settings_t GetSettings();
    enum Event {
        NONE, CLOSED, RESUME_GAME, START_SERVER, NEW_GAME, MAIN_MENU
    } m_event;
};
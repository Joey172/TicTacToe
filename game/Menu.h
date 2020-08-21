#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <vector>
#include <memory>
#include "GameButton.h"
#include "OnlinePlay.h"
using namespace std;


class Menu {
protected:
    bool m_running;
    settings_t m_settings = g_defaultSettings;
    std::shared_ptr<sf::RenderWindow> m_window;
    vector<GameButton*> m_btns;
public:
    static void InitWindow();
    virtual void HandleInput();
    virtual void Update(sf::Time dt) = 0;
    virtual void Draw();

    bool IsRunning() {
        return m_running;
    }
    settings_t GetSettings();
    enum Event {
        NONE, CLOSED, RESUME_GAME, START_SERVER, NEW_GAME, MAIN_MENU
    } m_event;
};
class MainMenu : public Menu {
    GameButton m_btnResumeGame;
    GameButton m_btnNewGame;
    GameButton m_btnStartServer;
    GameButton m_btnChangeWidth;
    GameButton m_btnChangeHeight;
    GameButton m_btnChangeNumPlayers;
public:
    MainMenu(std::shared_ptr<sf::RenderWindow> win);
    ~MainMenu();

    void Update(sf::Time dt);
};

class OnlineMenu : public Menu {
    // All the buttons used to navigate the online menu.
    GameButton m_btnNewGame;
    GameButton m_btnBack;

    GameButton m_btnChangeWidth;
    GameButton m_btnChangeHeight;
    GameButton m_btnChangeNumPlayers;

    GameButton m_btnIPBox;
    GameButton m_btnName;
    // Input text boxes.
    GameButton m_inputIP;
    GameButton m_txtName;
    //info text
    GameButton m_txtStatus;
    GameButton m_btnStatus;

    vector<shared_ptr<sf::Texture>> m_tileTextures;

    sf::IpAddress m_ip = sf::IpAddress::None;
    std::shared_ptr<Server> m_server;

    vector <OnlinePlayer> m_players;
    vector <GameButton> m_btnPlayers;

    // used to keep track of where user is in menu
    enum srv_men_state {
        NAME_INPUT,
        IP_INPUT,
        PORT_INPUT,
        READY,
    } m_state;
    // used to keep track of connection status
    enum srv_state {
        NOT_CONNECTED,
        FAILED_TO_CONNECT, // implies not connected
        CONNECTED,
    } m_srvStatus;

public:

    OnlineMenu(std::shared_ptr<sf::RenderWindow>, vector<shared_ptr<sf::Texture>>);
    ~OnlineMenu();
    void Init(std::shared_ptr<Server> server);
    string m_name;
    virtual void HandleInput() override;
    void Update(sf::Time dt);
    virtual void Draw() override;

};


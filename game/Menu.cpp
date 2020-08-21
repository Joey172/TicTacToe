#include <iostream>
#include <math.h>
#include "Menu.h"
#include "GameSettings.h"
using namespace std;

extern unsigned g_boardWidth, g_boardHeight;
extern float g_fboardWidth;
extern float g_fboardHeight;
extern float g_widthByHeight;
extern unsigned g_numPlayers;
extern sf::Font* g_tahoma;

//sf::RenderWindow* Menu::m_window = nullptr;
void Menu::InitWindow()
{
  //m_window = new sf::RenderWindow();
  //m_window->create({ 300, 300 }, "TicTacToe");
}

MainMenu::MainMenu(std::shared_ptr<sf::RenderWindow> pwin)
{
  m_window = pwin;
  m_event = MainMenu::Event::NONE;
  m_btnNewGame.SetText("New Game -");
  m_btnNewGame.SetPosition(20, 20);

  m_btnResumeGame.SetText("Resume Game");
  m_btnResumeGame.SetPosition(20, 70);

  m_btnStartServer.SetText("Online Play");
  m_btnStartServer.SetPosition(20, 180);

  m_btnChangeWidth.SetText("0");
  m_btnChangeWidth.SetPosition(195, 20);
  m_btnChangeWidth.SetActive(true);
  m_btnChangeHeight.SetText("0");
  m_btnChangeHeight.SetPosition(230, 20);

  m_btnChangeNumPlayers.SetText("");
  m_btnChangeNumPlayers.SetPosition(250, 20);

  m_btns.push_back(&m_btnResumeGame);
  m_btns.push_back(&m_btnNewGame);
  m_btns.push_back(&m_btnStartServer);

  m_btns.push_back(&m_btnChangeHeight);
  m_btns.push_back(&m_btnChangeWidth);
  m_btns.push_back(&m_btnChangeNumPlayers);

  m_running = true;
}


MainMenu::~MainMenu()
{
}

void Menu::HandleInput()
{
  sf::Event event;
  while (m_window->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      m_running = false;

      m_window->close();
    }

    if (event.type == sf::Event::MouseButtonPressed) {
      sf::Vector2f pos = { -1,-1 };

      pos.x = float(sf::Mouse::getPosition(*m_window).x);
      pos.y = float(sf::Mouse::getPosition(*m_window).y);

      for (auto& btn : m_btns) {
        btn->HandleInput(pos);
      }
    }
  }
}

struct boardSetting {
  unsigned width;
  unsigned height;
  unsigned numPlayers;
  string name;
};

void MainMenu::Update(sf::Time dt) {
  static unsigned i = 0;
  settings_t changedSettings = m_settings;

  for (auto& btn : m_btns) {
    btn->Update();
  }
  m_event = NONE;
  if (m_btnResumeGame.WasClicked()) {
    m_event = RESUME_GAME;
  }
  else if (m_btnNewGame.WasClicked()) {
    m_event = NEW_GAME;
  }
  else if (m_btnChangeWidth.WasClicked()) {
    changedSettings.board.width = (m_settings.board.width + 1) % 7;
    if (changedSettings.board.width <= 2) changedSettings.board.width = 3;
  }
   else if (m_btnChangeHeight.WasClicked()) {
    changedSettings.board.height = (m_settings.board.height + 1) % 7;
    if (changedSettings.board.height <= 2) changedSettings.board.height = 3;
  }
  else if (m_btnChangeNumPlayers.WasClicked()) {
    changedSettings.numPlayers = (changedSettings.numPlayers + 1) % 7;
  }
  else if (m_btnStartServer.WasClicked()) {
    m_event = Event::START_SERVER;
  }

  if (m_event != NONE) {
    m_running = false;
  }
  m_settings = changedSettings;

  m_btnChangeWidth.SetText(to_string(m_settings.board.width) + "x");
  m_btnChangeHeight.SetText(to_string(m_settings.board.height));
  m_btnChangeNumPlayers.SetText(to_string(m_settings.numPlayers) + "p");
}

void Menu::Draw()
{
  m_window->clear();
  for (auto& btn : m_btns) {
    btn->Draw(m_window.get());
  }

  m_window->display();
}

settings_t Menu::GetSettings()
{
  return m_settings;
}



OnlineMenu::OnlineMenu(std::shared_ptr<sf::RenderWindow> gameWin, vector<shared_ptr<sf::Texture>> tileTextures) 
    : m_tileTextures(tileTextures) {
  m_window = gameWin;
  m_btns.push_back(&m_txtName);

  m_btns.push_back(&m_btnNewGame);
  m_btns.push_back(&m_btnName);
  m_btns.push_back(&m_btnBack);
  m_btns.push_back(&m_btnIPBox);
  m_btns.push_back(&m_inputIP);
  m_btns.push_back(&m_txtStatus);
  m_btns.push_back(&m_btnStatus);
  //settings
  m_btns.push_back(&m_btnChangeHeight);
  m_btns.push_back(&m_btnChangeWidth);
  m_btns.push_back(&m_btnChangeNumPlayers);

  m_running = true;
}

OnlineMenu::~OnlineMenu()
{
}

void OnlineMenu::Init(std::shared_ptr<Server> server)
{
  m_server = server;
  m_state = NAME_INPUT;
  m_event = OnlineMenu::Event::NONE;
  //Name Entry
  m_txtName.SetText("Enter Name:");
  m_txtName.SetPosition(50, 15);
  m_txtName.SetActive(true);
  m_btnName.SetActive(true);
  m_btnName.SetText("");
  m_btnName.SetPosition(50, 50);

  //IP Entry, appears after name entry.
  m_inputIP.SetText("Enter Host IP:");
  m_inputIP.SetPosition(50, 15);
  m_inputIP.SetActive(false);
  m_btnIPBox.SetText("");
  m_btnIPBox.SetPosition(50, 50);
  m_btnIPBox.SetActive(false);
  // Common Controls
  m_btnNewGame.SetText("Start Game");
  m_btnNewGame.SetPosition(70, 50);
  m_btnNewGame.SetActive(false);

  // Settings Controls
  m_btnChangeWidth.SetText("0");
  m_btnChangeWidth.SetPosition(95, 20);
  m_btnChangeWidth.SetActive(false);

  m_btnChangeHeight.SetText("0");
  m_btnChangeHeight.SetPosition(130, 20);
  m_btnChangeHeight.SetActive(false);

  m_btnChangeNumPlayers.SetText("");
  m_btnChangeNumPlayers.SetPosition(150, 20);
  m_btnChangeNumPlayers.SetActive(false);

  m_btnBack.SetText("Back");
  m_btnBack.SetPosition(10, 240);

  m_txtStatus.SetText("Status:");
  m_txtStatus.SetPosition(4, 4);
  m_txtStatus.SetActive(true);
  m_txtStatus.SetHowBigItIs(15);
  m_btnStatus.SetText("");
  m_btnStatus.SetPosition(100, 4);
  m_btnStatus.SetActive(true);
  m_btnStatus.SetHowBigItIs(15);
}
void OnlineMenu::HandleInput() {
  sf::Event event;
  static GameButton* activeTextField = &m_btnName;
  while (m_window->pollEvent(event)) {
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Key::BackSpace)
      {
        activeTextField->SetText(activeTextField->GetText().erase(activeTextField->GetText().size() - 1));
      }
      else if (event.key.code == sf::Keyboard::Key::Return)
      { // ENTER key pressed
        switch (m_state) {
        case NAME_INPUT:
          m_inputIP.SetActive(true);
          m_btnIPBox.SetActive(true);
          m_txtName.SetActive(false);
          m_btnName.SetActive(false);
          m_name = m_btnName.GetText();
          activeTextField = &m_btnIPBox;

          m_state = IP_INPUT;
          break;
        case IP_INPUT:
          m_ip = m_btnIPBox.GetText();
          cout << "IP Box Text:" << m_btnIPBox.GetText() << endl;
          cout << "IP Address Obtained: " << m_ip << endl;

          m_btnStatus.SetText("Connecting...");

          if (m_server->Connect(m_ip, m_name)) {
            m_inputIP.SetActive(false);
            m_btnIPBox.SetActive(false);
            m_btnBack.SetText("Disconnect");
            m_btnNewGame.SetActive(true);
            m_btnNewGame.SetActive(true);
            m_btnChangeHeight.SetActive(true);
            m_btnChangeWidth.SetActive(true);
            m_btnChangeNumPlayers.SetActive(true);
            m_btnStatus.SetText("Connected to " + m_ip.toString());

            m_state = READY;
          }
          else {
            Init(m_server);
            m_btnStatus.SetText("Failed to connect.");
          }
          break;
        case READY:

          break;
        }
      }
    }
    else if (event.type == sf::Event::TextEntered)
    {
      char c = static_cast<char>(event.text.unicode);
      if (c != 8 && c != '\n' && c != '\r')
        activeTextField->SetText(activeTextField->GetText() + c);
    }
    if (event.type == sf::Event::Closed) {
      m_running = false;
      m_window->close();
    }

    if (event.type == sf::Event::MouseButtonPressed) {
      sf::Vector2f pos = { -1,-1 };

      pos.x = float(sf::Mouse::getPosition(*m_window).x);
      pos.y = float(sf::Mouse::getPosition(*m_window).y);

      for (auto& btn : m_btns) {
        btn->HandleInput(pos);
      }
    }
  }
}


void OnlineMenu::Update(sf::Time dt) {
  settings_t changedSettings = m_settings;
  settings_t receivedSettings = m_settings;

  for (auto& btn : m_btns) {
    btn->Update();
  }

  //retrieve new settings
  if (m_server->NewSettingsAvailable()) {
    receivedSettings = m_server->GetSettings();
    m_settings = receivedSettings;
    changedSettings = m_settings;
  }
  // update display of players
  if (m_server->PlayerListAvailable()) {
    m_btnPlayers.clear();

    m_players = m_server->GetPlayers();
    for (auto& player : m_players) {
      m_btnPlayers.push_back(GameButton(to_string(player.id) + ". " + player.name, g_tahoma));
      m_btnPlayers.back().SetPosition(10, 90 + 30 * (player.id - 1));
    }
  }
  // send request to change settings if width, height or numPlayers btn pressed.
  if (m_btnChangeWidth.WasClicked()) {
    changedSettings.board.width = (m_settings.board.width + 1) % 7;
    if (changedSettings.board.width <= 2) changedSettings.board.width = 3;
  }
  if (m_btnChangeHeight.WasClicked()) {
    changedSettings.board.height = (m_settings.board.height + 1) % 7;
    if (changedSettings.board.height <= 2) changedSettings.board.height = 3;
  }
  if (m_btnChangeNumPlayers.WasClicked()) {
    changedSettings.numPlayers = (changedSettings.numPlayers + 1) % 7;
  }
  if (changedSettings != m_settings) {
    m_server->SendSettingsChange(changedSettings);
  }
  if (m_btnNewGame.WasClicked()) {
    //m_server->SetReady(true);
    m_server->SendStartGame();
  }
  if (m_btnBack.WasClicked()) {
    if (m_server->Connected()) {
      
    }
    m_event = MAIN_MENU;
  }
  if (m_server->StartGame()) {
    m_event = NEW_GAME;
  }
  if (m_event != NONE) {
    m_running = false;
  }

  m_btnChangeWidth.SetText(to_string(m_settings.board.width) + "x");
  m_btnChangeHeight.SetText(to_string(m_settings.board.height));
  m_btnChangeNumPlayers.SetText(to_string(m_settings.numPlayers) + "p");
}

void OnlineMenu::Draw()
{
  sf::RectangleShape cursor({ 5,35 });
  cursor.setPosition(50 + 15 * m_btnIPBox.GetText().size(), 50);
  m_window->clear();
  static int cntr = 0;
  cntr++;
  int trans = 256 * sin((cntr + 1) / 10);

  cursor.setFillColor(sf::Color(255, 255, 255, trans));
  m_window->draw(cursor);


  for (auto name : m_btnPlayers) {
    sf::RectangleShape sqrboi;
    sf::Color colors[] = { sf::Color::White, sf::Color(130,255,130), sf::Color(255,130,130), sf::Color(130,130,255) };

    int id = name.GetText()[0] - '0';
    float iconSize = 22;
    sqrboi.setTexture(m_tileTextures[id].get());
    sqrboi.setPosition(name.GetRect().left + name.GetRect().width + 27, name.GetRect().top - (24 - 22) / 2);
    sqrboi.setSize({ iconSize, iconSize });
    sqrboi.setFillColor(sf::Color(255, 255, 255, 64)); // make transparent
    m_window->draw(sqrboi);

    name.SetColor(colors[id]);
    name.Draw(m_window.get());
  }


  for (auto& btn : m_btns) {
    btn->Draw(m_window.get());
  }

  m_window->display();
}


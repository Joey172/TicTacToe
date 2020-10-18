#include <iostream>
#include <math.h>
#include "Menu.h"
#include "GameSettings.h"
#include "GUI/EditBox.h"
using namespace std;

extern unsigned g_boardWidth, g_boardHeight;
extern float g_fboardWidth;
extern float g_fboardHeight;
extern float g_widthByHeight;
extern unsigned g_numPlayers;
extern sf::Font* g_tahoma;

void Menu::PushScreen(ScreenState nextScreen)
{
  ScreenState prevScreen = m_currScreen;
  
  m_screenHist.push(nextScreen);
  m_currScreen = m_screenHist.top();
  //CurrentScreen()->Parent = &m_screens[prevScreen];
}

ScreenState Menu::PopScreen()
{
  if (m_screenHist.size() != 1) {
    m_screenHist.pop();
  }
  m_currScreen = m_screenHist.top();
  return m_screenHist.top();
}

screen_t* Menu::CurrentScreen()
{
  return &m_screens[m_screenHist.top()];
}

Menu::Menu(std::shared_ptr<sf::RenderWindow> win)
  : m_window(win)
{
}

//sf::RenderWindow* Menu::m_window = nullptr;
void Menu::InitWindow()
{
  //m_window = new sf::RenderWindow();
  //m_window->create({ 300, 300 }, "TicTacToe");
}

void Menu::Init(std::shared_ptr<Server> server) {
  m_server = server;

  m_screens.emplace(MainScreen, screen_t());
  
  m_screens.emplace(OnlinePlaySelection, screen_t());
  m_screens.emplace(SingleServerConnect, screen_t());
  m_screens.emplace(MultiServerConnect, screen_t());
  m_screens.emplace(PreGameLobby, screen_t());
  m_screens.emplace(Lobby, screen_t());

  PushScreen(MainScreen);
  {/// Main Menu buttons
    m_screens[MainScreen].buttons.emplace("NewGame",    new GameButton{ "New Game -", { 20, 20 } });
    m_screens[MainScreen].buttons.emplace("Resume",     new GameButton{ "Resume Game", { 20, 70 } });
    m_screens[MainScreen].buttons.emplace("Online",     new GameButton{ "Online Play", { 20, 180 } });
    m_screens[MainScreen].buttons.emplace("Width",      new GameButton{ "0", { 195, 20 },Gui::Style::UNDERLINE });
    m_screens[MainScreen].buttons.emplace("Height",     new GameButton{ "0", { 230, 20 },Gui::Style::UNDERLINE });
    m_screens[MainScreen].buttons.emplace("NumPlayers", new GameButton{ "0", { 250, 20 },Gui::Style::UNDERLINE });
  }
  {
    m_screens[OnlinePlaySelection].buttons.emplace("EnterLobby", new GameButton{ "Enter Online Lobby", {20,15} });
    m_screens[OnlinePlaySelection].buttons.emplace("HostIPGame", new GameButton{ "Host IP Game", {20,50} });
    m_screens[OnlinePlaySelection].buttons.emplace("JoinIPGame", new GameButton{ "Join IP Game", {20,85} });
  }
  {
    m_screens[SingleServerConnect].buttons.emplace("NamePrompt", new GameButton{ "Enter Name:", { 20, 15 } });
    m_screens[SingleServerConnect].buttons.emplace("NameInput",   new TextInput { "", { 30, 50 },220.f });
    m_screens[SingleServerConnect].buttons.emplace("IPPrompt",     new GameButton{ "Enter IP", { 20, 95 } });
    m_screens[SingleServerConnect].buttons.emplace("IPInput",       new TextInput{ "", { 30, 130 }, 220.f });
    m_screens[SingleServerConnect].buttons.emplace("Status",         new GameButton{ "", {20,225 } });
  }
  {
    m_screens[MultiServerConnect].buttons.emplace("NamePrompt", new GameButton{ "Enter Name:", { 20, 15 } });
    m_screens[MultiServerConnect].buttons.emplace("NameInput", new TextInput{ "", { 30, 50 },220.f });
    m_screens[MultiServerConnect].buttons.emplace("Status", new GameButton{ "", {20,225 } });
  }
  {
    //m_screens[Lobby].buttons.emplace("Status", new GameButton{ "Status", {25,35} });
    m_screens[Lobby].buttons.emplace("Create", new GameButton{ "Create", {25,225} });
  }
  {
    m_screens[PreGameLobby].buttons.emplace("Start Game", new GameButton{ "Host IP Game", {25,50} });
    m_screens[PreGameLobby].buttons.emplace("JoinIPGame", new GameButton{ "Join IP Game", {25,85} });
  }
  m_event = Event::NONE;

  for (auto& screen : m_screens) {
    screen.second.buttons.emplace("Back", new GameButton{ "<Back", { 10, 260 } });
  }

  m_running = true;
}

void Menu::InitScreen(ScreenState screen)
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

    sf::Vector2f mousePos = {
      mousePos.x = float(sf::Mouse::getPosition(*m_window).x),
      mousePos.y = float(sf::Mouse::getPosition(*m_window).y) };
    bool anyButtonClicked = false;
    for (auto& btn : m_screens[m_currScreen].buttons) {
      btn.second->HandleInput(event, mousePos);
      anyButtonClicked = btn.second->WasClicked(false) || anyButtonClicked;
    }
    for (auto& gameJoinBtn : m_gameListBtn) {
      gameJoinBtn.HandleInput(event, mousePos);
    }
    if (event.type == sf::Event::MouseButtonPressed && !anyButtonClicked) {
      GameButton::SetActiveButton(nullptr);
    }
  }
}
void Menu::Update(sf::Time dt)
{
  m_event = Event::NONE;
  
  for (auto& btn : CurrentScreen()->buttons) {
    btn.second->Update();
  }
  if (CurrentScreen()->buttons["Back"]->WasClicked()) {
    PopScreen();
  }
  switch (m_currScreen) {
  case ScreenState::MainScreen: // MAIN SCREEN TURN ON
    if (m_screens[MainScreen].buttons["Online"]->WasClicked()) {
      PushScreen(OnlinePlaySelection);
    }
    else if (m_screens[MainScreen].buttons["Resume"]->WasClicked()) {
      m_event = RESUME_GAME;
    }
    else if (m_screens[MainScreen].buttons["NewGame"]->WasClicked()) {
      m_event = NEW_GAME;
    }
    break;
  case ScreenState::OnlinePlaySelection:
    if (m_screens[OnlinePlaySelection].buttons["JoinIPGame"]->WasClicked()) {
      PushScreen(SingleServerConnect);
    }
    else if (m_screens[OnlinePlaySelection].buttons["EnterLobby"]->WasClicked()) {
      PushScreen(MultiServerConnect);
    }
    break;
  case ScreenState::SingleServerConnect: {
    if (((TextInput*)m_screens[SingleServerConnect].buttons["IPInput"].get())->IsReady()
      && ((TextInput*)m_screens[SingleServerConnect].buttons["NameInput"].get())->IsReady()) {
      bool succ = m_server->Connect(
        sf::IpAddress(m_screens[SingleServerConnect].buttons["IPInput"]->GetText()),
        m_screens[SingleServerConnect].buttons["NameInput"]->GetText());
      if (succ) {
        PushScreen(PreGameLobby);
      }
      else {
        GameButton::SetActiveButton(m_screens[SingleServerConnect].buttons["NameInput"].get());
        m_screens[SingleServerConnect].buttons["Status"]->SetText("Failed to connect");
      }
    }
    break;
  }
  case ScreenState::MultiServerConnect: {
    if (((TextInput*)m_screens[MultiServerConnect].buttons["NameInput"].get())->IsReady()) {
      bool succ = m_server->Connect(
        (sf::IpAddress&)sf::IpAddress::LocalHost,
        m_screens[MultiServerConnect].buttons["NameInput"]->GetText());
      if (succ) {
        PushScreen(Lobby);
      }
      else {
        GameButton::SetActiveButton(m_screens[MultiServerConnect].buttons["NameInput"].get());
        m_screens[MultiServerConnect].buttons["Status"]->SetText("Failed to connect");
      }
    }
    break;
  }
  case ScreenState::Lobby: {
    if (!m_server->Connected()) {
      PopScreen();
      break;
    }
    if (m_server->GameListAvailable()) {
      int y = 0;
      for (auto& game : m_server->GetLobby()->games) { // populate screen with clickable buttons to join game
        m_gameListBtn.push_back(
          GameButton(to_string((unsigned)game.id) + ": " + to_string((unsigned)game.settings.board.width) + "x" + to_string((unsigned)game.settings.board.height),
            { 15.0f, 15.0f + 35*(y++) }));
        m_gameListBtn.back().SetHowBigItIs(15);

      }
    }

    for (auto& gameJoinBtn : m_gameListBtn) {
      gameJoinBtn.Update();
      if (gameJoinBtn.WasClicked()) {
        if (m_server->JoinGame(1)) {
          PushScreen(ScreenState::PreGameLobby);
          m_gameListBtn.clear();
          break;
        }
      }
    }
    break;
  }
  case ScreenState::PreGameLobby: {
    settings_t changedSettings = m_settings;
    settings_t receivedSettings = m_settings;
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
    if (m_server->StartGame()) {
      m_event = NEW_GAME;
    }
    if (m_event != NONE) {
      m_running = false;
    }
    break;
  }
  }
}
void Menu::Draw()
{
  m_window->clear();


  for (auto& btn : m_screens[m_currScreen].buttons) {
    btn.second->Draw(m_window.get());
  }
  for (auto& btn : m_gameListBtn) {
    btn.Draw(m_window.get());
  }
  for (auto& btn : m_btnPlayers) {
    btn.Draw(m_window.get());
  }
  m_window->display();
}

settings_t Menu::GetSettings()
{
  return m_settings;
}

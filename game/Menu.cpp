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
  
  // send request to change settings if width, height or numPlayers btn pressed.
  //if (m_btnChangeWidth.WasClicked()) {
  //  changedSettings.board.width = (m_settings.board.width + 1) % 7;
  //  if (changedSettings.board.width <= 2) changedSettings.board.width = 3;
  //}
  //if (m_btnChangeHeight.WasClicked()) {
  //  changedSettings.board.height = (m_settings.board.height + 1) % 7;
  //  if (changedSettings.board.height <= 2) changedSettings.board.height = 3;
  //}
  //if (m_btnChangeNumPlayers.WasClicked()) {
  //  changedSettings.numPlayers = (changedSettings.numPlayers + 1) % 7;
  //}
  //if (changedSettings != m_settings) {
  //  m_server->SendSettingsChange(changedSettings);
  //}
  //if (m_btnNewGame.WasClicked()) {
  //  //m_server->SetReady(true);
  //  m_server->SendStartGame();
  //}
  //if (m_btnBack.WasClicked()) {
  //  if (m_server->Connected()) {
  //
  //  }
  //  m_event = MAIN_MENU;
  //}


  //m_btnChangeWidth.SetText(to_string(m_settings.board.width) + "x");
  //m_btnChangeHeight.SetText(to_string(m_settings.board.height));
  //m_btnChangeNumPlayers.SetText(to_string(m_settings.numPlayers) + "p");

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


MainMenu::MainMenu(std::shared_ptr<sf::RenderWindow> pwin) : Menu(pwin)
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
  m_btnChangeWidth.SetEnabled(true);
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





OnlineMenu::OnlineMenu(std::shared_ptr<sf::RenderWindow> gameWin, vector<shared_ptr<sf::Texture>> tileTextures) 
    : Menu(gameWin), m_tileTextures(tileTextures){
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
  m_txtName.SetEnabled(true);
  m_btnName.SetEnabled(true);
  m_btnName.SetText("");
  m_btnName.SetPosition(50, 50);

  //IP Entry, appears after name entry.
  m_inputIP.SetText("Enter Host IP:");
  m_inputIP.SetPosition(50, 15);
  m_inputIP.SetEnabled(false);
  m_btnIPBox.SetText("");
  m_btnIPBox.SetPosition(50, 50);
  m_btnIPBox.SetEnabled(false);
  // Common Controls
  m_btnNewGame.SetText("Start Game");
  m_btnNewGame.SetPosition(70, 50);
  m_btnNewGame.SetEnabled(false);
  // Server Controls

  // Settings Controls
  m_btnChangeWidth.SetText("0");
  m_btnChangeWidth.SetPosition(95, 20);
  m_btnChangeWidth.SetEnabled(false);

  m_btnChangeHeight.SetText("0");
  m_btnChangeHeight.SetPosition(130, 20);
  m_btnChangeHeight.SetEnabled(false);

  m_btnChangeNumPlayers.SetText("");
  m_btnChangeNumPlayers.SetPosition(150, 20);
  m_btnChangeNumPlayers.SetEnabled(false);

  m_btnBack.SetText("Back");
  m_btnBack.SetPosition(10, 240);

  m_txtStatus.SetText("Status:");
  m_txtStatus.SetPosition(4, 4);
  m_txtStatus.SetEnabled(true);
  m_txtStatus.SetHowBigItIs(15);
  m_btnStatus.SetText("");
  m_btnStatus.SetPosition(100, 4);
  m_btnStatus.SetEnabled(true);
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
          m_inputIP.SetEnabled(true);
          m_btnIPBox.SetEnabled(true);
          m_txtName.SetEnabled(false);
          m_btnName.SetEnabled(false);
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
            m_inputIP.SetEnabled(false);
            m_btnIPBox.SetEnabled(false);
            m_btnBack.SetText("Disconnect");
            m_btnNewGame.SetEnabled(true);
            m_btnNewGame.SetEnabled(true);
            m_btnChangeHeight.SetEnabled(true);
            m_btnChangeWidth.SetEnabled(true);
            m_btnChangeNumPlayers.SetEnabled(true);
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
    else if (event.type == sf::Event::TextEntered) {
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
        //btn->HandleInput(pos);
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


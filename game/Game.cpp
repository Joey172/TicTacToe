#include "Game.h"
#include <iostream>
#include <math.h>
#include <SFML/Network.hpp>
#include "ttt_util.h"
using namespace std;
extern float g_fboardWidth;
extern float g_fboardHeight;
extern float g_widthByHeight;
extern unsigned g_numPlayers;

void Game::Init(bool newGame)
{
  if (newGame) {
    m_hist.Init("AutoSave.txt");
  }
  else {
    m_hist.LoadFromFile(WindowsOnlyOpenFileDialog());
    m_replayInProgress = true;
  }

    
  for (unsigned i = 0; i < m_hist.GetNumMoves(); i++) {
    Update(sf::seconds(0));
    sf::Event event;
    while (m_window->pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        m_running = false;
        m_window->close();
      }
      if (event.type == sf::Event::Resized) {
        if (event.size.width != m_width)
          Resize(event.size.width, event.size.height);
      }
      if (event.type == sf::Event::MouseButtonPressed && m_victor == NONE) {
      }
    }
    play_t loadedPlay = m_hist.GetMove(i);
    m_subboards[loadedPlay.majorPos.x][loadedPlay.majorPos.y].Play(loadedPlay.tile, loadedPlay.minorPos.x, loadedPlay.minorPos.y);
    float currMove = float(m_hist.GetCurrMove());
    float numMoves = float(m_hist.GetNumMoves());
    float speed = std::pow(currMove / numMoves, 2);
    sf::sleep(sf::seconds(1 * speed));
    Draw();

  }
  m_running = true;

  m_turn = CROSS;
  m_prevTurn = Tile(g_numPlayers);
  m_nextTurn = CIRCLE;
}
void Game::Resize(unsigned width, unsigned height) {
  // if you want to avoid a headache, do not touch this.
  m_gameView.setSize(m_width, m_height);
  m_gameView.setCenter(m_width / 2, m_height / 2);

  m_gameWidth = (height - m_toolbarHeight) * (g_fboardWidth / g_fboardHeight);

  if(m_gameWidth > width) {
    cout << "Height controlled by Width" << endl;
    
    m_gameWidth = m_width;
    m_gameHeight = m_gameWidth * (g_fboardHeight / g_fboardWidth)- m_toolbarHeight;
  }
  else {
    cout << "Width controlled by Height" << endl;
    m_gameHeight = m_height - m_toolbarHeight;
    m_gameWidth = (m_gameHeight) * (g_fboardWidth / g_fboardHeight);
  }

  // apply offsets
  float xOff = -(m_width - m_gameWidth) / 2;
  float yOff = -m_toolbarHeight - ((m_height-m_gameHeight-m_toolbarHeight)/2);
  cout << "X offset: " << xOff << endl;
  cout << "Y offset: " << yOff << endl;

  m_gameView.move(xOff, yOff);

  // print
  cout << "-------------RESIZED--------------" << endl;
  cout << "game w/h: " << m_gameWidth << ", " << m_gameHeight << endl;
  cout << "g ratio:" << m_gameWidth / m_gameHeight << endl;
  cout << "w/h: " << width << ", " << height << endl;
  cout << "----------------------------------" << endl << endl;

  m_toolBarView.setSize(m_width, m_height);
  m_toolBarView.setCenter(m_width / 2.0f, (m_height) / 2.0f);

  m_gameWidth -= m_spacing * g_boardWidth;
  m_gameHeight -= m_spacing * g_boardHeight;

  //calculate subboard dimensions
  float subbrdWidth = m_gameWidth / g_fboardWidth;
  float subbrdHeight = subbrdWidth * (g_fboardHeight / g_fboardWidth);

  // change sizes
  for (unsigned x = 0; x < g_boardWidth; x++) {
    for (unsigned y = 0; y < g_boardHeight; y++) {

      m_subboards[x][y].SetPos({
        m_spacing / 2 + x * (subbrdWidth + m_spacing),
        m_spacing / 2 + y * (subbrdHeight + m_spacing)
      });

      m_subboards[x][y].SetDim({
        subbrdWidth,
        subbrdHeight
      });
      m_subboards[x][y].setGamePtr(this);
    }
  }
}
Game::Game(std::shared_ptr<sf::RenderWindow> gameWin, vector<shared_ptr<sf::Texture>> textures, const settings_t gameSettings, shared_ptr<Server> server)
  : m_window(gameWin), m_tileTextures(textures),m_settings(gameSettings), m_server(server), m_hist()
{
  if (server->Connected()) {
    m_settings = server->GetSettings();
  }

  m_subboards.resize(m_settings.board.width);

  for (auto &col : m_subboards) {
    col.resize(m_settings.board.height);
  }

  //m_window.create({ 600, 600 }, "TicTacToe");
  m_window->setSize({ 600,600 });
  Resize(600, 600);

  m_btnUndo.SetPosition(12, 0);
  m_btnUndo.SetFont(g_tahoma);
  m_btnUndo.SetText("Undo");
  m_uiButtons.push_back(&m_btnUndo);

  m_btnBack.SetPosition(540, 0);
  m_btnBack.SetFont(g_tahoma);
  m_btnBack.SetText("Exit Game");
  m_uiButtons.push_back(&m_btnBack);

  m_txtConfirm.SetPosition(540, 0);
  m_txtConfirm.SetFont(g_tahoma);
  m_txtConfirm.SetText("Are You Sure?");
  m_txtConfirm.SetActive(false);
  m_uiButtons.push_back(&m_txtConfirm);

  m_btnYes.SetPosition(540, 0);
  m_btnYes.SetFont(g_tahoma);
  m_btnYes.SetText("(Y");
  m_btnYes.SetActive(false);
  m_uiButtons.push_back(&m_btnYes);


  m_btnNo.SetPosition(540, 0);
  m_btnNo.SetFont(g_tahoma);
  m_btnNo.SetText("/ N)");
  m_btnNo.SetActive(false);
  m_uiButtons.push_back(&m_btnNo);

  SetActiveBoard({ 0,0 });
  m_turn = CROSS;
}


Game::~Game()
{
}
void Game::SetActiveBoard(Point boardPos) {
  m_activeBoard = boardPos;
}

Subboard * Game::GetActiveBoard()
{
  return &m_subboards[m_activeBoard.x][m_activeBoard.y];
}

Subboard * Game::GetHoverBoard()
{
  sf::Vector2f mousePos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window), m_gameView);
  //sf::Vector2f(sf::Mouse::getPosition(m_window));

  for (auto &row : m_subboards) {
    for (auto &board : row) {
      sf::FloatRect rcBoard;
      rcBoard = {
        board.GetPos().x, board.GetPos().y,
        board.GetDim().x, board.GetDim().y,
      };
      if (rcBoard.contains(mousePos)) {
        m_hoverBoard = &board;
        return &board;
      }
    }
  }
  return nullptr;
}

void Game::Restart()
{

}

void Game::HandleInput()
{
  sf::Event event;
  sf::Vector2f pos = { -1,-1 };

  pos.x = float(sf::Mouse::getPosition(*m_window).x);
  pos.y = float(sf::Mouse::getPosition(*m_window).y);



  while (m_window->pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      m_running = false;
      //m_score.setString("");
      m_window->close();
      Exit();
    }
    //m_view.setSize();
    if (event.type == sf::Event::Resized) {
      float w = event.size.width;
      float h = event.size.height;
      m_width  = w;
      m_height = h;


      // ratio of w/h
      sf::Vector2f scale = { 1,1 };

      // win height controls view width.
      if (w<h) {
        scale.x = (w/ h);
        scale.y = 1;
      }
      else if (h < w) {
        scale.x = 1; ;
        scale.y =(h / w);
      }
   
      //m_gameView.move(0.0f, -m_toolbarHeight);
      Resize(w, h);


      m_btnBack.SetPosition(m_width - 148, 0);
      m_txtConfirm.SetPosition(m_width - 300, 0);
      m_btnYes.SetPosition(m_width - 90, 0);
      m_btnNo.SetPosition(m_width - 55, 0);


      //Resize(m_gameView.getSize().x, m_gameView.getSize().y);
      //window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
    }
    if (event.type == sf::Event::MouseButtonPressed && m_victor == NONE) {
      sf::Vector2f pos = { -1,-1 };

      pos.x = float(sf::Mouse::getPosition(*m_window).x);
      pos.y = float(sf::Mouse::getPosition(*m_window).y);

      for (auto& btn : m_uiButtons) {
        btn->HandleInput(pos);
      }

	    if (m_isOnline) {
        if (0 == count_if(m_players.begin(), m_players.end(), [&](OnlinePlayer p) {return p.id == m_turn; })) {

        }
        else if (m_turn == m_server->GetAssignedTurn()) {
        }
        else return;
	    }
      m_mouseClick = true;

      bool success = false;

      for (unsigned x = 0; x < g_boardWidth; x++) {
        for (unsigned y = 0; y < g_boardHeight; y++) {

          sf::Vector2f center = { m_width / 2.0f, m_height / 2.0f };

          //pos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
          pos = m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window), m_gameView);

          success = m_subboards[x][y].HandleClicks(pos, m_turn);
          if (success) {
            m_hist.logPlay({ { x,y }, m_activeBoard, m_turn , m_freePlay });
            m_server->PlayMove(m_hist.GetLastPlay());
            //cout << char('a' + (y)) << char('1' + (x));
            //IncrementTurn();
            break;
          }
        }
        if (success) {
          break;
        }
      }
    }
  }
}

void Game::Update(sf::Time dt) {
  static bool firstTime = true;
  m_isOnline = m_server->Connected();

  // Update UI buttons.
  for (auto &btn: m_uiButtons) {
    btn->Update();
  }

  if (m_btnUndo.WasClicked()) {
    play_t lastMove = m_hist.GetLastPlay();

    m_subboards[lastMove.majorPos.x][lastMove.majorPos.y]
      .RemoveTile(lastMove.minorPos.x, lastMove.minorPos.y);

    SetActiveBoard(lastMove.majorPos);
    m_hist.RemoveLastPlay();
    DecrementTurn();
  }
  else if (m_btnBack.WasClicked()) {
    m_btnBack.SetActive(false);
    m_txtConfirm.SetActive(true);
    m_btnYes.SetActive(true);
    m_btnNo.SetActive(true);
    //m_exit = true;
  }
  else if (m_btnYes.WasClicked()) {
    Exit();
  }
  else if (m_btnNo.WasClicked()) {
    m_btnBack.SetActive(true);
    m_txtConfirm.SetActive(false);
    m_btnYes.SetActive(false);
    m_btnNo.SetActive(false);
  }

  if (m_isOnline) {
    if (m_server->MoveAvailable()) {
      play_t play = m_server->GetPlay();
      m_subboards[play.majorPos.x][play.majorPos.y]
        .Play(m_turn, play.minorPos.x, play.minorPos.y);
    }

    if (m_server->PlayerListAvailable() || firstTime) {
      m_btnPlayers.clear();

      m_players = m_server->GetPlayers();
      for (auto &player : m_players) {
        m_btnPlayers.push_back(GameButton(to_string(player.id) + ". " + player.name, g_tahoma));
        m_btnPlayers.back().SetPosition(135 + 155 * (player.id - 1), 0);
      }
    }
    for (auto player : m_players) {
      if (m_turn == player.id) {
        m_currPlayer = &player;
      }
    }
  }

  for (unsigned x = 0; x < g_boardWidth; x++) {
    for (unsigned y = 0; y < g_boardHeight; y++) {
      m_subboards[x][y].Update();
    }
  }
  // Win Condition

  // returns true if the three are equal.
  auto isWin = [](Subboard &a, Subboard &b, Subboard &c) {
    return (a.GetOwner() == b.GetOwner()) && (a.GetOwner() == c.GetOwner());
  };
  Subboard a, b, c;
  bool victory[4] = { 0 };

  // check for any vertical 3-in-a-row
  for (unsigned x = 0; x < g_boardWidth && !m_victor; x++) {
    a = m_subboards[x][0]; b = m_subboards[x][1]; c = m_subboards[x][2];
    m_victor = isWin(a, b, c) ? a.GetOwner() : NONE;
  }
  // check for any horizontal 3-in-a-row
  for (unsigned y = 0; y < g_boardHeight; y++) {
    for (unsigned x = 0; x < g_boardWidth - 2 && !m_victor; x++) {
      a = m_subboards[x][y]; b = m_subboards[x + 1][y]; c = m_subboards[x + 2][y];
      m_victor = isWin(a, b, c) ? a.GetOwner() : NONE;
    }
  }
  // diag top-left to botton-right 3-in-a-row
  for (unsigned x = 0; x < g_boardWidth - 2 && !m_victor; x++) {
    a = m_subboards[x][0]; b = m_subboards[x + 1][1]; c = m_subboards[x + 2][2];
    m_victor = isWin(a, b, c) ? a.GetOwner() : NONE;
  }
  // diag top-right to bottom-left win condition
  for (unsigned x = 2; x < g_boardWidth && !m_victor; x++) {
    a = m_subboards[x][0]; b = m_subboards[x - 1][1]; c = m_subboards[x - 2][2];
    m_victor = isWin(a, b, c) ? a.GetOwner() : NONE;
  }
  firstTime = false;
}

void Game::Draw()
{
  sf::Vector2f pos = GetActiveBoard()->GetPos();
  sf::Vector2f dim = GetActiveBoard()->GetDim();

  sf::RectangleShape sqrboi;
  sf::Color colorboi;

  //prep
  m_window->setView(m_window->getDefaultView());
  m_window->clear();

  //Draw tool bar items.
  m_window->setView(m_toolBarView);

  for (auto& btn : m_uiButtons) {
    btn->Draw(m_window.get());
  }
  for (auto name : m_btnPlayers) {
    sf::Color colors[] = { sf::Color::White, sf::Color(130,255,130), sf::Color(130,255,130), sf::Color(130,130,255) };
    sf::Color currTurnColors[] = { sf::Color::White, sf::Color::Green, sf::Color::Red, sf::Color::Blue };

    int id = name.GetText()[0] - '0';
    //if (id == m_server->GetAssignedTurn() && id >=0 && id <=3) {
    //  name.SetColor(colors[id]);
    //}
    name.SetColor(colors[id]);
    if (id == m_turn && id >= 0 && id <= 3) {
      name.SetColor(currTurnColors[id]);
    }

    name.Draw(m_window.get());
  }
  // draw game items
  m_window->setView(m_gameView);

  sqrboi.setPosition({ 0,0 });
  sqrboi.setSize({ float(m_gameView.getSize().x),float(m_gameView.getSize().y) });
  sqrboi.setFillColor(sf::Color::Transparent);
  sqrboi.setOutlineThickness(5);
  sqrboi.setOutlineColor(sf::Color::Yellow);
  //m_window.draw(sqrboi);
    // draw victory indicator
  if (m_victor != NONE) {
    sqrboi.setTexture(GetTileGraphic(m_victor));
    sqrboi.setPosition({ 0,0 });
    sqrboi.setSize({ float(m_width),float(m_height) });
    m_window->draw(sqrboi);
  }

    // draw each subboard.
  for (unsigned x = 0; x < g_boardWidth; x++) {
    for (unsigned y = 0; y < g_boardHeight; y++) {
      m_subboards[x][y].Draw(m_window.get());
    }
  }



    // Draw turn indicators
  switch (m_turn) {
    case CROSS:    colorboi = sf::Color::Green; break;
    case TRIANGLE: colorboi = sf::Color::Blue;  break;
    case CIRCLE:   colorboi = sf::Color::Red;   break;
  }

  sqrboi.setOutlineColor(colorboi);
  sqrboi.setOutlineThickness(5);

  if (!m_freePlay) {
    sqrboi.setPosition(pos - sf::Vector2f({ 10, 10 }));
    sqrboi.setSize(dim + sf::Vector2f({ 20, 20 }));
    m_window->draw(sqrboi);

    if (m_isOnline && m_currPlayer!= nullptr) {
      sf::RectangleShape nameTagBG;
      sf::Text nameTag;
      sqrboi.setFillColor(sf::Color::Transparent);
      nameTagBG.setFillColor(colorboi);
      nameTagBG.setOutlineColor(colorboi);
      nameTagBG.setPosition(pos);
      nameTagBG.setSize({ 40,14 });
      nameTag.setFont(*g_tahoma);
      if (m_currPlayer->name != "")
        nameTag.setString(m_currPlayer->name);
      nameTag.setScale({ 0.4f,0.4f });
      nameTag.setPosition(pos );
      m_window->draw(nameTagBG);
      m_window->draw(nameTag);
    }
  } else {
    //sqrboi.setPosition(m_subboards[0][0].GetPos().x,
    //                   m_subboards[0][0].GetPos().y);
    ////sqrboi.setPosition(,
    ////                   );
    //sqrboi.setSize(dim + sf::Vector2f({ 20, 20 }));
  }


    // draw hover indicator
  Subboard *hoverBoard = nullptr;
  if (!m_freePlay) {
    hoverBoard = GetActiveBoard();
  } else {
    hoverBoard = GetHoverBoard();
  }
  if (hoverBoard != nullptr) {
    pos = { float(hoverBoard->GetHoverPos().x), float(hoverBoard->GetHoverPos().y) };
    //pos = sf::Vector2f(m_window.mapCoordsToPixel(pos));
  }
  if (!(pos.x < 0 || pos.x > g_boardWidth - 1 || pos.y < 0 || pos.y > g_boardHeight - 1)) {
    static float t = 0;
    switch (m_nextTurn) {
      case CROSS:    colorboi = sf::Color::Green; break;
      case CIRCLE:   colorboi = sf::Color::Red;   break;
      case TRIANGLE: colorboi = sf::Color::Blue;  break;
    }

    sqrboi.setOutlineColor(colorboi);
    sqrboi.setOutlineThickness(1);

    pos = m_subboards[unsigned(pos.x)][unsigned(pos.y)].GetPos();
    pos -= sf::Vector2f({ 7, 7 });
    //sf::Vector2f drawpos = m_window.mapCoordsToPixel(pos);

    sqrboi.setPosition(pos);
    sqrboi.setSize(dim + sf::Vector2f({ 15, 15 }));

    m_window->draw(sqrboi);
  }

  //display
  m_window->display();
}

void Game::Exit()
{
  m_exit = true;
  if (m_isOnline && m_server->Connected()) {
    m_server->Disconnect();
  }
}

void Game::Play(Point mjrPos, Point mnrPos)
{
}

void Game::IncrementTurn()
{
  m_prevTurn = m_turn;
  m_turn = m_nextTurn;
  m_nextTurn = Tile(1 + (m_turn % g_numPlayers));
}

void Game::DecrementTurn()
{
  m_nextTurn = m_turn;
  m_turn = m_prevTurn;
  m_prevTurn = Tile((m_turn - 2 + g_numPlayers) % g_numPlayers + 1);
}

Tile Game::GetTurn()
{
  return m_turn;
}

sf::Texture* Game::GetTileGraphic(Tile tile)
{
  return m_tileTextures[tile].get();
}

void Game::AllowFreePlay(bool allow)
{
  m_freePlay = allow;
}

bool Game::IsFreePlayAllowed(void)
{
  return m_freePlay;
}

bool Game::OnlinePlayAllowed()
{
  //bool playAllowed = false;
  //if (m_server->GetCurrentTurn() == m_server->GetAssignedTurn()) {
  //  playAllowed = true;
  //}
  //return playAllowed;
  return true;
}

bool Game::IsRunning()
{
  return m_window->isOpen() && !m_exit;
}

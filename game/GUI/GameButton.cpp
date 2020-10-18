#include "GameButton.h"
#include <string>
using namespace std;
extern sf::Font* g_tahoma;

GameButton GameButton::m_dummyButton = GameButton();
GameButton* GameButton::s_activeBtn = nullptr;
GameButton* GameButton::s_hoverBtn = nullptr;

void GameButton::SetActiveButton(GameButton *btn)
{
  s_activeBtn = btn;
}
GameButton * GameButton::GetActiveButton(void)
{
  return s_activeBtn;
}
void GameButton::SetHoverButton(GameButton* btn)
{
  s_hoverBtn = btn;
}
GameButton* GameButton::GetHoverButton(void)
{
  return s_hoverBtn;
}
GameButton::GameButton(string title, sf::Font * pFont)
{
  m_text.setFont(*pFont);
  m_text.setString(title);

  m_pFont = pFont;
  m_pFont = g_tahoma;
  m_text.setFont(*m_pFont);
  m_text.setPosition({ 0,0 });
  m_text.setCharacterSize(24);
  m_isClicked = false;
  m_wasClicked = false;
  m_style = Gui::Style::NONE;
}

GameButton::GameButton(string title, sf::Vector2f pos, Gui::Style style, bool active)
{
  SetText(title);
  SetPosition(pos.x, pos.y);
  SetEnabled(active);

  m_rect = { 0, 0, 10, 10 };
  m_pFont = g_tahoma;
  m_text.setFont(*m_pFont);
  m_text.setCharacterSize(24);
  m_isClicked = false;
  m_wasClicked = false;
  SetStyle(style);
}

GameButton::GameButton()
{
  m_rect = { 0, 0, 10, 10 };
  m_pFont = g_tahoma;
  m_text.setFont(*m_pFont);
  m_isClicked = false;
  m_wasClicked = false;
}

void GameButton::SetPosition(float x, float y)
{
  m_rect.left = x;
  m_rect.top = y;
  m_text.setPosition({ x,y });
}

sf::Rect<float> GameButton::GetRect()
{
  return m_rect;
}

void GameButton::SetText(string txt)
{
  m_text.setString(txt);
}

string GameButton::GetText()
{
	return m_text.getString();
}

void GameButton::SetEnabled(bool enable)
{
	m_enabled = enable;
}

bool GameButton::IsEnabled()
{
  return m_enabled;
}

bool GameButton::IsSelected()
{
  return GetActiveButton() == this;
}

void GameButton::SetFont(sf::Font * pFont)
{
  m_pFont = pFont;
  m_text.setFont(*pFont);
}

void GameButton::SetHowBigItIs(unsigned bigness)
{
  m_text.setCharacterSize(bigness);
}

void GameButton::SetColor(sf::Color color)
{
  m_textFillColor = color;
  m_text.setFillColor(color);
}
void GameButton::SetStyle(Gui::Style style)
{
  m_style = style;
}
void GameButton::AddStyle(Gui::Style style)
{
  m_style = Gui::Style((uint16_t)m_style | (uint16_t)style);
}
void GameButton::RemoveStyle(Gui::Style style)
{
  m_style = Gui::Style((uint16_t)m_style & ~(uint16_t)style);
}
bool GameButton::HasStyle(Gui::Style style)
{
  return ((uint16_t)m_style & (uint16_t)style) != 0;
}
#include <iostream>
bool GameButton::WasClicked(bool reset)
{
	if (!m_enabled)
		return false;
  bool ret = m_wasClicked;
  //std::cout << "btn" <<  << " was clicked:" << ret;
  if (reset) {
    m_isClicked = false;
    m_wasClicked = false;
  }
  return ret;
}

void GameButton::Update()
{
  if (!m_enabled) return;
  m_wasClicked = m_isClicked || m_wasClicked;


}
void GameButton::HandleInput(sf::Event event, sf::Vector2f mousePos) {

  if (!m_enabled) return;
  bool hover = m_rect.contains(mousePos);
  m_isClicked = hover && (event.type == sf::Event::MouseButtonPressed);
  if (hover) {
    s_hoverBtn = this;
  }
  if (m_isClicked) {
    SetActiveButton(this);
  }
}
void GameButton::Draw(sf::RenderWindow *pWin) {
  static bool firstTime = true;
  sf::Text displayText;

  sf::RectangleShape underLine;
  sf::RectangleShape border;

  if (firstTime) {
    pWin->draw(m_text);
    firstTime = false;
  }

  if (!pWin) return;
  if (!m_pFont) return;
  if (!m_enabled) return;
    
  // Define bounding rect
  m_rect.left = m_text.getPosition().x;
  m_rect.top  = m_text.getPosition().y;
  m_text.setPosition({ m_rect.left,m_rect.top });
  m_rect.width  = m_text.getLocalBounds().width;
  m_rect.height = m_text.getCharacterSize();

  if (m_forceRect.width != 0) {
    m_rect.width = m_forceRect.width;
  }

  unsigned index = 0;
  while (index < m_text.getString().getSize()) {
    if (m_text.findCharacterPos(index++).x > m_rect.left + m_rect.width) {
      break;
    }
  }

  if (this->HasStyle(Gui::Style::UNDERLINE)) {
    underLine.setSize({ m_rect.width, 1 });
    underLine.setPosition({ m_rect.left, m_rect.top + m_rect.height + 4 });
    pWin->draw(underLine);
  }
  // Draw border
  if (this->HasStyle(Gui::Style::BORDER) || this->HasStyle(Gui::Style::SEL_BORDER)) {
    border.setPosition({ m_rect.left - 3,m_rect.top - 3 });
    border.setSize({ float(m_rect.width + 6),float(m_rect.height + 6) });
    border.setFillColor(sf::Color(0, 0, 0, 0));
    border.setOutlineThickness(2);

    if (this->HasStyle(Gui::Style::BORDER)) {
      border.setOutlineColor(m_borderColor);
      pWin->draw(border);
    }
    if (this->HasStyle(Gui::Style::SEL_BORDER) && this->IsSelected()) {
      border.setOutlineColor(m_activeBorderColor);
      pWin->draw(border);
    }
  }
  displayText = m_text;
  if (this == s_hoverBtn) {
    cout << this->GetText() << " Is hovered" << endl;
    displayText.setOutlineColor(sf::Color(0,0,255));
    displayText.setOutlineThickness(1);
    displayText.setScale(1.08,1.08);
  }
  
  displayText.setFont(*m_pFont);
  displayText.setString(m_text.getString().toAnsiString().substr(0, index));

  pWin->draw(displayText);
}
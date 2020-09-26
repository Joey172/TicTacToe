#include "GameButton.h"
#include <string>
using namespace std;
extern sf::Font* g_tahoma;

GameButton GameButton::m_dummyButton = GameButton();
GameButton* GameButton::m_activeBtn = nullptr;

void GameButton::SetActiveButton(GameButton *btn)
{
  m_activeBtn = btn;
}
GameButton * GameButton::GetActiveButton(void)
{
  return m_activeBtn;
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

}

GameButton::GameButton(string title, sf::Vector2f pos, bool active)
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

bool GameButton::IsActive()
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

  m_isClicked = m_rect.contains(mousePos) && (event.type == sf::Event::MouseButtonPressed);
  if (m_isClicked) {
    SetActiveButton(this);
  }
}
void GameButton::Draw(sf::RenderWindow *pWin) {
  static bool firstTime = true;
  if (firstTime) {
    pWin->draw(m_text);
    firstTime = false;
  }
  if (!pWin) return;
  if (!m_pFont) return;
  if (!m_enabled) return;
    

  m_rect.left = m_text.getPosition().x;
  m_rect.top = m_text.getPosition().y;
  m_text.setPosition({ m_rect.left,m_rect.top });
  m_rect.width = m_text.getLocalBounds().width;
  m_rect.height = m_text.getLocalBounds().height;

  if (m_forceRect.width != 0) {
    m_rect.width = m_forceRect.width;

  }
  
  m_rect.height = m_text.getCharacterSize();

  sf::Text displayText = m_text;
  unsigned index = 0;
  while (index < m_text.getString().getSize()) {
    if (m_text.findCharacterPos(index++).x > m_rect.left + m_rect.width) {
      break;
    }
  }
  displayText.setFont(*m_pFont);
  displayText.setString(m_text.getString().toAnsiString().substr(0, index));
  //dummyText.setString();
  
  //m_rect.width = dummyText.getLocalBounds().width;
  //m_rect.width = m_text.getString().getSize() * m_text.getCharacterSize() *.5;
  pWin->draw(displayText);

  sf::RectangleShape sqrboi;
  sqrboi.setPosition({ m_rect.left - 3,m_rect.top - 3 });
  sqrboi.setSize({ float(m_rect.width + 6),float(m_rect.height + 6) });
  sqrboi.setOutlineColor(IsActive() ? m_activeBorderColor : m_borderColor);
  sqrboi.setFillColor(sf::Color(0, 0, 0, 0));
  sqrboi.setOutlineThickness(2);
  pWin->draw(sqrboi);
}
#include "GameButton.h"
extern sf::Font* g_tahoma;

GameButton* GameButton::m_activeBtn = nullptr;
void GameButton::SetActiveButton(GameButton *btn)
{
  m_activeBtn = btn;
}
GameButton * GameButton::GetActiveButton(void)
{
  return m_activeBtn;
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

void GameButton::SetActive(bool active)
{
	m_active = active;
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
  m_color = color;
  m_text.setFillColor(color);
}
#include <iostream>
bool GameButton::WasClicked()
{
	if (!m_active)
		return false;
  bool ret = m_wasClicked;
  //std::cout << "btn" <<  << " was clicked:" << ret;
  m_isClicked = false;
  m_wasClicked = false;
  return ret;
}

void GameButton::Update()
{
  if (!m_active) return;
  m_wasClicked = m_isClicked || m_wasClicked;

  if (m_wasClicked) {
    SetActiveButton(this);
  }
}
void GameButton::HandleInput(sf::Vector2f mousePos) {
  if (!m_active) return;
  m_isClicked = m_rect.contains(mousePos);
}
void GameButton::Draw(sf::RenderWindow *pWin) {
  if (!pWin) return;
  if (!m_pFont) return;
  if (!m_active) return;
  
  if (this == GetActiveButton()) {
    sf::RectangleShape sqrboi;
    sqrboi.setPosition({ m_rect.left-3,m_rect.top-3 });
    sqrboi.setSize({ float(m_rect.width+6),float(m_rect.height+6) });
    sqrboi.setOutlineColor(sf::Color(255,255,255,255));
    sqrboi.setFillColor(sf::Color(0, 0, 0, 255));
    sqrboi.setOutlineThickness(2);
    pWin->draw(sqrboi);

  }
  
  pWin->draw(m_text);

  m_rect.left = m_text.getPosition().x;
  m_rect.top = m_text.getPosition().y;
  m_text.setPosition({ m_rect.left,m_rect.top });
  m_rect.width = m_text.getLocalBounds().width;
  m_rect.height = m_text.getLocalBounds().height;
}
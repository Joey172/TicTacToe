#include "EditBox.h"
#include <iostream>
using namespace std;

TextInput::TextInput(std::string title, sf::Vector2f pos, float width, bool enabled) :
  GameButton(title, pos, enabled)
{
  m_width = width;
  m_forceRect.width = width;
  m_borderColor = sf::Color(100, 100, 100, 255);
}


void TextInput::HandleInput(sf::Event event, sf::Vector2f mousePos ) {
  GameButton::HandleInput(event, mousePos);

  if (this != GetActiveButton())
    return;

  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Key::BackSpace && !GetText().empty()) {
      this->SetText(this->GetText().erase(this->GetText().size() - 1));
    }
    else if (event.key.code == sf::Keyboard::Key::Return) {
      //m_ready = true;
      SetActiveButton(&m_dummyButton);
    }
  }
  else if (event.type == sf::Event::TextEntered) {
    char c = static_cast<char>(event.text.unicode);
    cout << c << endl;
    if (c != 8 && c != '\n' && c != '\r') {
      this->SetText(this->GetText() + c);
    }
  }
}
void TextInput::Update() {
  GameButton::Update();
}
void TextInput::Draw(sf::RenderWindow* pWin) {
  static sf::Clock timer;
  static sf::Time dt;
  static sf::RectangleShape cursor;
  GameButton::Draw(pWin);

  if (timer.getElapsedTime().asSeconds() < 0.5) {
    if (this->IsActive()) {
      cursor.setPosition({ float(m_text.findCharacterPos(999).x) + 5,m_rect.top + 3 });
      cursor.setSize({ 1.5f, float(m_rect.height - 6) });
      cursor.setOutlineColor(sf::Color::White);
      cursor.setFillColor(sf::Color::White);
      cursor.setOutlineThickness(2);

      pWin->draw(cursor);
    }
  }
  else if (timer.getElapsedTime().asSeconds() > 1) {
    timer.restart();
  }
}

bool TextInput::IsReady(bool reset) {
  //bool ret = m_ready;
  bool ret = !GetText().empty() && !IsActive();
  cout << "Ready? " << ret << endl;
  m_ready = ret;
  if (reset && ret) {
    m_ready = false;
  }
  return ret;
}
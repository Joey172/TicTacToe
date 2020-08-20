#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>
using namespace std;
class GameButton {

  sf::Text m_text;
  sf::Font *m_pFont;
  sf::Color m_color = sf::Color::White;
  sf::Rect<float> m_rect = { 0,0,100,100 };
  bool m_isClicked = false;
  bool m_wasClicked = false;
  bool m_active = true;
  static GameButton* m_activeBtn;
  //function<void(void)> *onClick;
public:
  static void SetActiveButton(GameButton*);
  static GameButton* GetActiveButton(void);

  static void SetHoverButton(GameButton*);
  static GameButton* GetHoverButton(void);

  GameButton(string title, sf::Font *pFont);
  GameButton();
  void HandleInput(sf::Vector2f mousePos);
  void SetPosition(float x, float y);
  sf::Rect<float> GetRect();
  void SetText(string txt);
  string GetText();
  void SetActive(bool);
  void SetFont(sf::Font *pFont);
  void SetHowBigItIs(unsigned bigness);
  void SetColor(sf::Color color);
  void Update();
  void Draw(sf::RenderWindow *pWin);
  bool WasClicked();
};


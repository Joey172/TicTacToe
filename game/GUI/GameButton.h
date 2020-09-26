#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

class GameButton {
protected:
  static GameButton m_dummyButton;
  sf::Text m_text;
  sf::Font *m_pFont;
  sf::Color m_textFillColor = sf::Color::White;
  sf::Color m_borderColor = sf::Color::Transparent;
  sf::Color m_activeBorderColor = sf::Color::White;
  sf::Rect<float> m_rect = { 0,0,100,100 };
  sf::Rect<float> m_forceRect = { 0,0,0,0 };
  bool m_isClicked = false;
  bool m_wasClicked = false;
  bool m_enabled = true;
  static GameButton* m_activeBtn;
  
  //function<void(void)> *onClick;
public:
  static void SetActiveButton(GameButton*);
  static GameButton* GetActiveButton(void);

  static void SetHoverButton(GameButton*);
  static GameButton* GetHoverButton(void);

  GameButton(std::string title, sf::Font *pFont);
  GameButton(std::string title, sf::Vector2f pos, bool enabled = true);
  GameButton();

  virtual void HandleInput(sf::Event event, sf::Vector2f mousePos = {0,0});
  virtual void Update();
  virtual void Draw(sf::RenderWindow* pWin);

  void SetPosition(float x, float y);
  sf::Rect<float> GetRect();
  void SetText(std::string txt);
  std::string GetText();
  void SetEnabled(bool);
  bool IsEnabled();
  bool IsActive();
  void SetFont(sf::Font *pFont);
  void SetHowBigItIs(unsigned bigness);
  void SetColor(sf::Color color);


  // Returns if the button has been clicked since the last call to the function
  // if @reset=false, a subsequent call will return true
  bool WasClicked(bool reset = true);
};


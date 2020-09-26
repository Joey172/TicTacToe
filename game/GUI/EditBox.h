#include "GameButton.h"

class TextInput : public GameButton {
  bool m_ready = false;
  float m_width = 0;
public:
  TextInput(std::string title, sf::Vector2f pos, float width, bool enabled = true);
  virtual void HandleInput(sf::Event event, sf::Vector2f mousePos = { 0,0 }) override;
  virtual void Update() override;
  virtual void Draw(sf::RenderWindow* pWin) override;

  // returns true after the user hits enter.
  bool IsReady(bool reset = true);
};
//#include "inih.h"
#include "GameSettings.h"
#include <SFML/Graphics.hpp>
#include "inih.h"
#include <iostream>
using namespace std;
sf::Color StrToColor(std::string hexStr) {
	return sf::Color(std::stoul(hexStr, nullptr, 16));
}

void loadAllSettings() {
  INIReader configReader("./config.ini");
  if (configReader.ParseError() < 0) {
    std::cout << "Can't load 'test.ini'\n";
  }

  for (auto str : configReader.Sections()) {
    cout << str << endl;
  }
  cout << endl << "bgColor: " << stoul(configReader.Get("style", "bgColor", "not_found"), nullptr, 16);
}
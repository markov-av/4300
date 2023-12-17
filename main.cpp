#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/Event.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <tuple>

class WindowConfig {
public:
  int height = 0;
  int width = 0;

  WindowConfig() {}
  WindowConfig(int height, int width) : height(height), width(width) {}
};

class Rectangle {
public:
  int height = 0;
  int width = 0;
  std::string name = "";
  std::tuple<float, float> position;
  std::tuple<float, float> velocity;
  std::tuple<int, int, int> rgb;
  std::tuple<float, float> size;

  Rectangle() {}
  Rectangle(int height, int width, std::string name,
            std::tuple<float, float> position,
            std::tuple<float, float> velocity, std::tuple<int, int, int> rgb,
            std::tuple<float, float> size)
      : height(height), width(width), name(name), position(position),
        velocity(velocity), rgb(rgb), size(size) {}
};

std::tuple<WindowConfig, Rectangle>
read_config_file(const std::string filename) {
  std::fstream fin(filename);
  std::string temp;

  WindowConfig window;
  Rectangle rectangle;

  while (fin >> temp) {
    if (temp == "Window") {
      fin >> window.height;
      fin >> window.width;
      continue;
    }
    if (temp == "Rectangle") {
      fin >> rectangle.name;
      float x, y, vx, vy;
      fin >> x, fin >> y, fin >> vx, fin >> vy;
      rectangle.position = {x, y};
      rectangle.velocity = {vx, vy};
      int r, g, b;
      fin >> r, fin >> g, fin >> b;
      rectangle.rgb = {r, g, b};
      fin >> x, fin >> y;
      rectangle.size = {x, y};
    }
  }
  fin.close();
  return std::tuple(window, rectangle);
}

int main(int argc, char *argv[]) {
  const std::string config_file_path = "config.txt";
  auto data = read_config_file(config_file_path);

  WindowConfig window_config = std::get<0>(data);
  Rectangle rectangle_config = std::get<1>(data);

  sf::RenderWindow window(
      sf::VideoMode(window_config.height, window_config.width), "SFML window");
  window.setFramerateLimit(60);

  sf::RectangleShape rect(
      std::make_from_tuple<sf::Vector2f>(std::move(rectangle_config.size)));
  rect.setFillColor(
      std::make_from_tuple<sf::Color>(std::move(rectangle_config.rgb)));

  float Xv = 1.f, Vy = 0.5f;

  while (window.isOpen()) {
    int WINDOW_W = window.getSize().x;
    int WINDOW_H = window.getSize().y;

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear();
    // auto vec = std::make_from_tuple<sf::Vector2f>(
    //     std::move(rectangle_config.velocity));
    auto current_pos = rect.getPosition();
    if ((current_pos.x - 1 <= 0) || (current_pos.y - 1 <= 0)) {
      Xv = 1.f;
      Vy = 0.5f;
    }
    if ((current_pos.x + rect.getSize().x >= WINDOW_W) ||
        (current_pos.y + rect.getSize().y >= WINDOW_H)) {
      Xv = -1.f;
      Vy = -0.5f;
    }
    // std::cout << current_pos.x << " " << current_pos.y << std::endl;

    rect.move(Xv, Vy);
    sf::Vector2f pos = rect.getPosition();
    window.draw(rect);
    window.display();
  }

  return 0;
}

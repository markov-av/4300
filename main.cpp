#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/Event.hpp"
#include <fstream>
#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <tuple>
#include <vector>

class WindowConfig {
public:
  int height = 0;
  int width = 0;

  WindowConfig() {}
  WindowConfig(int height, int width) : height(height), width(width) {}
};

class RectangleParams {
public:
  int height = 0;
  int width = 0;
  std::string name = "";
  std::tuple<float, float> position;
  std::tuple<float, float> velocity;
  std::tuple<int, int, int> rgb;
  std::tuple<float, float> size;

  RectangleParams() {}
  RectangleParams(int height, int width, std::string name,
                  std::tuple<float, float> position,
                  std::tuple<float, float> velocity,
                  std::tuple<int, int, int> rgb, std::tuple<float, float> size)
      : height(height), width(width), name(name), position(position),
        velocity(velocity), rgb(rgb), size(size) {}
};

void read_config_file(const std::string filename, WindowConfig &window,
                      std::vector<RectangleParams> &rects) {
  std::fstream fin(filename);
  std::string temp;

  while (fin >> temp) {
    if (temp == "Window") {
      fin >> window.height;
      fin >> window.width;
      continue;
    }
    if (temp == "Rectangle") {
      RectangleParams rectangle;
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
      rects.push_back(rectangle);
    }
  }
  fin.close();
}

int main(int argc, char *argv[]) {
  const std::string config_file_path = "config.txt";
  WindowConfig window_conf;
  std::vector<RectangleParams> rects_conf;
  read_config_file(config_file_path, window_conf, rects_conf);

  sf::RenderWindow window(sf::VideoMode(window_conf.height, window_conf.width),
                          "SFML window");
  window.setFramerateLimit(60);

  std::vector<sf::RectangleShape> rects;
  std::vector<std::tuple<float, float>> velocities;

  for (auto &elem : rects_conf) {
    sf::RectangleShape shape(
        std::make_from_tuple<sf::Vector2f>(std::move(elem.size)));
    shape.setFillColor(std::make_from_tuple<sf::Color>(std::move(elem.rgb)));
    rects.push_back(shape);
    velocities.push_back(elem.velocity);
  }

  const int WINDOW_W = window.getSize().x;
  const int WINDOW_H = window.getSize().y;

  while (window.isOpen()) {

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear();

    for (size_t i = 0; i < rects.size(); i++) {
      auto &shape = rects[i];
      auto current_pos = shape.getPosition();
      auto velocity = velocities[i];
      float vx = std::get<0>(velocity), vy = std::get<1>(velocity);
      if (current_pos.x <= 0 && vx < 0) {
        vx = -vx;
      } else if (current_pos.x + shape.getSize().x >= WINDOW_W && vx > 0) {
        vx = -vx;
      }

      if (current_pos.y <= 0 && vy < 0) {
        vy = -vy;
      } else if (current_pos.y + shape.getSize().y >= WINDOW_H && vy > 0) {
        vy = -vy;
      }

      shape.move(vx, vy);
      window.draw(shape);
      velocities[i] = std::tuple(vx, vy);
    }
    window.display();
  }

  return 0;
}

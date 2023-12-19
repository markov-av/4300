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
  ~WindowConfig();
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
  ~RectangleParams();
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

  for (auto &elem : rects_conf) {
    sf::RectangleShape shape(
        std::make_from_tuple<sf::Vector2f>(std::move(elem.size)));
    shape.setFillColor(std::make_from_tuple<sf::Color>(std::move(elem.rgb)));
    rects.push_back(shape);
  }

  while (window.isOpen()) {
    const int WINDOW_W = window.getSize().x;
    const int WINDOW_H = window.getSize().y;

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear();

    for (auto &rect : rects) {
      float Xv = 1.f, Vy = 0.5f;
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

      rect.move(Xv, Vy);
      sf::Vector2f pos = rect.getPosition();
      window.draw(rect);
    }

    window.display();
  }

  return 0;
}

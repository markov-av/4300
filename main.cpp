#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

class WindowConfig {
public:
  int height = 0;
  int width = 0;

  WindowConfig() {}
  WindowConfig(int height, int width) : height(height), width(width) {}
};

class Entity {
public:
  std::string type = "";
  std::unique_ptr<sf::Shape> shape;
  std::tuple<float, float> velocity;

  Entity() {}
  Entity(std::string type, std::tuple<float, float> velocity,
         std::unique_ptr<sf::Shape> shape)
      : type(type), shape(std::move(shape)), velocity(velocity) {}
};

void read_config_file(const std::string filename, WindowConfig &window,
                      std::vector<Entity> &entities) {
  std::fstream fin(filename);
  std::string temp;

  while (fin >> temp) {
    if (temp == "Window") {
      fin >> window.height;
      fin >> window.width;
      continue;
    }
    if (temp == "Rectangle") {
      Entity entity;
      std::unique_ptr<sf::RectangleShape> shape =
          std::make_unique<sf::RectangleShape>();
      fin >> entity.type;
      float x, y, vx, vy;
      int r, g, b;

      fin >> x, fin >> y, fin >> vx, fin >> vy;
      shape->setPosition(x, y);
      entity.velocity = {vx, vy};

      fin >> r, fin >> g, fin >> b;
      shape->setFillColor(sf::Color(r, g, b));

      fin >> x, fin >> y;
      shape->setSize(sf::Vector2f(x, y));

      entity.shape = std::move(shape);
      entities.push_back(std::move(entity));
    }
    if (temp == "Circle") {
      Entity entity;
      std::unique_ptr<sf::CircleShape> shape =
          std::make_unique<sf::CircleShape>();
      fin >> entity.type;

      float x, y, vx, vy, radius;
      int r, g, b;

      fin >> x, fin >> y, fin >> vx, fin >> vy;
      shape->setPosition(x, y);
      entity.velocity = {vx, vy};

      fin >> r, fin >> g, fin >> b;
      shape->setFillColor(sf::Color(r, g, b));

      fin >> radius;
      shape->setRadius(radius);

      entity.shape = std::move(shape);
      entities.push_back(std::move(entity));
    }
  }
  fin.close();
}

int main(int argc, char *argv[]) {
  const std::string config_file_path = "config.txt";
  WindowConfig window_conf;
  std::vector<Entity> entities;
  read_config_file(config_file_path, window_conf, entities);

  sf::RenderWindow window(sf::VideoMode(window_conf.height, window_conf.width),
                          "SFML window");
  window.setFramerateLimit(60);

  const int WINDOW_W = window.getSize().x;
  const int WINDOW_H = window.getSize().y;

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear();

    for (size_t i = 0; i < entities.size(); i++) {
      auto &entity = entities[i];
      auto *shape = entity.shape.get();
      auto current_pos = shape->getPosition();
      auto &velocity = entity.velocity;
      float vx = std::get<0>(velocity), vy = std::get<1>(velocity);

      sf::FloatRect bounds = shape->getGlobalBounds();
      float shapeWidth = bounds.width;
      float shapeHeight = bounds.height;

      if (bounds.left <= 0 && vx < 0) {
        vx *= -1;
      } else if (bounds.left + shapeWidth >= WINDOW_W && vx > 0) {
        vx *= -1;
      }

      if (bounds.top <= 0 && vy < 0) {
        vy *= -1;
      } else if (bounds.top + shapeHeight >= WINDOW_H && vy > 0) {
        vy *= -1;
      }
      entity.velocity = {vx, vy};
    }

    for (size_t i = 0; i < entities.size(); i++) {
      auto &entity = entities[i];
      auto *shape = entity.shape.get();
      auto &velocity = entity.velocity;

      float vx = std::get<0>(velocity), vy = std::get<1>(velocity);

      shape->move(vx, vy);
      window.draw(*shape);
    }

    window.display();
  }

  return 0;
}

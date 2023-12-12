#include <cstdio>
#include <fstream>
#include <string>

class Window {
public:
  int height = 0;
  int width = 0;

  Window() {}
  Window(int height, int width) : height(height), width(width) {}
};

class Rectangle {
public:
  int height = 0;
  int width = 0;
  std::string name = "";
  std::tuple<float, float> position;
  std::tuple<float, float> velocity;
  std::tuple<int, int, int> rgb;

  Rectangle() {}
  Rectangle(int height, int width, std::string name,
            std::tuple<float, float> position,
            std::tuple<float, float> velocity, std::tuple<int, int, int> rgb)
      : height(height), width(width), name(name), position(position),
        velocity(velocity), rgb(rgb) {}
};

void read_config_file(const std::string filename) {
  std::fstream fin(filename);
  std::string temp;
  while (fin >> temp) {
    if (temp == "Window") {
      Window window;
      fin >> window.height;
      fin >> window.width;
      continue;
    }
    if (temp == "Rectangle") {
      Rectangle rectangle;
      fin >> rectangle.name;
    }
  }
}

int main(int argc, char *argv[]) {
  const std::string config_file_path = "config.txt";
  std::fstream fin file;
  file.open(config_file_path);

  std::string line;

  if (file.is_open()) {
    while (getline(file, line)) {
    }
    file.close();
  }

  return 0;
}

#include "Flock.h"
#include "Steering.h"
#include <cmath>

int main() {
    sf::RenderWindow window{sf::VideoMode{800, 800}, "Game"};
    window.setFramerateLimit(30);
    Flock f{100, 800, 800};
    f.max_speed = 200.f;
    f.max_force = 800.f;
    f.put_on_rectangle(800, 800, 10, 10);

    Steering seek{Steering::Behavior::Arrival, {.arrival={.target={400., 400.}, .range=200.}}, 20.};
    bool running{true};
    sf::Clock c{};
    auto date = c.getElapsedTime();
    while (running) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            switch (event.type) {

//      case sf::Event::MouseButtonPressed: {
//        if (event.mouseButton.button == sf::Mouse::Left) {
//          sub_seek.met_cible(
//              sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
//        }
//      }
                case sf::Event::KeyPressed:
                    switch (event.key.code) {

                        case sf::Keyboard::Q:
                            running = false;
                            break;

                        default:
                            break;
                    }
                    break;

                case sf::Event::EventType::Closed:
                    running = false;
                    break;
                default:
                    break;
            }
        }
        window.clear(sf::Color::White);

        auto tmp = c.getElapsedTime();
        seek.compute(f);
        f.update(tmp - date);
        date = tmp;
        f.draw(window);
        window.display();
    }

    window.close();

    return 0;
}

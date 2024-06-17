#include "Behavior/AtomicBehavior.h"
#include "Behavior/CombinedBehavior.h"
#include "Creatures/Flock.h"
#include "World/FiniteWorld.h"
#include "interface.h"
#include <SFML/System/Clock.hpp>
#include <cmath>
#include <iostream>
long long int fps_somme = 0;
long long int fps_compte = 0;
int main() {
    sf::Texture texture;
    texture.loadFromFile("./resources/bg.jpg");
    sf::Sprite background(texture);
    sf::RenderWindow window{sf::VideoMode{800, 800}, "Game"};
    sf::View vue = window.getDefaultView();
    window.setFramerateLimit(30);
    // Ihm test{&window};
    FiniteWorld w{800.f, 800.f};
    w.flocks.emplace_back(Animal::Bird, 20.f, 1000);
    Flock &f1{w.flocks[0]};
    f1.max_speed = 200.f;
    f1.max_force = 800.f;
    f1.put_on_rectangle(400, 400, 25, 40);

    CombinedBehavior cb{};
    cb.add(AtomicBehavior::Type::Cohesion,
           {.cas = {.range = 25.f, .cos_fov = .5f}}, 15.f);
    cb.add(AtomicBehavior::Type::Alignment,
           {.cas = {.range = 30.f, .cos_fov = .8f}}, 15.f);
    cb.add(AtomicBehavior::Type::Separation,
           {.cas = {.range = 50.f, .cos_fov = 0.f}}, 50.f);
    cb.add(AtomicBehavior::Type::Wander,
           {.wander = {.sphere_dist = 100.f,
                       .sphere_radius = 90.f,
                       .displacement_amplitude = .5f}},
           30.f);

    AtomicBehavior steer{AtomicBehavior::Type::Arrival,
                         {.arrival = {.target = {400., 400.}, .range = 200.}},
                         20.};
    sf::Clock c{};
    auto dt = c.restart();
    //-_-_-_-__-_-_-_-_-_-_-__-_-_-_-Etats souris
    bool selection{false};
    bool translation{false};
    sf::Vector2f position_précédente_souris_monde;
    //-_-_-_-_-_-_-_-_-_-_-_-_-_-_
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            // test.évènements_ihm(event);
            switch (event.type) {
            case sf::Event::MouseButtonPressed:
                switch (event.mouseButton.button) {
                case sf::Mouse::Left:
                    // selection
                    if (!translation && !selection) {
                        selection = true;
                    }
                    break;
                case sf::Mouse::Middle:
                    // translation
                    if (!translation && !selection) {
                        position_précédente_souris_monde =
                            window.mapPixelToCoords(
                                sf::Mouse::getPosition(window));
                        translation = true;
                    }
                    break;
                default:
                    break;
                }
                break;
            case sf::Event::MouseButtonReleased:
                switch (event.mouseButton.button) {
                case sf::Mouse::Left:
                    // selection
                    if (!translation && selection) {
                        selection = false;
                    }
                    break;
                case sf::Mouse::Middle:
                    // fin de la translation
                    if (translation && !selection) {
                        translation = false;
                    }
                    break;
                default:
                    break;
                }
            case sf::Event::MouseMoved:

                if (translation) {
                    const sf::Vector2f position_nouvelle_souris_monde =
                        window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    vue.move(-position_nouvelle_souris_monde +
                             position_précédente_souris_monde);
                    window.setView(vue);
                    position_précédente_souris_monde =
                        window.mapPixelToCoords(sf::Mouse::getPosition(window));
                }
                break;
            case sf::Event::MouseWheelScrolled: {
                const sf::Vector2f pos_ini_vue = vue.getCenter();
                vue.setCenter(
                    window.mapPixelToCoords(sf::Mouse::getPosition(window)));
                vue.zoom(1.f + 0.1f * event.mouseWheelScroll.delta);
                vue.setCenter(pos_ini_vue);
                window.setView(vue);
                break;
            }

            case sf::Event::Resized: {
                sf::FloatRect zoneVisible(
                    0, 0, static_cast<float>(event.size.width),
                    static_cast<float>(event.size.height));
                vue = sf::View(zoneVisible);
                window.setView(vue);
                break;
            }
            case sf::Event::KeyPressed:
                switch (event.key.code) {

                case sf::Keyboard::Q:
                    window.close();
                    break;

                default:
                    break;
                }
                break;

            case sf::Event::EventType::Closed:
                window.close();
                break;
            default:
                break;
            }
        }
        window.clear(sf::Color::White);
        dt = c.restart();
        cb.compute(f1, &w);
        w.update(dt);
        //-_-_-_-_-__-_-_-_-_-_-_-_--_-_DESSSIN
        window.draw(background);
        f1.draw(window);
        // test.affichage_ihm(&w);
        window.display();
        float fps = 1E6 / dt.asMicroseconds();
        fps_somme += fps;
        fps_compte++;
        std::cout << "\x1B[31m \r \x1B[2K" << fps << std::flush;
    }
    std::cout << "\n moyenne : " << fps_somme / fps_compte << "fps"
              << std::flush;
}

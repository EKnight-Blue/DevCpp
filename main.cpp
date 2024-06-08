#include "Movement/CombinedBehavior.h"
#include "Movement/Flock.h"
#include "Movement/Steering.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/System/Clock.hpp>
#include <cmath>
#include <iostream>
int main() {
    sf::Texture texture;
    texture.loadFromFile("./resources/bg.jpg");
    sf::Sprite background(texture);
    sf::RenderWindow window{sf::VideoMode{800, 800}, "Game"};
    sf::View vue = window.getDefaultView();
    window.setFramerateLimit(30);
    ImGui::SFML::Init(window);
    Flock f1{Animal::Rabbit, 20.f, 100, 800, 800};
    f1.max_speed = 200.f;
    f1.max_force = 800.f;
    f1.put_on_rectangle(400, 200, 10, 10);

//    Flock f2{100, 800, 800};
//    f2.max_speed = 200.f;
//    f2.max_force = 800.f;
//    f2.put_on_rectangle(400, 200, 10, 10);
//    f2.move({0., 200.});

    //    Steering seek{Steering::Behavior::Flee, {.seek_flee={.target={400.,
    //    400.}}}, 20.};

    CombinedBehavior cb{};
//    cb.add(Steering::Behavior::Cohesion,
//           {.cas = {.detection_range = 25.f, .detection_cos_fov = -.3f}}, 15.f);
//    cb.add(Steering::Behavior::Alignment,
//           {.cas = {.detection_range = 30.f, .detection_cos_fov = -.2f}}, 15.f);
//    cb.add(Steering::Behavior::Separation,
//           {.cas = {.detection_range = 50.f, .detection_cos_fov = -.5f}}, 100.f);
//    cb.add(Steering::Behavior::Wander,
//           {.wander = {.sphere_dist = 100.f, .sphere_radius = 90.f, .displacement_amplitude=.2f}}, 6.f);

    Steering steer{Steering::Behavior::Arrival,
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
            ImGui::SFML::ProcessEvent(event);
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
                sf::FloatRect zoneVisible(0, 0, event.size.width,
                                          event.size.height);
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
        ImGui::SFML::Update(window, dt);
        ImGui::ShowDemoWindow();
        auto tmp = c.getElapsedTime();
        cb.compute(f1);
//        cb.compute(f2);
        f1.update(dt);
//        f2.update(dt);
        //-_-_-_-_-__-_-_-_-_-_-_-_--_-_DESSSIN
        window.draw(background);
        f1.draw(window);
//        f2.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }
}

#include "CombinedBehavior.h"
#include "Flock.h"
#include "Steering.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include <SFML/System/Clock.hpp>
#include <cmath>
int main() {
    sf::RenderWindow window{sf::VideoMode{800, 800}, "Game"};
    window.setFramerateLimit(30);
    ImGui::SFML::Init(window);
    Flock f1{100, 800, 800};
    f1.max_speed = 200.f;
    f1.max_force = 800.f;
    f1.put_on_rectangle(400, 200, 10, 10);

    Flock f2{100, 800, 800};
    f2.max_speed = 200.f;
    f2.max_force = 800.f;
    f2.put_on_rectangle(400, 200, 10, 10);
    f2.move({0., 200.});

    //    Steering seek{Steering::Behavior::Flee, {.seek_flee={.target={400.,
    //    400.}}}, 20.};

    CombinedBehavior cb{};
    cb.add(Steering::Behavior::Arrival,
           {.arrival = {.target = {402., 399.}, .range = 3200.}}, 2.);
    cb.add(Steering::Behavior::Cohesion,
           {.cas = {.detection_range = 200, .detection_cos_fov = .3}}, 100.);
    cb.add(Steering::Behavior::Alignment,
           {.cas = {.detection_range = 300, .detection_cos_fov = .5}}, 100.);
    cb.add(Steering::Behavior::Separation,
           {.cas = {.detection_range = 180, .detection_cos_fov = -1}}, 500.);

    Steering steer{Steering::Behavior::Arrival,
                   {.arrival = {.target = {400., 400.}, .range = 200.}},
                   20.};
    sf::Clock c{};
    auto dt = c.restart();
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            switch (event.type) {

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
        cb.compute(f2);
        f1.update(dt);
        f2.update(dt);
        f1.draw(window);
        f2.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }
}

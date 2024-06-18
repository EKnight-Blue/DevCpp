#include "Behavior/AtomicBehavior.h"
#include "Creatures/Flock.h"
#include "SFML/System/Clock.hpp"
#include "World/FiniteWorld.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include <cmath>
#include <iostream>

void handle_events(sf::RenderWindow &window) {
    static sf::Vector2f last_mouse_position;
    static int last_button{-1};
    static sf::View vue = window.getDefaultView();

    sf::Vector2f new_mouse_position;

    sf::Event event{};
    while (window.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);
        switch (event.type) {
        case sf::Event::MouseButtonPressed:
            if (last_button != -1)
                break;
            last_button = event.mouseButton.button;
            last_mouse_position =
                window.mapPixelToCoords(sf::Mouse::getPosition(window));
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == last_button)
                last_button = -1;
            break;
        case sf::Event::MouseMoved:
            if (last_button != sf::Mouse::Middle)
                break;
            new_mouse_position =
                window.mapPixelToCoords(sf::Mouse::getPosition(window));
            vue.move(last_mouse_position - new_mouse_position);
            window.setView(vue);
            last_mouse_position =
                window.mapPixelToCoords(sf::Mouse::getPosition(window));
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
            sf::FloatRect zoneVisible(0, 0,
                                      static_cast<float>(event.size.width),
                                      static_cast<float>(event.size.height));
            vue = sf::View(zoneVisible);
            window.setView(vue);
            break;
        }
        case sf::Event::KeyPressed:
            if (event.key.code == sf::Keyboard::Q) {
                window.close();
            }
            break;

        case sf::Event::EventType::Closed:
            window.close();
            break;
        default:
            break;
        }
    }
}

int main() {
    float fps_somme = 0;
    long long int fps_compte = 0;
#ifdef WIN32
    std::system("color");
#endif
    sf::Texture texture;
    texture.loadFromFile("./resources/bg.jpg");
    sf::Sprite background(texture);
    sf::RenderWindow window{sf::VideoMode{800, 800}, "Game"};
    window.setFramerateLimit(0);
    FiniteWorld w{1600.f, 1600.f};
    w.flocks.emplace_back(Animal::Bird, 20.f, 2000, 50.f, 100.f);
    w.behaviors.emplace_back();

    Flock &f1{w.flocks[0]};
    f1.move({800.f, 800.f});

    auto cb{w.behaviors.begin()};
    cb->add(AtomicBehavior::Type::Cohesion,
            {.cas = {.range = 40.f, .cos_fov = -0.5f}}, 15.f);
    cb->add(AtomicBehavior::Type::Alignment,
            {.cas = {.range = 30.f, .cos_fov = .4f}}, 15.f);
    cb->add(AtomicBehavior::Type::Separation,
            {.cas = {.range = 50.f, .cos_fov = -.5f}}, 10.f);
    cb->add(AtomicBehavior::Type::Wander,
            {.wander = {.sphere_dist = 100.f,
                        .sphere_radius = 90.f,
                        .displacement_amplitude = .5f}},
            5.f);

    sf::Clock c{};
    auto dt = c.restart();
    std::cout << "\x1B[31m";

    ImGui::SFML::Init(window);

    while (window.isOpen()) {
        handle_events(window);
        window.clear(sf::Color::White);
        dt = c.restart();

        w.process_behaviors();
        w.update(dt);
        //-_-_-_-_-__-_-_-_-_-_-_-_--_-_DESSSIN
        window.draw(background);
        w.draw(window);
        ImGui::SFML::Update(window, dt);
        if (ImGui::Begin("Menu")) {
            w.make_gui();
        }
        ImGui::End();
        ImGui::SFML::Render(window);
        window.display();

        float fps = 1E6f / static_cast<float>(dt.asMicroseconds());
        if (fps < 90.f) {
            fps_somme += fps;
            fps_compte++;
        }
        std::cout << "\x1B[2K\r" << fps << std::flush;
    }
    std::cout << "\nmoyenne : " << fps_somme / static_cast<float>(fps_compte)
              << "fps\x1B[0m" << std::flush;
}

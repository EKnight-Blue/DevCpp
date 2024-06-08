#include "utils.h"
#include <random>

sf::Vector2f operator^(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    // complex product
    return {v1.x * v2.x - v1.y * v2.y, v1.x * v2.y + v1.y * v2.x};
}

sf::Vector2f operator*(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    // element-to-element product
    return {v1.x * v2.x, v1.y * v2.y};
}

float dot(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

float random_float() {
    static std::random_device rd;
    static std::default_random_engine engine(rd());
    static std::uniform_real_distribution<float> distribution{0.f, 1.f};
    return distribution(engine);
}
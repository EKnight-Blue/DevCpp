#include "utils.h"


sf::Vector2f operator^(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    // complex product
    return {v1.x * v2.x - v1.y * v2.y, v1.x * v2.y + v1.y * v2.x};
}

float dot(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}
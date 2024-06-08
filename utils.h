
//

#ifndef BOIDSFML_UTILS_H
#define BOIDSFML_UTILS_H
#include "SFML/Graphics.hpp"
#include <cmath>

sf::Vector2f operator^(sf::Vector2f const& v1, sf::Vector2f const& v2);
float dot(sf::Vector2f const& v1, sf::Vector2f const& v2);
float random_float();

constexpr float TWO_PI = 2.f * static_cast<float>(M_PI);


#endif //BOIDSFML_UTILS_H

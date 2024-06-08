
//

#ifndef BOIDSFML_UTILS_H
#define BOIDSFML_UTILS_H
#include "SFML/Graphics.hpp"

sf::Vector2f operator^(sf::Vector2f const& v1, sf::Vector2f const& v2);
float dot(sf::Vector2f const& v1, sf::Vector2f const& v2);

#endif //BOIDSFML_UTILS_H


//

#ifndef BOIDSFML_UTILS_H
#define BOIDSFML_UTILS_H
#include "SFML/Graphics.hpp"
#include <cmath>

// complex product
sf::Vector2f operator^(sf::Vector2f const& v1, sf::Vector2f const& v2);

// element-to-element product
sf::Vector2f operator*(sf::Vector2f const& v1, sf::Vector2f const& v2);
float dot(sf::Vector2f const& v1, sf::Vector2f const& v2);
float random_float();
uint8_t random_uint8();
size_t random_size();

template<typename T>
constexpr T pow_i(T base, std::size_t power) {
    T result{1};
    while (power) {
        if (power & 1) {
            result *= base;
        }
        power >>= 1;
        base *= base;
    }
    return result;
};

constexpr float TWO_PI = 2.f * static_cast<float>(M_PI);

#endif //BOIDSFML_UTILS_H

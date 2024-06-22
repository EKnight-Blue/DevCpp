#include "utils.h"
#include <random>

sf::Vector2f operator^(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    // complex product
    return {v1.x * v2.x - v1.y * v2.y, v1.x * v2.y + v1.y * v2.x};
}


sf::Vector2f operator~(sf::Vector2f const& v){
    // complex conjugate
    return {v.x, -v.y};
};

sf::Vector2f operator*(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    // element-to-element product
    return {v1.x * v2.x, v1.y * v2.y};
}

float dot(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

void truncate(sf::Vector2f &v, float max_norm) {
    float norm = magnitude(v);
    if (norm > max_norm) {
        float const scale{max_norm / norm};
        v.x *= scale;
        v.y *= scale;
    }
}


float random_float() {
    static std::random_device rd;
    static std::default_random_engine engine(rd());
    static std::uniform_real_distribution<float> distribution{0.f, 1.f};
    return distribution(engine);
}


size_t random_size() {
    static std::random_device rd;
    static std::default_random_engine engine(rd());
    static std::uniform_int_distribution<size_t> distribution{0, 255};
    return distribution(engine);
}

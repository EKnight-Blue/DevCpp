#ifndef BOIDSFML_VECTORFIELD_H
#define BOIDSFML_VECTORFIELD_H

#include <cstdint>
#include "SFML/Graphics.hpp"

class VectorField{
public:
    VectorField(size_t width, size_t height);
private:
    size_t width;
    size_t height;
    std::vector<sf::Vector2f> vectors;
};


#endif //BOIDSFML_VECTORFIELD_H


#ifndef BOIDSFML_FLOCKMEMBER_H
#define BOIDSFML_FLOCKMEMBER_H

#include <cstdint>
#include "SFML/System/Vector2.hpp"

struct FlockMember{
    sf::Vector2f position{0., 0.};
    sf::Vector2f orientation{1., 0.};
    sf::Vector2f force{0, 0};
    float speed{0.};
    float last_wander_angle{0.};
    uint16_t age{0};
    uint8_t state{1};
};

#endif //BOIDSFML_FLOCKMEMBER_H

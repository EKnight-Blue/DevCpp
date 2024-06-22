#ifndef BOIDSFML_NEIGHBORSEARCH_H
#define BOIDSFML_NEIGHBORITERATOR_H

#include "World/World.h"

/**
 * Search around the world for FlockMembers of a certain type in eye-sight of another member
 */
class NeighborIterator{
public:
    NeighborIterator(World const *world, Animal animal, FlockMember const& member, float range, float cos_fov);
    virtual ~NeighborIterator() = default;
    virtual FlockMember const * next() = 0;



protected:
    [[nodiscard]] bool test(const sf::Vector2f &position) const;
    [[nodiscard]] bool test_vector(const sf::Vector2f &position) const;
    World const * world;
    Animal const animal;
    FlockMember const& member;
    float const sq_range;
    float const cos_fov;
};



#endif //BOIDSFML_NEIGHBORSEARCH_H

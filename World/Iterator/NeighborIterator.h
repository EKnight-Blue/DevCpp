#ifndef BOIDSFML_NEIGHBORITERATOR_H
#define BOIDSFML_NEIGHBORITERATOR_H

#include "World/World.h"


class NeighborIterator{
public:
    NeighborIterator(World const * world, Animal animal, FlockMember const& member, float range, float cos_fov);
    virtual ~NeighborIterator() = default;
    virtual bool next(FlockMember &output_member) = 0;
protected:
    [[nodiscard]] bool test(const sf::Vector2f &position) const;
    World const * const world;
    Animal const animal;
    FlockMember const& member;
    float const sq_range;
    float const cos_fov;
};



#endif //BOIDSFML_NEIGHBORITERATOR_H

#ifndef BOIDSFML_NAIVEITERATOR_H
#define BOIDSFML_NAIVEITERATOR_H

#include "NeighborIterator.h"

class NaiveIterator : public NeighborIterator{
public:
    NaiveIterator(World const *const world, Animal const animal, FlockMember const &member, float const range, float const cos_fov);
    FlockMember const * next() override;
private:
    size_t flock_index{0};
    size_t member_index{0};
};

#endif //BOIDSFML_NAIVEITERATOR_H

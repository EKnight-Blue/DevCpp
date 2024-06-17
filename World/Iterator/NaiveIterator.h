#ifndef BOIDSFML_NAIVEITERATOR_H
#define BOIDSFML_NAIVEITERATOR_H

#include "NeighborIterator.h"

class NaiveIterator : public NeighborIterator{
public:
    NaiveIterator(World const * world, Animal animal, FlockMember const &member, float range, float cos_fov);
    bool next(FlockMember &output_member) override;
private:
    size_t flock_index{0};
    size_t member_index{0};
};

#endif //BOIDSFML_NAIVEITERATOR_H

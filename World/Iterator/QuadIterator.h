#ifndef BOIDSFML_QUADITERATOR_H
#define BOIDSFML_QUADITERATOR_H
#include "NeighborIterator.h"
#include "World/FiniteWorld.h"

class QuadIterator : public NeighborIterator{
public:
    QuadIterator(FiniteWorld *world, Animal animal, FlockMember const &member, float range, float cos_fov);
    FlockMember* next() override;
private:
    QuadTree const * current;
    float range;
    size_t element_index{0};

    FlockMember * process_elements();
    bool propagate_to_children();
};

#endif //BOIDSFML_QUADITERATOR_H

#ifndef BOIDSFML_QUADITERATOR_H
#define BOIDSFML_QUADITERATOR_H
#include "NeighborIterator.h"
#include "World/FiniteWorld.h"

/**
 * Use QuadTrees to to search for neighbors
 */
class QuadIterator : public NeighborIterator{
public:
    QuadIterator(FiniteWorld const *world, Animal animal, FlockMember const &member, float range, float cos_fov);
    FlockMember const * next() override;
private:
    QuadTree const * current;
    float range;
    size_t element_index{0};

    FlockMember const * process_elements();
    bool propagate_to_children();
};

#endif //BOIDSFML_QUADITERATOR_H

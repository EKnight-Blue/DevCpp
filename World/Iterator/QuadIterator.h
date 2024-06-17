#ifndef BOIDSFML_QUADITERATOR_H
#define BOIDSFML_QUADITERATOR_H
#include "NeighborIterator.h"
#include "World/FiniteWorld.h"

class QuadIterator : public NeighborIterator{
public:
    QuadIterator(FiniteWorld const *world, Animal animal, FlockMember const &member, float range, float cos_fov);
    bool next(FlockMember &output_member) override;
private:
    QuadTree const * current;
    float range;
    size_t element_index{0};

    bool process_elements(FlockMember &output_member);
    bool propagate_to_children();
};

#endif //BOIDSFML_QUADITERATOR_H

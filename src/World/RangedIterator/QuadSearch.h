
//

#ifndef BOIDSFML_QUADSEARCH_H
#define BOIDSFML_QUADSEARCH_H

#include "NeighborSearch.h"
#include "World/FiniteWorld.h"

class QuadSearch : public NeighborSearch {
protected:
    FlockMember* next() override;

public:
    QuadSearch(FiniteWorld *world, Animal animal, FlockMember const& eyes, FOV fov);

private:
    QuadTree * current_node;
    size_t element_index{0};
    float x_min;
    float dy;

    std::array<sf::Vector2f, 4> approximation_rect;

    FlockMember * process_elements();
    bool intersects(QuadTree * tree);
    bool propagate_to_children();
};

#endif //BOIDSFML_QUADSEARCH_H

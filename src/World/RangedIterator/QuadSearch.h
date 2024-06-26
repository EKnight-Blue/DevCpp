
//

#ifndef BOIDSFML_QUADSEARCH_H
#define BOIDSFML_QUADSEARCH_H

#include "NeighborSearch.h"
#include "World/FiniteWorld.h"

class QuadSearch : public NeighborSearch {
public:
    FlockMember* next() override;
    NeighborGenerator co_neighbors() override;
    QuadSearch(FiniteWorld *world, Animal animal, FlockMember const& eyes, FOV fov);

private:
    QuadTree * current_node;
    size_t element_index{0};

    sf::Vector2f approximation_top_left;
    sf::Vector2f approximation_bottom_right;

    FlockMember * process_elements();
    bool intersects(QuadTree const *tree) const;
    bool propagate_to_children();
    friend QuadTree;
};

#endif //BOIDSFML_QUADSEARCH_H

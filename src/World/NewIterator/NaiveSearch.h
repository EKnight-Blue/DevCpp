
//

#ifndef BOIDSFML_NAIVESEARCH_H
#define BOIDSFML_NAIVESEARCH_H

#include "NeighborSearch.h"

class NaiveSearch : public NeighborSearch {
public:
    NaiveSearch(World *world, Animal animal, FlockMember const& eyes, FOV fov);
protected:
    FlockMember* next() override;
private:
    size_t member_index{0};
    size_t flock_index{0};
};


#endif //BOIDSFML_NAIVESEARCH_H

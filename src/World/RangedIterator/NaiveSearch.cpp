
//

#include "NaiveSearch.h"
#include "World/World.h"

NaiveSearch::NaiveSearch(World *world, Animal animal, const FlockMember &eyes, FOV fov) :
        NeighborSearch(world, animal, eyes, fov) {}

FlockMember *NaiveSearch::next() {
    for (; flock_index < world->flocks.size(); ++flock_index) {
        Flock& flock{world->flocks[flock_index]};

        if (flock.animal != animal) continue;

        for (; member_index < flock.members.size(); ++member_index) {
            FlockMember & candidate{flock.members[member_index]};
            if (&candidate == &eyes) continue;

            if (test(candidate.position)) {
                ++member_index;
                return &candidate;
            }
        }
    }
    return nullptr;
}

NeighborGenerator NaiveSearch::co_neighbors() {
    for (auto& flock : world->flocks) {
        if (flock.animal != animal) continue;
        for (auto& candidate : flock.members) {
            if (&candidate == &eyes) continue;

            if (test(candidate.position)) {
                co_yield &candidate;
            }
        }
    }
}
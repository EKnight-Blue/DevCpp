#include "NaiveIterator.h"


NaiveIterator::NaiveIterator(World const *const world, Animal const animal, FlockMember const &member, float const range, float const cos_fov) :
        NeighborIterator(world, animal, member, range, cos_fov){
}

FlockMember const * NaiveIterator::next() {
    // restart from last position
    for (;flock_index < world->flocks.size(); ++flock_index) {
        Flock const& flock{world->flocks[flock_index]};
        // only search flock with the right animal
        if (flock.animal != animal) {
            member_index = 0;
            continue;
        }
        for (;member_index <flock.members.size(); ++member_index) {
            FlockMember const& other{flock.members[member_index]};
            // no reflexion
            if (&other == &member)
                continue;
            if (test(other.position)) {
                // next time, start from the next
                ++member_index;
                return &other;
            }
        }
        member_index = 0;
    }
    return nullptr;
}
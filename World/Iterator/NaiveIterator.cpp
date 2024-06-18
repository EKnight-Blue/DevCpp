#include "NaiveIterator.h"


NaiveIterator::NaiveIterator(World * const world, Animal const animal, FlockMember const &member, float const range, float const cos_fov) :
        NeighborIterator(world, animal, member, range, cos_fov){
}

FlockMember* NaiveIterator::next() {
    for (;flock_index < world->flocks.size(); ++flock_index) {
        Flock & flock{world->flocks[flock_index]};
        if (flock.animal != animal) {
            member_index = 0;
            continue;
        }
        for (;member_index <flock.members.size(); ++member_index) {
            FlockMember & other{flock.members[member_index]};
            if (&other == &member)
                continue;
            if (test(other.position)) {
                ++member_index;
                return &other;
            }
        }
        member_index = 0;
    }
    return nullptr;
}
#include "NaiveIterator.h"


NaiveIterator::NaiveIterator(World const * const world, Animal const animal, FlockMember const &member, float const range, float const cos_fov) :
        NeighborIterator(world, animal, member, range, cos_fov){
}

bool NaiveIterator::next(FlockMember &output_member) {
    for (;flock_index < world->flocks.size(); ++flock_index) {
        Flock const& flock{world->flocks[flock_index]};
        if (flock.animal != animal) {
            member_index = 0;
            continue;
        }
        for (;member_index <flock.members.size(); ++member_index) {
            FlockMember const& other{flock.members[member_index]};
            if (&other == &member)
                continue;
            if (test(other.position)) {
                output_member = other;
                ++member_index;
                return true;
            }
        }
        member_index = 0;
    }
    return false;
}
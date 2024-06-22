#include "StatedBehavior.h"

void StatedBehavior::compute_body(Flock& flock, FlockMember& member, World * world) {
    behaviors[member.state].compute_body(flock, member, world);
}
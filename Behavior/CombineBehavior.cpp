#include "CombinedBehavior.h"

void CombinedBehavior::compute_body(Flock& flock, FlockMember& member, World * world) {
    for (auto& steering : behaviors) {
        steering.compute_body(flock, member, world);
    }
}

void CombinedBehavior::add(AtomicBehavior::Type behavior, AtomicBehavior::Parameters const& data, const float c) {
    behaviors.emplace_back(behavior, data, c);
}
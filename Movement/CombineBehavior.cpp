#include "CombinedBehavior.h"


void CombinedBehavior::compute(Flock &flock) {
    for (auto& steering : behaviors) {
        steering.compute(flock);
    }
}

void CombinedBehavior::add(Steering::Behavior behavior, SteeringParameters const& data, const float c) {
    behaviors.emplace_back(behavior, data, c);
}
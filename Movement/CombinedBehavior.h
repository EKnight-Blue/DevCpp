#ifndef BOIDSFML_COMBINEDBEHAVIOR_H
#define BOIDSFML_COMBINEDBEHAVIOR_H

#include "Steering.h"

class CombinedBehavior {
public:
    void compute(Flock &flock);
    void add(Steering::Behavior behavior, SteeringParameters const& data, float c);
private:
    std::vector<Steering> behaviors{};
};

#endif //BOIDSFML_COMBINEDBEHAVIOR_H

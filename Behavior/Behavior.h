#ifndef BOIDSFML_BEHAVIOR_H
#define BOIDSFML_BEHAVIOR_H

#include "World/World.h"

class Behavior{
public:
    void compute(Flock& flock, World* world) {
        for (auto& member : flock.members) {
            compute_body(flock, member, world);
        }
    };
    virtual ~Behavior() = default;
    virtual void make_gui() = 0;
private:
    virtual void compute_body(Flock& flock, FlockMember& member, World * world) = 0;
};

#endif //BOIDSFML_BEHAVIOR_H

#ifndef BOIDSFML_BEHAVIOR_H
#define BOIDSFML_BEHAVIOR_H

#include "Creatures/Flock.h"

class World;
class Behavior{
public:
    /**
     * Compute behavior on the whole flock
     * @param flock
     * @param world
     */
    void compute(Flock& flock, World *world) {
        for (auto& member : flock.members) {
            compute_body(flock, member, world);
        }
    };
    virtual ~Behavior() = default;
    virtual void make_gui() = 0;
private:
    /**
     * Compute behavior on one member
     * @param flock
     * @param member
     * @param world
     */
    virtual void compute_body(Flock& flock, FlockMember& member, World * world) = 0;
};

#endif //BOIDSFML_BEHAVIOR_H

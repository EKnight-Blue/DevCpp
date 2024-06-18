
//

#ifndef BOIDSFML_STATEDBEHAVIOR_H
#define BOIDSFML_STATEDBEHAVIOR_H

#include "CombinedBehavior.h"

class StatedBehavior : public Behavior{
private:
    void compute_body(Flock& flock, FlockMember& member, World const * world) override;
    std::vector<CombinedBehavior> behaviors;
};



#endif //BOIDSFML_STATEDBEHAVIOR_H

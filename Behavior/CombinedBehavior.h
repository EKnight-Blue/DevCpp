#ifndef BOIDSFML_COMBINEDBEHAVIOR_H
#define BOIDSFML_COMBINEDBEHAVIOR_H

#include "Behavior.h"
#include "AtomicBehavior.h"

class CombinedBehavior : public Behavior{
public:
    void add(AtomicBehavior::Type behavior, AtomicBehavior::Parameters const& data, float c);
    void make_gui() override;
private:
    friend class StatedBehavior;
    void compute_body(Flock& flock, FlockMember& member, World * world) override;
    std::vector<AtomicBehavior> behaviors{};
};

#endif //BOIDSFML_COMBINEDBEHAVIOR_H

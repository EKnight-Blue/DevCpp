
//

#ifndef BOIDSFML_STEERINGBEHAVIOR_H
#define BOIDSFML_STEERINGBEHAVIOR_H

#include "Flock.h"

/**
 * Steering behaviors from
 * https://www.red3d.com/cwr/steer/gdc99/
 */

class SteeringBehavior{
public:
    explicit SteeringBehavior(Flock& flock) : flock{flock} {};
    void compute();
    virtual ~SteeringBehavior() = default;
protected:
    Flock& flock;
private:
    virtual void compute_one_member(FlockMember& member) = 0;
};


class Seek final : public SteeringBehavior {
public:
    explicit Seek(Flock& flock, sf::Vector2f target) : SteeringBehavior{flock}, target{target} {};
    ~Seek() override = default;
private:
    sf::Vector2f target;
    void compute_one_member(FlockMember& member) override;
};

#endif //BOIDSFML_STEERINGBEHAVIOR_H

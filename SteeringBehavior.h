
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
    virtual sf::Vector2f compute_one_member(FlockMember& member) = 0;
};


class Seek final : public SteeringBehavior {
public:
    explicit Seek(Flock& flock, sf::Vector2f target) : SteeringBehavior{flock}, target{target} {};
private:
    sf::Vector2f target;
    sf::Vector2f compute_one_member(FlockMember& member) override;
};

class Arrival final : public SteeringBehavior {
public:
    explicit Arrival(Flock& flock, sf::Vector2f target, float slowdown_distance) : SteeringBehavior{flock}, target{target}, slowdown_distance{slowdown_distance} {};
private:
    sf::Vector2f target;
    float slowdown_distance;
    sf::Vector2f compute_one_member(FlockMember& member) override;
};



class Flee final : public SteeringBehavior {
public:
    explicit Flee(Flock& flock, sf::Vector2f target) : SteeringBehavior{flock}, target{target} {};
private:
    sf::Vector2f target;
    sf::Vector2f compute_one_member(FlockMember& member) override;
};


#endif //BOIDSFML_STEERINGBEHAVIOR_H

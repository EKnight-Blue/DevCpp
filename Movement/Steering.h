
//

#ifndef DEV_CPP_STEERING_H
#define DEV_CPP_STEERING_H

#include "Flock.h"

/**
 * Steering behaviors from
 * https://www.red3d.com/cwr/steer/gdc99/
 */


union SteeringParameters{
    struct SeekFlee{
        sf::Vector2f target;
    };
    SeekFlee seek_flee;

    struct Arrival{
        sf::Vector2f target{0., 0.};
        float range{0.};
    };
    Arrival arrival;

    struct CohesionAlignmentSeparation{
        float detection_range;
        float detection_cos_fov;
    };
    CohesionAlignmentSeparation cas;

    struct Wander{
        float sphere_dist;
        float sphere_radius;
        float displacement_amplitude;
    };
    Wander wander;
};


class Steering{
public:
    enum class Behavior{
        Seek,
        Flee,
        Arrival,
        Cohesion,
        Alignment,
        Separation,
        Wander,
        Count
    };

    explicit Steering(Behavior behavior, SteeringParameters const& data, const float c) : behavior{behavior}, parameters{data}, coefficient{c} {};
    void update_values(SteeringParameters const& new_data, float c);
    void compute(Flock &flock);

private:
    Behavior const behavior;
    SteeringParameters parameters;
    float coefficient{1.0};

    [[nodiscard]] inline bool in_fov(FlockMember const& member, sf::Vector2f const& vec, float sq_radius) const;

    using BehaviorMethod = sf::Vector2f (Steering::*)(Flock const& flock, FlockMember& member) const;
    [[nodiscard]] sf::Vector2f seek(Flock const& flock, FlockMember &member) const;
    [[nodiscard]] sf::Vector2f flee(Flock const& flock, FlockMember &member) const;
    [[nodiscard]] sf::Vector2f arrival(Flock const& flock, FlockMember &member) const;
    [[nodiscard]] sf::Vector2f cohesion(Flock const& flock, FlockMember &member) const;
    [[nodiscard]] sf::Vector2f alignment(Flock const& flock, FlockMember &member) const;
    [[nodiscard]] sf::Vector2f separation(Flock const& flock, FlockMember &member) const;
    [[nodiscard]] sf::Vector2f wander(Flock const& flock, FlockMember &member) const;
};


#endif // DEV_CPP_STEERING_H

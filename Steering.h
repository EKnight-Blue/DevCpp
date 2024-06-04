
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
        Count
    };

    explicit Steering(Behavior behavior, SteeringParameters const& data, const float c) : behavior{behavior}, parameters{data}, coefficient{c} {};
    void update_values(SteeringParameters const& new_data, float c);
    void compute(Flock &flock);

private:
    Behavior behavior;
    SteeringParameters parameters;
    float coefficient{1.0};

    [[nodiscard]] inline bool in_fov(FlockMember const& member, sf::Vector2f const& vec, float sq_radius) const;

    using BehaviorMethod = sf::Vector2f (Steering::*)(Flock &flock, FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f seek(Flock &flock, FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f flee(Flock &flock, FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f arrival(Flock &flock, FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f cohesion(Flock &flock, FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f alignment(Flock &flock, FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f separation(Flock &flock, FlockMember const& member) const;
};


#endif // DEV_CPP_STEERING_H

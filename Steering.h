
//

#ifndef DEV_CPP_STEERING_H
#define DEV_CPP_STEERING_H

#include "Flock.h"

/**
 * Steering behaviors from
 * https://www.red3d.com/cwr/steer/gdc99/
 */


union SB_Data{
    struct {
        float neighbor_detection_range;
        float neighbor_detection_cos_fov;
    };
    struct {
        sf::Vector2f static_target{};
        float proximity_range{};
    };
};


struct Steering{
    enum class Behavior{
        Seek,
        Flee,
        Arrival,
        Cohesion,
        Alignment,
        Separation,
        Count
    };
    Behavior behavior;

    Flock& flock;
    SB_Data data;
    float coefficient{1.0};

    explicit Steering(Behavior behavior, Flock& flock, SB_Data const& data, const float c) : behavior{behavior}, flock{flock}, data{data}, coefficient{c} {};
    void update_values(SB_Data const& new_data, float c);

    void compute();
    using BehaviorMethod = sf::Vector2f (Steering::*)(FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f seek(FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f flee(FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f arrival(FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f cohesion(FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f alignment(FlockMember const& member) const;
    [[nodiscard]] sf::Vector2f separation(FlockMember const& member) const;
};


#endif // DEV_CPP_STEERING_H


//

#ifndef DEV_CPP_STEERING_H
#define DEV_CPP_STEERING_H

#include "Behavior.h"
/**
 * Steering behaviors from
 * https://www.red3d.com/cwr/steer/gdc99/
 */


class AtomicBehavior : public Behavior{
public:
    enum class Type{
        Seek,
        Flee,
        Arrival,
        Cohesion,
        Alignment,
        Separation,
        Wander,
        Pursuit,
        Evasion,
        Count
    };
    union Parameters{
        struct SeekFlee{
            sf::Vector2f target;
        };
        SeekFlee seek_flee;

        struct Arrival{
            sf::Vector2f target;
            float range;
        };
        Arrival arrival;


        struct DetectionFOV{
            float range;
            float cos_fov;
        };
        // Cohesion / Alignment / Separation
        DetectionFOV cas;

        struct Wander{
            float sphere_dist;
            float sphere_radius;
            float displacement_amplitude;
        };
        Wander wander;

        struct PursuitEvasion{
            Animal animal;
            DetectionFOV fov;
        };
        PursuitEvasion pursuit_evasion;
    };

    explicit AtomicBehavior(Type type, Parameters const& data, const float c) : type{type}, parameters{data}, coefficient{c} {};
    void update_values(Parameters const& new_data, float c);

private:
    friend class CombinedBehavior;
    void compute_body(Flock& flock, FlockMember& member, World * world) override;
    Type const type;
    Parameters parameters;
    float coefficient{1.0};

    using BehaviorMethod = sf::Vector2f (AtomicBehavior::*)(Flock const& flock, FlockMember& member, World const * world) const;
    [[nodiscard]] sf::Vector2f seek(Flock const& flock, FlockMember &member, World const *world) const;
    [[nodiscard]] sf::Vector2f flee(Flock const& flock, FlockMember &member, World const *world) const;
    [[nodiscard]] sf::Vector2f arrival(Flock const& flock, FlockMember &member, World const *world) const;
    [[nodiscard]] sf::Vector2f cohesion(Flock const& flock, FlockMember &member, World const *world) const;
    [[nodiscard]] sf::Vector2f alignment(Flock const& flock, FlockMember &member, World const *world) const;
    [[nodiscard]] sf::Vector2f separation(Flock const& flock, FlockMember &member, World const *world) const;
    [[nodiscard]] sf::Vector2f wander(Flock const &flock, FlockMember &member, World const *world) const;
    [[nodiscard]] sf::Vector2f pursuit(Flock const &flock, FlockMember &member, World const *world) const;
    [[nodiscard]] sf::Vector2f evasion(Flock const &flock, FlockMember &member, World const *world) const;
};


#endif // DEV_CPP_STEERING_H

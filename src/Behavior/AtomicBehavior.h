
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
            // Point on the world
            sf::Vector2f target;
        };
        SeekFlee seek_flee;

        struct Arrival{
            // Point on the world
            sf::Vector2f target;
            // Distance from which the agent slows down
            float range;
        };
        Arrival arrival;


        struct DetectionFOV{
            // Maximal distance to be seen
            float range;
            // Minimal normalized dot product with the member's orientation to be seen
            float cos_fov;
        };
        // Cohesion / Alignment / Separation
        DetectionFOV cas;

        struct Wander{
            // distance at which the circle is placed in front of the member
            float sphere_dist;
            float sphere_radius;
            // Maximal angle increment/decrement
            float displacement_amplitude;
        };
        Wander wander;

        struct PursuitEvasion{
            // Animals to hunt/to be scared of
            Animal animal;
            DetectionFOV fov;
            // Time used to predict the target future position
            float prediction_time;
        };
        PursuitEvasion pursuit_evasion;
    };

    explicit AtomicBehavior(Type type, Parameters const& data, const float c) : type{type}, parameters{data}, coefficient{c} {};
    void update_values(Parameters const &new_data, float new_coefficient);
    void make_gui() override;

private:
    friend class CombinedBehavior;
    void compute_body(Flock& flock, FlockMember& member, World * world) override;
    Type const type;
    Parameters parameters;
    float coefficient{1.0};

#pragma region BehaviorMethods
    using BehaviorMethod = void (AtomicBehavior::*)(Flock const& flock, FlockMember& member, World * world) const;
    void seek(Flock const& flock, FlockMember &member, World *world) const;
    void flee(Flock const& flock, FlockMember &member, World *world) const;
    void arrival(Flock const& flock, FlockMember &member, World *world) const;
    void cohesion(Flock const& flock, FlockMember &member, World *world) const;
    void alignment(Flock const& flock, FlockMember &member, World *world) const;
    void separation(Flock const& flock, FlockMember &member, World *world) const;
    void wander(Flock const &flock, FlockMember &member, World *world) const;
    void pursuit(Flock const &flock, FlockMember &member, World *world) const;
    void evasion(Flock const &flock, FlockMember &member, World *world) const;
#pragma endregion BehaviorMethods

#pragma region GuiMethods
    using BehaviorGui = void (AtomicBehavior::*)();
    void seek_flee_gui();
    void arrival_gui();
    void cas_gui();
    void wander_gui();
    void pursuit_evasion_gui();
#pragma endregion GuiMethods

};


#endif // DEV_CPP_STEERING_H

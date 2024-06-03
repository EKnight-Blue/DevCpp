#include "SteeringBehavior.h"
#include <cmath>

void SteeringBehavior::compute() {
    for (auto& member : flock.members) {
        compute_one_member(member);
    }
}

inline float magnitude(sf::Vector2f const& v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

void Seek::compute_one_member(FlockMember &member) {
    sf::Vector2f desired_velocity = target - member.position;
    float const old_mag = magnitude(desired_velocity);
    if (old_mag == 0.f) return;

    member.force += desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation;
}
#include "Steering.h"
#include <cmath>
#include <array>
#include <functional>


inline float magnitude(sf::Vector2f const& v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

inline float sq_magnitude(sf::Vector2f const& v) {
    return v.x * v.x + v.y * v.y;
}


void Steering::update_values(const SB_Data &new_data, const float c) {
    data = new_data;
    coefficient = c;
}



sf::Vector2f sub_seek(Flock const& flock, FlockMember const& member, sf::Vector2f desired_velocity) {
    float const old_mag = magnitude(desired_velocity);
    if (old_mag == 0.f) {
        return -member.speed * member.orientation;
    } else {
        return desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation;
    }
}


sf::Vector2f Steering::seek(FlockMember const& member) const {
    sf::Vector2f desired_velocity = data.static_target - member.position;
    return sub_seek(flock, member, desired_velocity);
}


sf::Vector2f Steering::flee(FlockMember const& member) const {
    sf::Vector2f desired_velocity = member.position - data.static_target;
    return sub_seek(flock, member, desired_velocity);
}

sf::Vector2f Steering::arrival(FlockMember const& member) const {
    sf::Vector2f desired_velocity = data.static_target - member.position;
    float const old_mag = magnitude(desired_velocity);
    if (old_mag < data.proximity_range) {
        return desired_velocity * (flock.max_speed / data.proximity_range) - member.speed * member.orientation;
    } else {
        return desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation;
    }
}


inline bool in_fov(Steering const * const behavior, FlockMember const& member, sf::Vector2f const & vec, float const& sq_radius) {
    float sq_mag = sq_magnitude(vec);
    return sq_mag < sq_radius && dot(member.orientation, vec) >= behavior->data.neighbor_detection_cos_fov * sqrtf(sq_mag);
}


sf::Vector2f Steering::cohesion(FlockMember const& member) const {
    sf::Vector2f desired_velocity{0., 0.};
    size_t neighbor_cnt = 0;
    float sq_radius = data.neighbor_detection_range * data.neighbor_detection_range;
    for (auto const& neighbor : flock.members) {
        if (&neighbor == &member)
            continue;
        sf::Vector2f vec = neighbor.position - member.position;
        if (in_fov(this, member, vec, sq_radius)) {
            ++neighbor_cnt;
            desired_velocity += vec;
        }
    }
    if (!neighbor_cnt) {
        return desired_velocity;
    }
    return sub_seek(flock, member, desired_velocity / static_cast<float>(neighbor_cnt));
}


sf::Vector2f Steering::alignment(FlockMember const& member) const {
    sf::Vector2f desired_velocity{0., 0.};
    size_t neighbor_cnt = 0;
    float sq_radius = data.neighbor_detection_range * data.neighbor_detection_range;
    for (auto const& neighbor : flock.members) {
        if (&neighbor == &member)
            continue;
        sf::Vector2f vec = member.position - neighbor.position;
        if (in_fov(this, member, vec, sq_radius)) {
            desired_velocity += member.speed * member.orientation;
            ++neighbor_cnt;
        }
    }
    if (!neighbor_cnt) {
        // 0., 0.
        return desired_velocity;
    }
    return sub_seek(flock, member, desired_velocity / static_cast<float>(neighbor_cnt));
}


sf::Vector2f Steering::separation(FlockMember const& member) const {
    sf::Vector2f force{0., 0.};
    float sq_radius = data.neighbor_detection_range * data.neighbor_detection_range;
    for (auto const& neighbor : flock.members) {
        if (&neighbor == &member)
            continue;
        sf::Vector2f vec = member.position - neighbor.position;
        float sq_mag = sq_magnitude(vec);
        if (0.f < sq_mag && in_fov(this, member, vec, sq_radius)) {
            force += vec / sq_mag;
        }
    }
    return force;
}


void Steering::compute() {
    static std::array<BehaviorMethod, static_cast<size_t>(Behavior::Count)> behaviors{
        &Steering::seek,
        &Steering::flee,
        &Steering::arrival,
        &Steering::cohesion,
        &Steering::alignment,
        &Steering::separation
    };
    BehaviorMethod const method = behaviors[static_cast<size_t>(behavior)];

    for (auto& member : flock.members) {
        member.force += coefficient * std::invoke(method, this, member);
    }
}
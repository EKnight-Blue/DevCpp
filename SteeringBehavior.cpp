#include "SteeringBehavior.h"
#include <cmath>

void SteeringBehavior::compute() {
    for (auto& member : flock.members) {
        member.force += coefficient * compute_one_member(member);
    }
}

inline float magnitude(sf::Vector2f const& v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

inline float sq_magnitude(sf::Vector2f const& v) {
    return v.x * v.x + v.y * v.y;
}


sf::Vector2f seek(Flock const& flock, FlockMember const& member, sf::Vector2f desired_velocity) {
    float const old_mag = magnitude(desired_velocity);
    if (old_mag == 0.f) {
        return -member.speed * member.orientation;
    } else {
        return desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation;
    }
}


sf::Vector2f Seek::compute_one_member(FlockMember const& member) {
    sf::Vector2f desired_velocity = target - member.position;
    return seek(flock, member, desired_velocity);
}


sf::Vector2f Arrival::compute_one_member(FlockMember const& member) {
    sf::Vector2f desired_velocity = target - member.position;
    float const old_mag = magnitude(desired_velocity);
    if (old_mag < slowdown_distance) {
        return desired_velocity * (flock.max_speed / slowdown_distance) - member.speed * member.orientation;
    } else {
        return desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation;
    }
}


sf::Vector2f Flee::compute_one_member(FlockMember const& member) {
    sf::Vector2f desired_velocity = member.position - target;
    return seek(flock, member, desired_velocity);
}



sf::Vector2f Cohesion::compute_one_member(FlockMember const& member) {
    sf::Vector2f desired_velocity{0., 0.};
    size_t neighbor_cnt = 0;
    float sq_radius = radius * radius;
    for (auto const& neighbor : flock.members) {
        if (&neighbor == &member)
            continue;
        sf::Vector2f vec = neighbor.position - member.position;
        if (sq_magnitude(vec) < sq_radius) {
            ++neighbor_cnt;
            desired_velocity += vec;
        }
    }
    if (!neighbor_cnt) {
        // 0., 0.
        return desired_velocity;
    }
    return seek(flock, member, desired_velocity / static_cast<float>(neighbor_cnt));
}


sf::Vector2f Separation::compute_one_member(FlockMember const& member) {
    sf::Vector2f desired_velocity{0., 0.};
    size_t neighbor_cnt = 0;
    float sq_radius = radius * radius;
    for (auto const& neighbor : flock.members) {
        if (&neighbor == &member)
            continue;
        if (sq_magnitude(member.position - neighbor.position) < sq_radius) {
            desired_velocity += member.speed * member.orientation;
            ++neighbor_cnt;
        }
    }
    if (!neighbor_cnt) {
        // 0., 0.
        return desired_velocity;
    }
    return seek(flock, member, desired_velocity / static_cast<float>(neighbor_cnt));
}




sf::Vector2f Alignment::compute_one_member(FlockMember const& member) {
    sf::Vector2f force{0., 0.};
    float sq_radius = radius * radius;
    for (auto const& neighbor : flock.members) {
        if (&neighbor == &member)
            continue;
        sf::Vector2f vec = member.position - neighbor.position;
        float sq_mag = sq_magnitude(vec);
        if (0.f < sq_mag && sq_mag < sq_radius) {
            force += vec / sq_mag;
        }
    }
    return force;
}


void Seek::met_cible(sf::Vector2f nouvelle_cible) {
    target = nouvelle_cible;
}
void Arrival::met_cible(sf::Vector2f nouvelle_cible) {
    target = nouvelle_cible;
}
void Flee::met_cible(sf::Vector2f nouvelle_cible) {
    target = nouvelle_cible;
}

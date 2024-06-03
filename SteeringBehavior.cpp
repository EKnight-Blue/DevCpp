#include "SteeringBehavior.h"
#include <cmath>

void SteeringBehavior::compute() {
  for (auto &member : flock.members) {
    member.force += compute_one_member(member);
  }
}

inline float magnitude(sf::Vector2f const &v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}

sf::Vector2f Seek::compute_one_member(FlockMember &member) {
  sf::Vector2f desired_velocity = target - member.position;
  float const old_mag = magnitude(desired_velocity);
  if (old_mag == 0.f) {
    return -member.speed * member.orientation;
  } else {
    return desired_velocity * (flock.max_speed / old_mag) -
           member.speed * member.orientation;
  }
}

sf::Vector2f Arrival::compute_one_member(FlockMember &member) {
  sf::Vector2f desired_velocity = target - member.position;
  float const old_mag = magnitude(desired_velocity);
  if (old_mag < slowdown_distance) {
    return desired_velocity * (flock.max_speed / slowdown_distance) -
           member.speed * member.orientation;
  } else {
    return desired_velocity * (flock.max_speed / old_mag) -
           member.speed * member.orientation;
  }
}

sf::Vector2f Flee::compute_one_member(FlockMember &member) {
  sf::Vector2f desired_velocity = member.position - target;
  float const old_mag = magnitude(desired_velocity);
  if (old_mag == 0.f) {
    return flock.max_speed * member.orientation;
  } else {
    return desired_velocity * (flock.max_speed / old_mag) -
           member.speed * member.orientation;
  }
}
void Seek::met_cible(sf::Vector2f nouvelle_cible) { target = nouvelle_cible; }
void Arrival::met_cible(sf::Vector2f nouvelle_cible) {
  target = nouvelle_cible;
}
void Flee::met_cible(sf::Vector2f nouvelle_cible) { target = nouvelle_cible; }

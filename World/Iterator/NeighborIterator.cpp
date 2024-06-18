#include "NeighborIterator.h"
#include "utils.h"

NeighborIterator::NeighborIterator(World * const world, Animal const animal, FlockMember const &member, float const range, float const cos_fov) :
        world{world}, animal{animal}, member{member}, sq_range{range * range}, cos_fov{cos_fov} {
}

bool NeighborIterator::test(const sf::Vector2f &position) const {
    sf::Vector2f vec{world->position_difference(position, member.position)};
    float sq_mag = sq_magnitude(vec);
    return sq_mag < sq_range && dot(member.orientation, vec) >= cos_fov * sqrtf(sq_mag);
}
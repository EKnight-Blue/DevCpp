#include "NeighborIterator.h"
#include "utils.h"

NeighborIterator::NeighborIterator(World const *world, Animal animal, FlockMember const &member, float range, float cos_fov) :
        world{world}, animal{animal}, member{member}, sq_range{range * range}, cos_fov{cos_fov} {
}

/**
 * Individual eye-sight test
 * @param position
 * @return
 */
bool NeighborIterator::test(const sf::Vector2f &position) const {
    sf::Vector2f vec{world->position_difference(position, member.position)};
    return test_vector(vec);
}

bool NeighborIterator::test_vector(const sf::Vector2f &vec) const {
    float sq_mag = sq_magnitude(vec);
    return sq_mag < sq_range && dot(member.orientation, vec) >= cos_fov * sqrtf(sq_mag);
}
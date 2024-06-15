#include "World.h"
#include "utils.h"

World::NeighborIterator::NeighborIterator(World const * const world, Animal const animal, FlockMember const &member, float const range, float const cos_fov) :
    world{world}, animal{animal}, member{member}, sq_range{range * range}, cos_fov{cos_fov} {
}

bool World::NeighborIterator::test(const FlockMember &other) const {
    sf::Vector2f vec{world->position_difference(other.position, member.position)};
    float sq_mag = sq_magnitude(vec);
    return sq_mag < sq_range && dot(member.orientation, vec) >= cos_fov * sqrtf(sq_mag);
}

World::NaiveIterator::NaiveIterator(World const * const world, Animal const animal, FlockMember const &member, float const range, float const cos_fov) :
        NeighborIterator(world, animal, member, range, cos_fov){
}

bool World::NaiveIterator::next(FlockMember &output_member) {
    for (;flock_index < world->flocks.size(); ++flock_index) {
        Flock const& flock{world->flocks[flock_index]};
        if (flock.animal != animal) {
            member_index = 0;
            continue;
        }
        for (;member_index <flock.members.size(); ++member_index) {
            FlockMember const& other{flock.members[member_index]};
            if (&other == &member)
                continue;
            if (test(other)) {
                output_member = other;
                ++member_index;
                return true;
            }
        }
        member_index = 0;
    }
    return false;
}

std::unique_ptr<World::NeighborIterator> World::make_neighbor_iterator(Animal animal, FlockMember const &member, float range, float cos_fov) const {
    return std::make_unique<NaiveIterator>(this, animal, member, range, cos_fov);
}

void World::update(sf::Time delta_time) {
    for (auto& flock : flocks) {
        flock.update(delta_time);
    }
}


sf::Vector2f ToroidalWorld::position_difference(sf::Vector2f const &v1, sf::Vector2f const &v2) const {
    sf::Vector2f v = v1 - v2;
    return v + sf::Vector2f{
        width * (static_cast<float>(v.x < -width * .5f) - static_cast<float>(v.x > width * .5f)),
        height * (static_cast<float>(v.y < -height * .5f) - static_cast<float>(v.y > height * .5f))
    };
}

void ToroidalWorld::validate_position(sf::Vector2f &position) const {
    position += sf::Vector2f{
        width * (static_cast<float>(position.x < 0.f) - static_cast<float>(position.x > width)),
        height * (static_cast<float>(position.y < 0.f) - static_cast<float>(position.y > height))
    };
}

void ToroidalWorld::update(sf::Time delta_time) {
    World::update(delta_time);
    for (auto& flock : flocks) {
        for (auto& member : flock.members) {
            validate_position(member.position);
        }
    }
}
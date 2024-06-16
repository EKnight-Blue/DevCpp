#include "World.h"
#include "utils.h"

World::NeighborIterator::NeighborIterator(World const * const world, Animal const animal, FlockMember const &member, float const range, float const cos_fov) :
    world{world}, animal{animal}, member{member}, sq_range{range * range}, cos_fov{cos_fov} {
}

bool World::NeighborIterator::test(const sf::Vector2f &position) const {
    sf::Vector2f vec{world->position_difference(position, member.position)};
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
            if (test(other.position)) {
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
    tree.reset();
    for (uint32_t flock_index{0}; flock_index < flocks.size(); ++flock_index) {
        Flock& flock{flocks[flock_index]};
        for (uint32_t member_index{0}; member_index < flock.members.size(); ++member_index) {
            FlockMember& member{flock.members[member_index]};
            validate_position(member.position);
            tree.insert({
                .animal=flock.animal,
                .position=member.position,
                .flock_index=flock_index,
                .member_index=member_index,
            });
        }

    }

}

ToroidalWorld::QuadIterator::QuadIterator(const ToroidalWorld *world, Animal animal, const FlockMember &member, float range, float cos_fov) :
        World::NeighborIterator(world, animal, member, range, cos_fov), current{&world->tree}, range{range} {

}
bool ToroidalWorld::QuadIterator::process_elements(FlockMember &output_member) {
    for (; element_index < current->cnt; ++element_index) {
        auto &element = current->elements[element_index];
        if (element.animal != animal)
            continue;
        if (test(element.position)) {
            output_member = world->flocks[element.flock_index].members[element.member_index];
            ++element_index;
            return true;
        }
    }
    return false;
}

bool ToroidalWorld::QuadIterator::propagate_to_children() {
    for (auto& child : current->children) {
        if (child.intersects_fov(member.position, member.orientation, sq_range, cos_fov, world)) {
            current = &child;
            return true;
        }
    }
    return false;
}

bool ToroidalWorld::QuadIterator::next(FlockMember &output_member) {
    while (true) {
        if (process_elements(output_member))
            return true;
        element_index = 0;
        if (propagate_to_children())
            continue;
        while (current->parent) {
            QuadTree * parent{current->parent};
            // try to access next sibling
            ++current;

            // out of siblings
            if (current == parent->children.data() + 4) {
                // pop the "stack"
                current = parent;
                continue;
            }
            if (current->intersects_fov(member.position, member.orientation, sq_range, cos_fov, world))
                break;
        }
        if (!current->parent)
            return false;
    }
}

std::unique_ptr<World::NeighborIterator> ToroidalWorld::make_neighbor_iterator(Animal animal, const FlockMember &member, float range, float cos_fov) const {
    return std::make_unique<QuadIterator>(this, animal, member, range, cos_fov);
}
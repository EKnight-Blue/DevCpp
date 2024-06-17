#include "World.h"
#include "utils.h"
#include "Iterator/NaiveIterator.h"

std::unique_ptr<NeighborIterator> World::make_neighbor_iterator(Animal animal, FlockMember const &member, float range, float cos_fov) const {
    return std::make_unique<NaiveIterator>(this, animal, member, range, cos_fov);
}


void World::update(sf::Time delta_time) {
    for (auto& flock : flocks) {
        for (auto& member : flock.members){
            flock.update(delta_time, member);
        }
    }
}

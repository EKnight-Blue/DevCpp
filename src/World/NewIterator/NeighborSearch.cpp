#include "NeighborSearch.h"
#include "utils.h"

NeighborSearch::NeighborSearch(World *world, Animal animal, const FlockMember &eyes, FOV fov)
    : world(world), animal(animal), eyes{eyes}, fov(fov), sq_range(fov.range * fov.range) {}


bool NeighborSearch::test(sf::Vector2f point) {
    sf::Vector2f vec{world->position_difference(point, eyes.position)};
    float sq_mag = sq_magnitude(vec);
    return sq_mag < sq_range && dot(eyes.orientation, vec) >= fov.cos_fov * sqrtf(sq_mag);
}


NeighborRange::NeighborRange(std::unique_ptr<NeighborSearch> algo) : algo{std::move(algo)} {}

NeighborRange::Iterator::Iterator(NeighborSearch* algo) : algo{algo} {}

NeighborRange::Iterator NeighborRange::begin() {
    Iterator it(algo.get());
    return ++it;
}

NeighborRange::Iterator NeighborRange::end() {
    return Iterator{nullptr};
}

NeighborRange::Iterator &NeighborRange::Iterator::operator++() {
    item = algo->next();
    return *this;
}

FlockMember *NeighborRange::Iterator::operator->() {
    return item;
}

FlockMember &NeighborRange::Iterator::operator*() {
    return *item;
}

bool NeighborRange::Iterator::operator!=(NeighborRange::Iterator const& other) const {
    return item != other.item;
}
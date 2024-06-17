#include "QuadIterator.h"
#include "utils.h"

QuadIterator::QuadIterator(FiniteWorld const *world, Animal animal, FlockMember const &member, float range, float cos_fov) :
        NeighborIterator(world, animal, member, range, cos_fov), current{&world->tree}, range{range},
        ray1(member.orientation ^ sf::Vector2f{cos_fov, sqrtf(1 - cos_fov * cos_fov)}),
        ray2(member.orientation ^ sf::Vector2f{cos_fov, -sqrtf(1 - cos_fov * cos_fov)})
        {}

bool QuadIterator::process_elements(FlockMember &output_member) {
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

bool QuadIterator::propagate_to_children() {
    for (auto& child : current->children) {
        if (child.intersects_fov(member.position, member.orientation, sq_range, cos_fov, world)) {
            current = &child;
            return true;
        }
    }
    return false;
}


bool QuadIterator::line_line(const sf::Vector2f &v1, const sf::Vector2f &v2, const sf::Vector2f &director) const {
    if (director.x != 0.f) {
        float t{v1.x / director.x};
        float dy{v1.y + director.y * t};
        if (0.f < t && t < range && 0.f <= dy && current->top_left.y + dy <= current->bottom_right.y)
            return true;
        t = v2.x / director.x;
        dy = v2.y + director.y * t;
        if (0.f < t && t < range && 0.f <= dy && current->top_left.y + dy <= current->bottom_right.y)
            return true;
    }
    if (director.y != 0.f) {
        float t{v1.y / director.y};
        float dx{v1.x + director.x * t};
        if (0.f < t && t < range && 0.f <= dx && current->top_left.x + dx <= current->bottom_right.x)
            return true;
        t = v2.y / director.y;
        dx = v2.x + director.x * t;
        if (0.f < t && t < range && 0.f <= dx && current->top_left.x + dx <= current->bottom_right.x)
            return true;
    }
    return false;
}

bool QuadIterator::arc_line(const sf::Vector2f &v1, const sf::Vector2f &v2) const {
    if (v1.x * v1.x < sq_range) {
        float dy{sqrtf(sq_range - v1.x * v1.x)};
        float ny{current->top_left.y + v1.y + dy};
        if (0.f <= v1.y + dy && ny <= current->bottom_right.y && dot(member.orientation, sf::Vector2f{v1.x, dy}) >= cos_fov * range)
            return true;
    }
    if (v2.x * v2.x < sq_range) {
        float dy{sqrtf(sq_range - v2.x * v2.x)};
        float ny{current->top_left.y + v2.y + dy};
        if (0.f <= v2.y + dy && ny <= current->bottom_right.y && dot(member.orientation, sf::Vector2f{v2.x, dy}) >= cos_fov * range)
            return true;
    }
    if (v1.y * v1.y < sq_range) {
        float dx{sqrtf(sq_range - v1.y * v1.y)};
        float nx{current->top_left.y + v1.y + dx};
        if (0.f <= v1.y + dx && nx <= current->bottom_right.y && dot(member.orientation, sf::Vector2f{dx, v1.x}) >= cos_fov * range)
            return true;
    }
    if (v2.y * v2.y < sq_range) {
        float dx{sqrtf(sq_range - v2.y * v2.y)};
        float nx{current->top_left.y + v2.y + dx};
        if (0.f <= v2.y + dx && nx <= current->bottom_right.y && dot(member.orientation, sf::Vector2f{dx, v2.x}) >= cos_fov * range)
            return true;
    }
    return false;
}

bool QuadIterator::in_my_fov() const {
    if (current->top_left.x <= member.position.x && current->top_left.y <= member.position.y &&
            member.position.x <= current->bottom_right.x && member.position.y <= current->bottom_right.y) {
        return true;
    }
    sf::Vector2f const v1 = world->position_difference(current->top_left, member.position);
    sf::Vector2f const v2 = world->position_difference(current->bottom_right, member.position);

    // find any corner inside fov
    float m{sq_magnitude(v1)};
    if (m < sq_range && dot(member.orientation, v1) >= cos_fov * sqrtf(m))
        return true;
    m = sq_magnitude(v2);
    if (m < sq_range && dot(member.orientation, v2) >= cos_fov * sqrtf(m))
        return true;
    m = sq_magnitude({v1.x, v2.y});
    if (m < sq_range && dot(member.orientation, {v1.x, v2.y}) >= cos_fov * sqrtf(m))
        return true;
    m = sq_magnitude({v2.x, v1.y});
    if (m < sq_range && dot(member.orientation, {v2.x, v1.y}) >= cos_fov * sqrtf(m))
        return true;

    if (line_line(v1, v2, ray1) || line_line(v1, v2, ray2))
        return true;
    return false;
}

bool QuadIterator::next(FlockMember &output_member) {
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
            if (in_my_fov())
                break;
        }
        if (!current->parent)
            return false;
    }
}
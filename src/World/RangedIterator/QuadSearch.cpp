
//


#include "QuadSearch.h"
#include "utils.h"
#include <functional>

QuadSearch::QuadSearch(FiniteWorld *world, Animal animal, const FlockMember &eyes, FOV fov)
    : NeighborSearch(world, animal, eyes, fov), current_node{&world->tree},
    x_min(fov.range * ((fov.cos_fov < 0.f) ? fov.cos_fov : 0.f)),
    dy(fov.range * ((fov.cos_fov < 0.f) ? 1.f : sqrtf(1.f - fov.cos_fov * fov.cos_fov))),
    approximation_rect{{
       sf::Vector2f{fov.range, dy} ^ eyes.orientation,
       sf::Vector2f{fov.range, -dy} ^ eyes.orientation,
       sf::Vector2f{x_min, dy} ^ eyes.orientation,
       sf::Vector2f{x_min, -dy} ^ eyes.orientation
}} {
    for (auto& point : approximation_rect) {
        std::invoke(world->validator, world, point);
    }
}


FlockMember *QuadSearch::process_elements() {
    for (; element_index < QuadTreeSize; ++element_index) {
        QuadTreeElement& candidate{current_node->elements[element_index]};
        if (candidate.animal != animal) continue;
        if (!test(candidate.position)) continue;

        FlockMember * result{&world->flocks[candidate.flock_index].members[candidate.member_index]};
        if (result == &eyes) continue;

        ++element_index;
        return result;
    }
    return nullptr;
}

bool QuadSearch::intersects(QuadTree *tree) {
    for (auto& point : approximation_rect) {
        if (tree->top_left.x >= point.x && tree->top_left.y >= point.y && tree->bottom_right.x <= point.x && tree->bottom_right.y <= point.y)
            return true;
    }
    sf::Vector2f v1{world->position_difference(tree->top_left, eyes.position)};
    sf::Vector2f v2{world->position_difference(tree->bottom_right, eyes.position)};

    for (auto& point : {v1, v2, sf::Vector2f(v1.x, v2.y), sf::Vector2f(v2.x, v1.y)}) {
        sf::Vector2f vec{point ^ ~eyes.orientation};
        if (vec.x <= fov.range && x_min <= vec.x && vec.y <= dy && -dy <= vec.y)
            return true;
    }
    return false;
}

bool QuadSearch::propagate_to_children() {
    for (auto& child : current_node->children) {
        if (child.intersects_fov(eyes.position, eyes.orientation, sq_range, fov.cos_fov, world)) {
            current_node = &child;
            return true;
        }
    }
    return false;
}


FlockMember *QuadSearch::next() {
    while (true) {
        FlockMember * result{process_elements()};
        if (result)
            return result;
        element_index = 0;
        if (propagate_to_children())
            // current has changed
            continue;
        while (current_node->parent) {
            QuadTree * parent{current_node->parent};
            // try to access next sibling
            ++current_node;

            // out of siblings
            if (current_node == &4[parent->children.data()]) {
                // pop the "stack"
                current_node = parent;
                continue;
            }
            if (current_node->intersects_fov(eyes.position, eyes.orientation, sq_range, fov.cos_fov, world))
                // current is valid
                break;
        }
        if (!current_node->parent)
            // back to the root, finished
            return nullptr;
    }
}
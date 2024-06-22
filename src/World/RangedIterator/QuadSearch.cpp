
//


#include "QuadSearch.h"
#include "utils.h"
#include <functional>

QuadSearch::QuadSearch(FiniteWorld *world, Animal animal, const FlockMember &eyes, FOV fov)
    : NeighborSearch(world, animal, eyes, fov), current_node{&world->tree},
    approximation_top_left(eyes.position - sf::Vector2f{fov.range, fov.range}),
    approximation_bottom_right(eyes.position + sf::Vector2f{fov.range, fov.range})
{
    std::invoke(world->validator, world, approximation_top_left);
    std::invoke(world->validator, world, approximation_bottom_right);
}


FlockMember *QuadSearch::process_elements() {
    for (; element_index < QuadTreeSize; ++element_index) {
        QuadTreeElement const& candidate{current_node->elements[element_index]};
        if (candidate.animal != animal) continue;
        if (!test(candidate.position)) continue;

        FlockMember * result{&world->flocks[candidate.flock_index].members[candidate.member_index]};
        if (result == &eyes) continue;

        ++element_index;
        return result;
    }
    return nullptr;
}

bool QuadSearch::intersects(QuadTree const *tree) const {
    sf::Vector2f inter_top_left;
    inter_top_left.x = (approximation_top_left.x > approximation_bottom_right.x || tree->top_left.x > approximation_top_left.x) ? tree->top_left.x : approximation_top_left.x;
    inter_top_left.y = (approximation_top_left.y > approximation_bottom_right.y || tree->top_left.y > approximation_top_left.y) ? tree->top_left.y : approximation_top_left.y;

    sf::Vector2f inter_bottom_right;
    inter_bottom_right.x = (approximation_top_left.x > approximation_bottom_right.x || tree->bottom_right.x < approximation_bottom_right.x) ? tree->bottom_right.x : approximation_bottom_right.x;
    inter_bottom_right.y = (approximation_top_left.y > approximation_bottom_right.y || tree->bottom_right.y < approximation_bottom_right.y) ? tree->bottom_right.y : approximation_bottom_right.y;

    return inter_top_left.x < inter_bottom_right.x && inter_top_left.y < inter_bottom_right.y;
}

bool QuadSearch::propagate_to_children() {
    for (auto& child : current_node->children) {
        if (intersects(&child)) {
            current_node = &child;
            return true;
        }
    }
    return false;
}


FlockMember *QuadSearch::next() {
    while (true) {
        if (FlockMember * result{process_elements()}; result)
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
            if (current_node == parent->children.data() + 4) {
                // pop the "stack"
                current_node = parent;
                continue;
            }
            if (intersects(current_node))
                // current is valid
                break;
        }
        if (!current_node->parent)
            // back to the root, finished
            return nullptr;
    }
}
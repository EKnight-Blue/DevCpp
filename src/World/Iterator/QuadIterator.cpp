#include "QuadIterator.h"

QuadIterator::QuadIterator(FiniteWorld const *world, Animal animal, FlockMember const &member, float range, float cos_fov) :
        NeighborIterator(world, animal, member, range, cos_fov), current{&world->tree}, range{range} {

}

/**
 * Test members that are in the current QuadTree
 * @return
 */
FlockMember const * QuadIterator::process_elements() {
    FlockMember const * result{nullptr};
    for (; element_index < current->cnt; ++element_index) {
        auto &element = current->elements[element_index];
        if (element.animal != animal)
            continue;
        if (test(element.position)) {
            result = world->flocks[element.flock_index].members.data() + element.member_index;
            if (result == &member) {
                continue;
            }
            ++element_index;
            return result;
        }
    }
    return result;
}

/**
 * Try to look in the current tree's children
 * @return
 */
bool QuadIterator::propagate_to_children() {
    for (auto& child : current->children) {
        if (child.intersects_fov(member.position, member.orientation, sq_range, cos_fov, world)) {
            current = &child;
            return true;
        }
    }
    return false;
}

FlockMember const * QuadIterator::next() {
    while (true) {
        FlockMember const * result{process_elements()};
        if (result)
            return result;
        element_index = 0;
        if (propagate_to_children())
            // current has changed
            continue;
        while (current->parent) {
            QuadTree * parent{current->parent};
            // try to access next sibling
            ++current;

            // out of siblings
            if (current == &4[parent->children.data()]) {
                // pop the "stack"
                current = parent;
                continue;
            }
            if (current->intersects_fov(member.position, member.orientation, sq_range, cos_fov, world))
                // current is valid
                break;
        }
        if (!current->parent)
            // back to the root, finished
            return nullptr;
    }
}

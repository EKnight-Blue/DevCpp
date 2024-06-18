#include "QuadIterator.h"

QuadIterator::QuadIterator(FiniteWorld *world, Animal animal, FlockMember const &member, float range, float cos_fov) :
        NeighborIterator(world, animal, member, range, cos_fov), current{&world->tree}, range{range} {

}

FlockMember * QuadIterator::process_elements() {
    FlockMember * result{nullptr};
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

bool QuadIterator::propagate_to_children() {
    for (auto& child : current->children) {
        if (child.intersects_fov(member.position, member.orientation, sq_range, cos_fov, world)) {
            current = &child;
            return true;
        }
    }
    return false;
}

FlockMember* QuadIterator::next() {
    while (true) {
        FlockMember * result{process_elements()};
        if (result)
            return result;
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
            return nullptr;
    }
}

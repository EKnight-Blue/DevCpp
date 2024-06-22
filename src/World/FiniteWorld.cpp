#include "FiniteWorld.h"
#include "World/Iterator/QuadIterator.h"
#include "World/NewIterator/QuadSearch.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include <functional>

/**
 * Make up for weird topologies
 * @param v1
 * @param v2
 * @return
 */
sf::Vector2f FiniteWorld::position_difference(sf::Vector2f const &v1, sf::Vector2f const &v2) const {
    sf::Vector2f v{v1.x - v2.x, v1.y - v2.y};
    if (is_toroidal) {
        // wrap around if possible
        if (v.x < -width * .5f)
            v.x += width;
        else if (v.x > width * .5f)
            v.x -= width;
        if (v.y < - height * .5f)
            v.y += height;
        else if (v.y > height * .5f)
            v.y -= height;
    }
    return v;
}

/**
 * Stick objects to the walls if they go out
 * @param point
 */
void FiniteWorld::validate_normal_position(sf::Vector2f &point) const {
    point.x = (0.f > point.x) ? 0.f : point.x;
    point.y = (0.f > point.y) ? 0.f : point.y;
    point.x = (width < point.x) ? width : point.x;
    point.y = (height < point.y) ? height : point.y;
}

/**
 * Teleport objects to the "other side" if the go out
 * @param point
 */
void FiniteWorld::validate_toroidal_position(sf::Vector2f &point) const {
    if (point.x < 0.f)
        point.x += width;
    else if (point.x > width)
        point.x -= width;
    if (point.y < 0.f)
        point.y += height;
    else if (point.y > height)
        point.y -= height;
}

/**
 * Process position changes for every flock, rebuild the quad tree
 * @param delta_time
 */
void FiniteWorld::update(sf::Time delta_time) {
    tree.reset(width, height);
    for (uint16_t flock_index{0}; flock_index < flocks.size(); ++flock_index) {
        Flock& flock{flocks[flock_index]};
        for (uint16_t member_index{0}; member_index < flock.members.size(); ++member_index) {
            FlockMember& member{flock.members[member_index]};
            flock.update(delta_time, member);
            std::invoke(validator, this, member.position);
            tree.insert({
                .animal=flock.animal,
                .position=member.position,
                .flock_index=flock_index,
                .member_index=member_index,
            });
        }

    }
}

std::unique_ptr<NeighborIterator> FiniteWorld::make_neighbor_iterator(Animal animal, FlockMember const &member, float range, float cos_fov) const {
    return std::make_unique<QuadIterator>(this, animal, member, range, cos_fov);
}

NeighborRange FiniteWorld::neighbors(Animal animal, const FlockMember &eyes, AtomicBehavior::Parameters::DetectionFOV fov) {
    return NeighborRange(std::make_unique<QuadSearch>(this, animal, eyes, fov));
}


void FiniteWorld::make_sub_gui() {
    World::make_sub_gui();
    ImGui::InputFloat("Width", &width);
    ImGui::InputFloat("Height", &height);
    if (ImGui::Checkbox("Toroidal World", &is_toroidal)) {
        static std::array<void (FiniteWorld::*)(sf::Vector2f&) const, 2> validators{
                &FiniteWorld::validate_normal_position,
                &FiniteWorld::validate_toroidal_position
        };
        validator = validators[is_toroidal];
    }
    ImGui::Checkbox("Show QuadTree", &show_tree);
}

void FiniteWorld::draw(sf::RenderTarget &target) {
    World::draw(target);
    if (show_tree)
        tree.draw(target);
}
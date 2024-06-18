#include "FiniteWorld.h"
#include "Iterator/QuadIterator.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include <functional>
#include "Iterator/NaiveIterator.h"

sf::Vector2f FiniteWorld::position_difference(sf::Vector2f const &v1, sf::Vector2f const &v2) const {
    sf::Vector2f v{v1.x - v2.x, v1.y - v2.y};
    if (is_toroidal) {
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

void FiniteWorld::validate_normal_position(sf::Vector2f &point) const {
    point.x = (0.f > point.x) ? 0.f : point.x;
    point.y = (0.f > point.y) ? 0.f : point.y;
    point.x = (width < point.x) ? width : point.x;
    point.y = (height < point.y) ? height : point.y;
}

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


void FiniteWorld::update(sf::Time delta_time) {
    static std::array<void (FiniteWorld::*)(sf::Vector2f&) const, 2> validators{
        &FiniteWorld::validate_normal_position,
        &FiniteWorld::validate_toroidal_position
    };
    auto validator = validators[is_toroidal];
    tree.reset(width, height);
    for (uint32_t flock_index{0}; flock_index < flocks.size(); ++flock_index) {
        Flock& flock{flocks[flock_index]};
        for (uint32_t member_index{0}; member_index < flock.members.size(); ++member_index) {
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
    return std::make_unique<NaiveIterator>(this, animal, member, range, cos_fov);
}


void FiniteWorld::make_sub_gui() {
    World::make_sub_gui();
    ImGui::InputFloat("Width", &width);
    ImGui::InputFloat("Height", &height);
    ImGui::Checkbox("Toroidal World", &is_toroidal);
    ImGui::Checkbox("Show QuadTree", &show_tree);
}

void FiniteWorld::draw(sf::RenderTarget &target) {
    World::draw(target);
    if (show_tree)
        tree.draw(target);
}
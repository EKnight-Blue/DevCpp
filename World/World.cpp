#include "World.h"
#include "utils.h"
#include "Iterator/NaiveIterator.h"
#include "imgui-SFML.h"
#include "imgui.h"

std::unique_ptr<NeighborIterator> World::make_neighbor_iterator(Animal animal, FlockMember const &member, float range, float cos_fov) {
    return std::make_unique<NaiveIterator>(this, animal, member, range, cos_fov);
}


void World::update(sf::Time delta_time) {
    for (auto& flock : flocks) {
        for (auto& member : flock.members){
            flock.update(delta_time, member);
        }
    }
}


void World::make_gui() {
    if (ImGui::TreeNode("World")) {
        make_sub_gui();
        ImGui::TreePop();
    }
}

void World::make_sub_gui() {
    if (ImGui::TreeNode("Flocks")) {
        for (auto &flock: flocks) {
            flock.make_gui();
        }
        if (ImGui::Button("Add Flock")) {}
        ImGui::TreePop();
    }
}

void World::draw(sf::RenderTarget &target) {
    for (auto& flock : flocks) {
        flock.draw(target);
    }
}
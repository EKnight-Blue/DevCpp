#include "World.h"
#include "utils.h"
#include "World/RangedIterator/NaiveSearch.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include "Behavior/CombinedBehavior.h"

std::unique_ptr< NeighborSearch> World::make_neighbor_iterator(Animal animal, FlockMember const &member, AtomicBehavior::Parameters::DetectionFOV fov) {
    return std::make_unique<NaiveSearch>(this, animal, member, fov);
}

/**
 * Propagate update on the flocks
 * @param delta_time
 */
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

void World::new_flock() {
    if (!ImGui::TreeNode("New Flock"))
        return;

    static Animal animal{Animal::Squirrel};
    if (ImGui::BeginListBox("Animal")) {
        for (size_t index{0}; index < static_cast<size_t>(Animal::Count); ++index) {
            if(ImGui::Selectable(animal_names[index].data(), static_cast<Animal>(index) == animal)) {
                animal = static_cast<Animal>(index);
            }
        }
        ImGui::EndListBox();
    }
    static int count{100};
    ImGui::InputInt("Number of members", &count);
    if (count < 0) {
        count = 0;
    }
    static float size;
    static float max_speed;
    static float max_force;
    ImGui::InputFloat("Size", &size);
    ImGui::InputFloat("Max Speed", &max_speed);
    ImGui::InputFloat("Max Force", &max_force);

    if (ImGui::Button("Create")) {
        flocks.emplace_back(
                animal, size, count, max_speed, max_force
        );
        behaviors.emplace_back();
    }
    ImGui::TreePop();
}

void World::make_sub_gui() {
    if (ImGui::TreeNode("Flocks")) {
        int id{0};
        for (size_t index{0}; index < flocks.size(); ++index) {
            ImGui::PushID(id);
            ++id;
            flocks[index].make_gui();
            behaviors[index].make_gui();
            ImGui::PopID();
        }
        ImGui::PushID(id);
        ++id;
        new_flock();
        ImGui::TreePop();
        ImGui::PopID();
    }
}

void World::draw(sf::RenderTarget &target) {
    for (auto& flock : flocks) {
        flock.draw(target);
    }
}

void World::process_behaviors() {
    for (size_t index{0}; index < flocks.size(); ++index) {
        behaviors[index].compute(flocks[index], this);
    }
}

NeighborRange World::neighbors(Animal animal, const FlockMember &eyes, AtomicBehavior::Parameters::DetectionFOV fov) {
    return NeighborRange(std::make_unique<NaiveSearch>(this, animal, eyes, fov));
}

NeighborGenerator World::co_neighbors(Animal animal, FlockMember const &eyes, AtomicBehavior::Parameters::DetectionFOV fov) {
    return NaiveSearch(this, animal, eyes, fov).co_neighbors();
}

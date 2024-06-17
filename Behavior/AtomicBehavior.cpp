#include "AtomicBehavior.h"
#include "utils.h"
#include <functional>

#include "imgui-SFML.h"
#include "imgui.h"
#include "World/Iterator/NeighborIterator.h"


void AtomicBehavior::update_values(const Parameters &new_data, const float c) {
    parameters = new_data;
    coefficient = c;
}


sf::Vector2f sub_seek(Flock const& flock, FlockMember const& member, sf::Vector2f desired_velocity) {
    float const old_mag = magnitude(desired_velocity);
    if (old_mag == 0.f) {
        return -member.speed * member.orientation;
    } else {
        return desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation;
    }
}


void AtomicBehavior::seek(Flock const& flock, FlockMember &member, World const *world) const {
    sf::Vector2f desired_velocity = world->position_difference(parameters.seek_flee.target, member.position);
    member.force += coefficient * sub_seek(flock, member, desired_velocity);
}


void AtomicBehavior::flee(Flock const& flock, FlockMember &member, World const *world) const {
    sf::Vector2f desired_velocity = world->position_difference(member.position, parameters.seek_flee.target);
    member.force += coefficient * sub_seek(flock, member, desired_velocity);
}

void AtomicBehavior::arrival(Flock const& flock, FlockMember &member, World const *world) const {
    sf::Vector2f desired_velocity = world->position_difference(parameters.arrival.target, member.position);
    float const old_mag = magnitude(desired_velocity);
    if (old_mag < parameters.arrival.range) {
        member.force += coefficient * (desired_velocity * (flock.max_speed / parameters.arrival.range) - member.speed * member.orientation);
    } else {
        member.force += coefficient * (desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation);
    }
}


void AtomicBehavior::cohesion(Flock const& flock, FlockMember &member, World const *world) const {
    sf::Vector2f desired_velocity{0., 0.};
    size_t neighbor_cnt = 0;

    auto it = world->make_neighbor_iterator(flock.animal, member, parameters.cas.range, parameters.cas.cos_fov);
    FlockMember neighbor;
    while (it->next(neighbor)) {
        ++neighbor_cnt;
        desired_velocity += world->position_difference(neighbor.position, member.position);
    }
    if (!neighbor_cnt) {
        return;
    }
    member.force += coefficient * sub_seek(flock, member, desired_velocity / static_cast<float>(neighbor_cnt));
}


void AtomicBehavior::alignment(Flock const& flock, FlockMember &member, World const *world) const {
    sf::Vector2f desired_velocity{0., 0.};
    size_t neighbor_cnt = 0;

    auto it = world->make_neighbor_iterator(flock.animal, member, parameters.cas.range, parameters.cas.cos_fov);
    FlockMember neighbor;
    while (it->next(neighbor)) {
        desired_velocity += neighbor.speed * neighbor.orientation;
        ++neighbor_cnt;
    }
    if (!neighbor_cnt) {
        // 0., 0.
        return;
    }
    member.force += coefficient * sub_seek(flock, member, desired_velocity / static_cast<float>(neighbor_cnt));
}


void AtomicBehavior::separation(Flock const& flock, FlockMember &member, World const *world) const {
    sf::Vector2f force{0., 0.};
    auto it = world->make_neighbor_iterator(flock.animal, member, parameters.cas.range, parameters.cas.cos_fov);
    FlockMember neighbor;
    while (it->next(neighbor)) {
        sf::Vector2f vec = world->position_difference(member.position, neighbor.position);
        float sq_mag = sq_magnitude(vec);
        if (0.f != sq_mag)
            force += vec / sq_mag;
    }
    member.force += (coefficient * parameters.cas.range) * force;
}


void AtomicBehavior::wander([[maybe_unused]] Flock const &flock, FlockMember &member, [[maybe_unused]] World const *world) const {
    member.last_wander_angle += (2.f * random_float() - 1.f) * parameters.wander.displacement_amplitude;
    member.force += coefficient * member.orientation * parameters.wander.sphere_dist + parameters.wander.sphere_radius * sf::Vector2f{cosf(member.last_wander_angle), sinf(member.last_wander_angle)};
}


void AtomicBehavior::pursuit([[maybe_unused]] Flock const &flock, [[maybe_unused]] FlockMember &member, [[maybe_unused]] World const *world) const {
    auto it = world->make_neighbor_iterator(parameters.pursuit_evasion.animal, member, parameters.pursuit_evasion.fov.range, parameters.pursuit_evasion.fov.cos_fov);
    FlockMember neighbor;
    FlockMember best;
    float best_dist{HUGE_VALF};
    while (it->next(neighbor)) {
        float dist{magnitude(world->position_difference(neighbor.position, member.position))};
        if (dist < best_dist){
            best_dist = dist;
            best = neighbor;
        }
    }
    if (best_dist == HUGE_VALF)
        return;
    sf::Vector2f vec = world->position_difference(
    best.position + (best.speed * parameters.pursuit_evasion.prediction_time) * best.orientation,
    member.position
    );
    member.force += coefficient * sub_seek(flock, member, vec);
}


void AtomicBehavior::evasion([[maybe_unused]] Flock const &flock, [[maybe_unused]] FlockMember &member, [[maybe_unused]]World const *world) const {
    auto it = world->make_neighbor_iterator(parameters.pursuit_evasion.animal, member, parameters.pursuit_evasion.fov.range, parameters.pursuit_evasion.fov.cos_fov);
    FlockMember neighbor;
    FlockMember best;
    float best_dist{HUGE_VALF};
    while (it->next(neighbor)) {
        float dist{magnitude(world->position_difference(neighbor.position, member.position))};
        if (dist < best_dist){
            best_dist = dist;
            best = neighbor;
        }
    }
    if (best_dist == HUGE_VALF)
        return;
    sf::Vector2f vec = world->position_difference(
            member.position,
            best.position + (best.speed * parameters.pursuit_evasion.prediction_time) * best.orientation
    );
    member.force += coefficient * sub_seek(flock, member, vec);
}

void AtomicBehavior::compute_body(Flock& flock, FlockMember& member, World * world) {
    static std::array<BehaviorMethod, static_cast<size_t>(Type::Count)> behaviors{
        &AtomicBehavior::seek,
        &AtomicBehavior::flee,
        &AtomicBehavior::arrival,
        &AtomicBehavior::cohesion,
        &AtomicBehavior::alignment,
        &AtomicBehavior::separation,
        &AtomicBehavior::wander,
        &AtomicBehavior::pursuit,
        &AtomicBehavior::evasion
    };
    BehaviorMethod const method = behaviors[static_cast<size_t>(type)];
    std::invoke(method, this, flock, member, world);
}



void AtomicBehavior::seek_flee_gui() {
    ImGui::InputFloat2("Target", std::bit_cast<float *>(&parameters.seek_flee.target));
}

void AtomicBehavior::arrival_gui() {
    ImGui::InputFloat2("Target", std::bit_cast<float *>(&parameters.arrival.target));
    ImGui::InputFloat("Slowdown range", &parameters.arrival.range);
}

void AtomicBehavior::cas_gui() {
    ImGui::InputFloat("Detection range", &parameters.cas.range);
    ImGui::InputFloat("Cos Field of view", &parameters.cas.cos_fov);
}

void AtomicBehavior::wander_gui() {
    ImGui::InputFloat("Sphere distance", &parameters.wander.sphere_dist);
    ImGui::InputFloat("Sphere radius", &parameters.wander.sphere_radius);
    ImGui::InputFloat("Increment amplitude", &parameters.wander.displacement_amplitude);
}

void AtomicBehavior::pursuit_evasion_gui() {
    if (ImGui::BeginListBox("Target animal")) {
        for (size_t index{0}; index < static_cast<size_t>(Animal::Count); ++index) {
            if(ImGui::Selectable(animal_names[index].data(), static_cast<Animal>(index) == parameters.pursuit_evasion.animal)) {
                parameters.pursuit_evasion.animal = static_cast<Animal>(index);
            }
        }
        ImGui::EndListBox();
    }
    ImGui::InputFloat("Detection range", &parameters.pursuit_evasion.fov.range);
    ImGui::InputFloat("Cos Field of view", &parameters.pursuit_evasion.fov.cos_fov);
}

void AtomicBehavior::make_gui() {
    static std::array<BehaviorGui, static_cast<size_t>(Type::Count)> const guis{
        &AtomicBehavior::seek_flee_gui,
        &AtomicBehavior::seek_flee_gui,
        &AtomicBehavior::arrival_gui,
        &AtomicBehavior::cas_gui,
        &AtomicBehavior::cas_gui,
        &AtomicBehavior::cas_gui,
        &AtomicBehavior::wander_gui,
        &AtomicBehavior::pursuit_evasion_gui,
        &AtomicBehavior::pursuit_evasion_gui
    };
    static std::array<std::string, static_cast<size_t>(Type::Count)> const names{
        "Seek",
        "Flee",
        "Arrival",
        "Cohesion",
        "Alignment",
        "Separation",
        "Wander",
        "Pursuit",
        "Evasion"
    };
    if(ImGui::TreeNode(names[static_cast<size_t>(type)].data())) {
        ImGui::InputFloat("Coefficient", &coefficient);
        std::invoke(guis[static_cast<size_t>(type)], this);
        ImGui::TreePop();
    }

}
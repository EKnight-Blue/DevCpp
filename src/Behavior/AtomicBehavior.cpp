#include "AtomicBehavior.h"
#include "utils.h"
#include <functional>
#include <bit>

#include "imgui-SFML.h"
#include "imgui.h"
#include "World/World.h"
#include "World/RangedIterator/NeighborSearch.h"

/**
 * Generic setter
 * @param new_data Parameters set according to the Type
 * @param new_coefficient
 */
void AtomicBehavior::update_values(Parameters const &new_data, const float new_coefficient) {
    parameters = new_data;
    coefficient = new_coefficient;
}

/**
 * Internal sub-function that computes seek behavior;
 * It is used by other behaviors
 *
 * @param flock Flock holding the current member
 * @param member Member for which the behavior is being computed
 * @param desired_velocity
 * @return
 */
sf::Vector2f sub_seek(Flock const& flock, FlockMember const& member, sf::Vector2f desired_velocity) {
    float const old_mag = magnitude(desired_velocity);
    if (old_mag == 0.f) {
        return -member.speed * member.orientation;
    } else {
        return desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation;
    }
}

/**
 * Constantly drive towards a point (at max speed)
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::seek(Flock const& flock, FlockMember &member, World *world) const {
    // try to go to the target
    sf::Vector2f desired_velocity = world->position_difference(parameters.seek_flee.target, member.position);
    member.force += coefficient * sub_seek(flock, member, desired_velocity);
}

/**
 * Constantly drive away from a point
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::flee(Flock const& flock, FlockMember &member, World *world) const {
    // try to go away from the target
    sf::Vector2f desired_velocity = world->position_difference(member.position, parameters.seek_flee.target);
    member.force += coefficient * sub_seek(flock, member, desired_velocity);
}

/**
 * Constantly drive towards a point (and slow down when close)
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::arrival(Flock const& flock, FlockMember &member, World *world) const {
    // try to go to the target
    sf::Vector2f desired_velocity = world->position_difference(parameters.arrival.target, member.position);
    float const old_mag = magnitude(desired_velocity);
    if (old_mag < parameters.arrival.range) {
        // arrive at reasonable speed
        member.force += coefficient * (desired_velocity * (flock.max_speed / parameters.arrival.range) - member.speed * member.orientation);
    } else {
        // rush the target
        member.force += coefficient * (desired_velocity * (flock.max_speed / old_mag) - member.speed * member.orientation);
    }
}

/**
 * Make members stay together
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::cohesion(Flock const& flock, FlockMember &member, World *world) const {
    sf::Vector2f desired_velocity{0., 0.};
    size_t neighbor_cnt = 0;


    for (FlockMember const& neighbor : world->neighbors(flock.animal, member, parameters.cas)) {
        ++neighbor_cnt;
        // average position of neighbors
        desired_velocity += world->position_difference(neighbor.position, member.position);
    }
    if (!neighbor_cnt) {
        return;
    }
    // try to go to the average neighbor position
    member.force += coefficient * sub_seek(flock, member, desired_velocity / static_cast<float>(neighbor_cnt));
}

/**
 * Make members want to have the same speed as their mates
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::alignment(Flock const& flock, FlockMember &member, World *world) const {
    sf::Vector2f desired_velocity{0., 0.};
    size_t neighbor_cnt = 0;


    for (FlockMember const& neighbor : world->neighbors(flock.animal, member, parameters.cas)) {
        // average speed of neighbors
        desired_velocity += neighbor.speed * neighbor.orientation;
        ++neighbor_cnt;
    }
    if (!neighbor_cnt) {
        return;
    }
    member.force += coefficient * sub_seek(flock, member, desired_velocity / static_cast<float>(neighbor_cnt));
}

/**
 * Keep member not too close to one another
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::separation(Flock const& flock, FlockMember &member, World *world) const {
    sf::Vector2f force{0., 0.};
    for (FlockMember const& neighbor : world->neighbors(flock.animal, member, parameters.cas)) {
        sf::Vector2f vec = world->position_difference(member.position, neighbor.position);
        float sq_mag = sq_magnitude(vec);
        if (0.f != sq_mag)
            // 1/r repulsion
            force += vec / sq_mag;
    }
    member.force += (coefficient * parameters.cas.range) * force;
}

/**
 * Random walk, chooses a random point on a circle placed in front of the member
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::wander([[maybe_unused]] Flock const &flock, FlockMember &member, [[maybe_unused]] World *world) const {
    member.last_wander_angle += (2.f * random_float() - 1.f) * parameters.wander.displacement_amplitude;
    member.force += coefficient * member.orientation * parameters.wander.sphere_dist + parameters.wander.sphere_radius * sf::Vector2f{cosf(member.last_wander_angle), sinf(member.last_wander_angle)};
}

/**
 * Follow other animals of a certain type
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::pursuit([[maybe_unused]] Flock const &flock, [[maybe_unused]] FlockMember &member, [[maybe_unused]] World *world) const {

    FlockMember const * best{nullptr};
    float best_dist{HUGE_VALF};
    // chose the closest member of the targeted animal

    for (FlockMember const& neighbor : world->neighbors(parameters.pursuit_evasion.animal, member, parameters.pursuit_evasion.fov)) {
        float dist{magnitude(world->position_difference(neighbor.position, member.position))};
        if (dist < best_dist){
            best_dist = dist;
            best = &neighbor;
        }
    }
    if (!best)
        return;
    // steer towards the animal
    sf::Vector2f vec = world->position_difference(
    best->position + (best->speed * parameters.pursuit_evasion.prediction_time) * best->orientation,
    member.position
    );
    member.force += coefficient * sub_seek(flock, member, vec);
}

/**
 * Flee other animals of a certain type
 * @param flock
 * @param member
 * @param world
 */
void AtomicBehavior::evasion([[maybe_unused]] Flock const &flock, [[maybe_unused]] FlockMember &member, [[maybe_unused]]World *world) const {

    FlockMember const *best{nullptr};
    float best_dist{HUGE_VALF};
    // chose the closest member of the targeted animal
    for (FlockMember const& neighbor : world->neighbors(parameters.pursuit_evasion.animal, member, parameters.pursuit_evasion.fov)) {
        float dist{magnitude(world->position_difference(neighbor.position, member.position))};
        if (dist < best_dist){
            best_dist = dist;
            best = &neighbor;
        }
    }
    if (!best)
        return;
    // steer away from the animal
    sf::Vector2f vec = world->position_difference(
    member.position,
    best->position + (best->speed * parameters.pursuit_evasion.prediction_time) * best->orientation
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
    // select the method that matches Type
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
        // select the method that matches Type
        std::invoke(guis[static_cast<size_t>(type)], this);
        ImGui::TreePop();
    }

}
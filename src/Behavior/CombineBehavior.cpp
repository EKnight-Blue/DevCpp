#include "CombinedBehavior.h"
#include "imgui-SFML.h"
#include "imgui.h"

/**
 * Accumulate the effects of every composing atomic behavior
 * @param flock
 * @param member
 * @param world
 */
void CombinedBehavior::compute_body(Flock& flock, FlockMember& member, World const * world) {
    for (auto& steering : behaviors) {
        steering.compute_body(flock, member, world);
    }
}

/**
 * Add a new atomic behavior
 * @param behavior
 * @param data
 * @param c
 */
void CombinedBehavior::add(AtomicBehavior::Type behavior, AtomicBehavior::Parameters const& data, const float c) {
    behaviors.emplace_back(behavior, data, c);
}

void CombinedBehavior::make_gui() {
    if (ImGui::TreeNode("Combined Behavior")) {
        for (auto& behavior : behaviors) {
            behavior.make_gui();
        }



        ImGui::TreePop();
    }
}
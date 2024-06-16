#include "CombinedBehavior.h"
#include "imgui-SFML.h"

void CombinedBehavior::compute_body(Flock& flock, FlockMember& member, World * world) {
    for (auto& steering : behaviors) {
        steering.compute_body(flock, member, world);
    }
}

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
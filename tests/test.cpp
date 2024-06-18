#include "gtest/gtest.h"
#include "Creatures/Flock.h"
#include "World/QuadTree.h"
#include "utils.h"
#include "World/FiniteWorld.h"
#include <memory>
#include "World/Iterator/QuadIterator.h"
#include <iostream>

// So that we have at least one test
TEST(Hey, JASON) { ASSERT_TRUE(true); }

TEST(arbre_quaternaire, basique) {
    QuadTree arbre =
        QuadTree{sf::Vector2f{0, 0}, sf::Vector2f{100, 100}, nullptr};
    arbre.insert({
        .animal = Animal::Rabbit,
        .position = sf::Vector2f{0, 0},
        .flock_index = 0,
        .member_index = 0,
    });
    EXPECT_EQ(arbre.cnt, 1);
}


TEST(Tree, Iteration) {
    constexpr float W{800.f};
    FiniteWorld world{W, W};
    constexpr int32_t nb{64};
    world.flocks.emplace_back(Animal::Bird, 20, nb, 0.f, 0.f);

    constexpr float increment{TWO_PI / static_cast<float>(nb)};
    float angle{0.f};
    uint16_t index{0};
    for (auto& member : world.flocks[0].members) {
        member.position.x  = W * 0.5f + 0.25f * W * cosf(angle);
        member.position.y  = W * 0.5f + 0.25f * W * sinf(angle);
        world.tree.insert({
           .animal = Animal::Bird,
           .position = member.position,
           .flock_index = 0,
           .member_index = index++
        });
        angle = static_cast<float>(index) * increment;
    }
    FlockMember observer{};
    observer.position={W * 0.5f, W * 0.5f};
    angle = increment / 2.f;
    for (int include{0}; include < nb / 2; include++) {
        std::cout << "Include = " << include << std::endl;

        std::unique_ptr<NeighborIterator> it = world.make_neighbor_iterator(Animal::Bird, observer, W * 0.5f, cosf(angle));
        int cnt = 0;
        for (FlockMember const *member{it->next()}; member; member = it->next()) {
            ++cnt;
            std::cout << " " << std::distance(const_cast<FlockMember const *>(world.flocks[0].members.data()), member);
        }
        std::cout << std::endl;
        EXPECT_EQ(cnt, 2 * include + 1);
        angle = increment * (static_cast<float>(include+1) + .5f);
    }
}

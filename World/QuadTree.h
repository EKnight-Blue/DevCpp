
//

#ifndef DEV_CPP_QUADTREE_H
#define DEV_CPP_QUADTREE_H
#include <vector>
#include <array>
#include "SFML/System/Vector2.hpp"
#include <cstdint>
#include "Creatures/Animals.h"

constexpr size_t QuadTreeSize = 10;

struct QuadTreeElement{
    Animal animal;
    sf::Vector2f position;
    uint32_t flock_index;
    uint32_t member_index;
};

class World;
class QuadTree{
public:
    QuadTree(sf::Vector2f top_left, sf::Vector2f bottom_right, QuadTree * parent);
    void insert(QuadTreeElement const &element);
    void reset();

    sf::Vector2f const top_left;
    sf::Vector2f const bottom_right;
    sf::Vector2f const center;
    size_t cnt{0};
    std::vector<QuadTree> children{};
    std::array<QuadTreeElement, QuadTreeSize> elements;
    QuadTree * parent;
private:
    [[nodiscard]] size_t chose_quadrant(sf::Vector2f const& point) const;
    void divide();
};


#endif //DEV_CPP_QUADTREE_H

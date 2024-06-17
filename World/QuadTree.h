
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

    bool intersects_fov(sf::Vector2f const &point, sf::Vector2f const& orientation, float sq_radius, float cos_fov, World const * world) const;

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
    bool line_line(sf::Vector2f const& point, sf::Vector2f const & director, float sq_radius, World const * const world) const;
    bool arc_line(sf::Vector2f const& point, sf::Vector2f const& orientation, float sq_radius, float cos_fov, World const * const world) const;
    bool completely_inside(sf::Vector2f const& point, sf::Vector2f const& orientation, float sq_radius, float cos_fov, World const * const world) const;
};


#endif //DEV_CPP_QUADTREE_H

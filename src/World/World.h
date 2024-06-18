#ifndef DEV_CPP_WORLD_H
#define DEV_CPP_WORLD_H

#include "SFML/System/Vector2.hpp"
#include "Creatures/Flock.h"
#include <memory>
#include "QuadTree.h"
#include "Behavior/CombinedBehavior.h"

class NeighborIterator;
class World{
public:
    virtual ~World() = default;
    [[nodiscard]] virtual sf::Vector2f position_difference(sf::Vector2f const& v1, sf::Vector2f const & v2) const {return v1 - v2;};
    virtual void update(sf::Time delta_time);
    void make_gui();
    virtual void make_sub_gui();
    void process_behaviors();

    virtual void draw(sf::RenderTarget &target);

    [[nodiscard]] virtual std::unique_ptr<NeighborIterator> make_neighbor_iterator(Animal animal, FlockMember const &member, float range, float cos_fov) const;
    std::vector<Flock> flocks{};
    std::vector<CombinedBehavior> behaviors;
    friend NeighborIterator;

    void new_flock();
};



#endif //DEV_CPP_WORLD_H
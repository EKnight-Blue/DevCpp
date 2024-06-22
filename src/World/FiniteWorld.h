#ifndef BOIDSFML_FINITEWORLD_H
#define BOIDSFML_FINITEWORLD_H
#include "World.h"

class FiniteWorld : public World{
public:
    FiniteWorld(float width, float height) : tree{{0.f, 0.f}, {width, height}, nullptr}, width{width}, height{height} {};
    [[nodiscard]] sf::Vector2f position_difference(sf::Vector2f const& v1, sf::Vector2f const & v2) const override;
    void update(sf::Time delta_time) override;
    [[nodiscard]] virtual std::unique_ptr<NeighborSearch> make_neighbor_iterator(Animal animal, FlockMember const &member, AtomicBehavior::Parameters::DetectionFOV fov);
    [[nodiscard]] NeighborRange neighbors(Animal animal, FlockMember const &eyes, AtomicBehavior::Parameters::DetectionFOV fov) override;

    void make_sub_gui() override;
    void draw(sf::RenderTarget & target) override;

    QuadTree tree;
    using Validator = void (FiniteWorld::*)(sf::Vector2f&) const;
    Validator validator{&FiniteWorld::validate_toroidal_position};
private:
    float width;
    float height;
    bool is_toroidal{true};
    bool show_tree{true};
    friend class QuadIterator;

    void validate_toroidal_position(sf::Vector2f & point) const;
    void validate_normal_position(sf::Vector2f & point) const;
    friend class QuadSearch;

};



#endif //BOIDSFML_FINITEWORLD_H

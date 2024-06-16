#ifndef DEV_CPP_WORLD_H
#define DEV_CPP_WORLD_H

#include "SFML/System/Vector2.hpp"
#include "Creatures/Flock.h"
#include <memory>

class World{
public:
    virtual ~World() = default;
    [[nodiscard]] virtual sf::Vector2f position_difference(sf::Vector2f const& v1, sf::Vector2f const & v2) const {return v1 - v2;};
    virtual void update(sf::Time delta_time);

#pragma region Iterator
    class NeighborIterator{
    public:
        NeighborIterator(World const * world, Animal animal, FlockMember const& member, float range, float cos_fov);
        virtual ~NeighborIterator() = default;
        virtual bool next(FlockMember &output_member) = 0;
    protected:
        [[nodiscard]] bool test(FlockMember const &other) const;
        World const * const world;
        Animal const animal;
        FlockMember const& member;
        float const sq_range;
        float const cos_fov;
    };
    [[nodiscard]] virtual std::unique_ptr<NeighborIterator> make_neighbor_iterator(Animal animal, FlockMember const &member, float range, float cos_fov) const;

    class NaiveIterator : public NeighborIterator{
    public:
        NaiveIterator(World const * world, Animal animal, FlockMember const &member, float range, float cos_fov);
        bool next(FlockMember &output_member) override;
    private:
        size_t flock_index{0};
        size_t member_index{0};
    };
#pragma endregion Iterator

    std::vector<Flock> flocks{};
    friend NaiveIterator;
};


class ToroidalWorld : public World {
public:
    ToroidalWorld(float width, float height) : width{width}, height{height} {};
    [[nodiscard]] sf::Vector2f position_difference(sf::Vector2f const &v1, sf::Vector2f const &v2) const override;
    void update(sf::Time delta_time) override;
private:
    void validate_position(sf::Vector2f &position) const;
    float width;
    float height;
};


#endif //DEV_CPP_WORLD_H

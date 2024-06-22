#ifndef BOIDSFML_NEIGHBORSEARCH_H
#define BOIDSFML_NEIGHBORSEARCH_H


#include "Behavior/AtomicBehavior.h"
#include "World/World.h"
#include <memory>

using FOV = AtomicBehavior::Parameters::DetectionFOV;

class NeighborSearch{
public:
    NeighborSearch(World *world, Animal animal, FlockMember const& eyes, FOV fov);
    virtual ~NeighborSearch() = default;
    virtual FlockMember* next() = 0;
protected:
    [[nodiscard]] bool test(sf::Vector2f point);
    World * const world;
    Animal const animal;
    FlockMember const& eyes;
    FOV const fov;
    float const sq_range;
};



class NeighborRange{
public:
    explicit NeighborRange(std::unique_ptr<NeighborSearch> algo);


    class Iterator{
    public:
        explicit Iterator(NeighborSearch* algo);

        Iterator& operator++();
        FlockMember* operator->();
        FlockMember& operator*();
        bool operator!=(Iterator const& other) const;
    private:
        FlockMember * item{nullptr};
        NeighborSearch * algo;
    };

    Iterator begin();
    static Iterator end();

private:
    std::unique_ptr<NeighborSearch> algo;
};
#endif //BOIDSFML_NEIGHBORSEARCH_H

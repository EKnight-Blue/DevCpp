#ifndef BOIDSFML_NEIGHBORSEARCH_H
#define BOIDSFML_NEIGHBORSEARCH_H


#include "Behavior/AtomicBehavior.h"
#include <memory>
#include <coroutine>



/**
 * Code copy-pasted and adapted from
 * https://www.vishalchovatiya.com/cpp20-coroutine-under-the-hood/#Generating_Integer_Sequence_Using_C20_Coroutine
 */
struct NeighborGenerator {

    struct promise_type {
        FlockMember* value;
        NeighborGenerator get_return_object() { return NeighborGenerator(this); }

        void unhandled_exception(){};
        static std::suspend_never initial_suspend() { return {}; }
        static std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(FlockMember* new_value) {
            value = new_value;
            return {};
        }
    };
    struct iterator {
        bool operator!=(const iterator& ) const { return !handle->done(); }
        iterator& operator++() {
            handle->resume();
            return *this;
        }
        FlockMember& operator*() const { return *handle->promise().value; }
        std::coroutine_handle<promise_type> *handle;
    };
    iterator begin() { return iterator{&handle}; }
    static iterator end() { return iterator{nullptr}; }

    explicit NeighborGenerator(promise_type* p) : handle(std::coroutine_handle<promise_type>::from_promise(*p)) {}
    ~NeighborGenerator() { handle.destroy(); }

    std::coroutine_handle<promise_type> handle;
};



using FOV = AtomicBehavior::Parameters::DetectionFOV;

class NeighborSearch{
public:
    NeighborSearch(World *world, Animal animal, FlockMember const& eyes, FOV fov);
    virtual ~NeighborSearch() = default;
    virtual FlockMember* next() = 0;
    virtual NeighborGenerator co_neighbors() = 0;
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
    Iterator end();

private:
    std::unique_ptr<NeighborSearch> algo;
};
#endif //BOIDSFML_NEIGHBORSEARCH_H

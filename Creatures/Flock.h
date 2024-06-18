
#ifndef DEV_CPP_FLOCK_H
#define DEV_CPP_FLOCK_H

#include "Animals.h"
#include "FlockMember.h"
#include "SFML/Graphics.hpp"
#include <array>
#include <memory>

class Flock {
  public:
    explicit Flock(Animal animal, float size, size_t nb_members,
                   float max_speed, float max_force);
    void put_on_rectangle(float width, float height, size_t columns,
                          size_t rows);
    void draw(sf::RenderTarget &target);
    void update(sf::Time delta_time, FlockMember &member) const;
    void move(sf::Vector2f v);

    Animal animal;
    float size;
    sf::VertexArray vertex_array;
    sf::Texture texture;
    std::vector<FlockMember> members;

    float max_speed{0.f};
    float max_force{0.f};
    void make_gui();

  private:
    void set_vertices();

    static std::array<uint8_t const, TotalStates> const frame_number;
    static std::array<sf::Vector2f const,
                      static_cast<size_t>(Animal::Count)> const anchor;
    static std::array<sf::Vector2f const,
                      static_cast<size_t>(Animal::Count)> const frame_size;
    static std::array<uint8_t, static_cast<size_t>(Animal::Count)> const
        accumulated_state_counts;
};

#endif // DEV_CPP_FLOCK_H

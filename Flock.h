
#ifndef BOIDSFML_FLOCK_H
#define BOIDSFML_FLOCK_H
#include "SFML/Graphics.hpp"


struct FlockMember{
    sf::Vector2f position{0., 0.};
    sf::Vector2f orientation{1., 0.};
    sf::Vector2f force{0, 0};
    float speed{100.};
};



sf::Vector2f operator^(sf::Vector2f const& v1, sf::Vector2f const& v2);


class Flock{
public:
    explicit Flock(size_t nb_members, float w, float h);
    void put_on_rectangle(float width, float height, size_t columns, size_t rows);
    void draw(sf::RenderTarget &target);
    void update(sf::Time delta_time);

    sf::VertexArray triangles;
    sf::Texture texture;
    std::vector<FlockMember> members;

    float const world_width;
    float const world_height;
    float max_speed{0.f};
    float max_force{0.f};

private:
    void set_vertices();
};


#endif //BOIDSFML_FLOCK_H

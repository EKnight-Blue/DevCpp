
#ifndef DEV_CPP_FLOCK_H
#define DEV_CPP_FLOCK_H
#include "SFML/Graphics.hpp"

#pragma region Animals

enum class Animal{
    // Sprites from https://elthen.itch.io/2d-pixel-art-squirrel-sprites

    Squirrel,


    // Sprites from https://lyaseek.itch.io/miniffanimals

    Rabbit,
    Bird,
    FemaleDeer,
    MaleDeer,
    Wolf,
    Bear,
    Boar,
    Fox,
    Count
};

enum class Squirrel{
    Idle,
    LookAround,
    Run,
    Dig, // idk
    Eat,
    Hurt,
    Die,
    Count
};

enum class Rabbit{
    Idle,
    Run,
    Hurt,
    Die,
    Count
};

enum class Bird{
    Idle,
    Fly,
    Die,
    Count
};

enum class FemaleDeer{
    Idle,
    Run,
    Jump,
    Attack,
    Hurt,
    Die,
    Count
};

enum class MaleDeer{
    Idle,
    Run,
    Jump,
    Attack1,
    Attack2,
    Hurt,
    Die,
    Count
};

enum class Wolf{
    Idle,
    Run,
    Jump,
    Bite,
    AirAttack, // idk
    Howling,
    Hurt,
    Die,
    Count
};

enum class Bear{
    Idle,
    Run,
    Jump,
    PawAttack,
    Bite,
    Standing,
    Hurt,
    Die,
    Count
};

enum class Boar{
    Idle,
    Run,
    Jump,
    Attack,
    Hurt,
    Die,
    Count
};

enum class Fox{
    Idle,
    Run,
    Jump,
    Attack,
    Hurt,
    Die,
    Count
};

constexpr size_t TotalStates =
     static_cast<size_t>(Squirrel::Count)
    +static_cast<size_t>(Rabbit::Count)
    +static_cast<size_t>(Bird::Count)
    +static_cast<size_t>(FemaleDeer::Count)
    +static_cast<size_t>(MaleDeer::Count)
    +static_cast<size_t>(Wolf::Count)
    +static_cast<size_t>(Bear::Count)
    +static_cast<size_t>(Boar::Count)
    +static_cast<size_t>(Fox::Count);
#pragma endregion


struct FlockMember{
    sf::Vector2f position{0., 0.};
    sf::Vector2f orientation{1., 0.};
    sf::Vector2f force{0, 0};
    float speed{0.};
    float last_wander_angle{0.};
    uint16_t age{0};
    uint8_t state{1};
};


class Flock{
public:
    explicit Flock(Animal animal, float size, size_t nb_members, float w, float h);
    void put_on_rectangle(float width, float height, size_t columns, size_t rows);
    void draw(sf::RenderTarget &target);
    void update(sf::Time delta_time);
    void move(sf::Vector2f v);

    Animal animal;
    float size;
    sf::VertexArray triangles;
    sf::Texture texture;
    std::vector<FlockMember> members;

    float const world_width;
    float const world_height;
    sf::Vector2f difference(sf::Vector2f const& v1, sf::Vector2f const & v2) const;

    float max_speed{0.f};
    float max_force{0.f};

private:
    void set_vertices();

    static std::array<uint8_t const, TotalStates> const frame_number;
    static std::array<sf::Vector2f const, static_cast<size_t>(Animal::Count)> const anchor;
    static std::array<sf::Vector2f const, static_cast<size_t>(Animal::Count)> const frame_size;
    static std::array<uint8_t, static_cast<size_t>(Animal::Count)> const accumulated_state_counts;
};


#endif // DEV_CPP_FLOCK_H

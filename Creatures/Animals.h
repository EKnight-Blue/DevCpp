#ifndef BOIDSFML_ANIMALS_H
#define BOIDSFML_ANIMALS_H

#include <cstdint>

enum class Animal {
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

#endif //BOIDSFML_ANIMALS_H

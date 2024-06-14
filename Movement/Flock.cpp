#include "Flock.h"
#include "utils.h"
#include <array>
#include <cmath>
#include <iostream>

constexpr uint16_t ANIMATION_FRAME_TIME_MS = 100;

Flock::Flock(Animal const animal, const float size, const size_t nb_members, float w, float h) : animal{animal}, size{size}, vertex_array{sf::Quads, 4 * nb_members}, members(nb_members), world_width{w}, world_height{h} {
    texture.loadFromFile("./resources/texture.png");

    for (int index{0}; index < nb_members; ++index) {
        members[index].last_wander_angle = random_float() * TWO_PI;
        members[index].orientation = {cosf(members[index].last_wander_angle),
                                      sinf(members[index].last_wander_angle)};
        for (int j{0}; j < 4; ++j) {
            vertex_array[4 * index + j].color = sf::Color::White;
        }
    }
}

Flock::Flock(Animal const animal, const float size, const size_t nb_members, float w, float h, Ihm &interface) : animal{animal}, size{size}, vertex_array{sf::Quads, 4 * nb_members}, members(nb_members), world_width{w}, world_height{h}, abonnements(NB_SUJETS) {
    texture.loadFromFile("./resources/texture.png");

    for (int index{0}; index < nb_members; ++index) {
        members[index].last_wander_angle = random_float() * TWO_PI;
        members[index].orientation = {cosf(members[index].last_wander_angle), sinf(members[index].last_wander_angle)};
        for (int j{0}; j < 4; ++j) {
            vertex_array[4 * index + j].color = sf::Color::White;
        }
    }
    // abonnement aux boutons de l'interface
    toroïdal = static_cast<bool *>(interface.abonnement(BOOL_TOR));
}

void Flock::put_on_rectangle(float const width, float const height, const size_t columns, const size_t rows) {
    for (size_t row{0}; row < rows; ++row) {
        for (size_t column{0}; column < columns; ++column) {
            members[row * columns + column].position = {
                static_cast<float>(column + 1) * width / static_cast<float>(columns + 1),
                static_cast<float>(row + 1) * height / static_cast<float>(rows + 1),
            };
        }
    }
}

void Flock::draw(sf::RenderTarget &target) {
    set_vertices();
    target.draw(vertex_array, &texture);
}

void Flock::update(sf::Time const delta_time) {
    for (auto& member : members) {
        member.age = (member.age + static_cast<uint16_t>(delta_time.asMilliseconds())) % (frame_number[accumulated_state_counts[static_cast<size_t>(animal)] + member.state] * ANIMATION_FRAME_TIME_MS);
        float force = sqrtf(member.force.x * member.force.x + member.force.y * member.force.y);
        if (force == 0.f) {
            // speed is unchanged
            member.position += (delta_time.asSeconds() * member.speed) * member.orientation;
            member.position += sf::Vector2f{
                    world_width * (static_cast<float>(member.position.x < 0.f) - static_cast<float>(member.position.x > world_width)),
                    world_height * (static_cast<float>(member.position.y < 0.f) - static_cast<float>(member.position.y > world_height))
            };
            continue;
        }

        sf::Vector2f speed;
        if (force > max_force)
            speed = member.speed * member.orientation + member.force * (delta_time.asSeconds() * max_force / force);
        else
            speed = member.speed * member.orientation + member.force * delta_time.asSeconds();
        member.force = {0., 0.};
        member.speed = sqrtf(speed.x * speed.x + speed.y * speed.y);
        if (member.speed == 0.f)
            continue;

        member.orientation = speed / member.speed;
        if (member.speed > max_speed)
            member.speed = max_speed;

        member.position +=
            (delta_time.asSeconds() * member.speed) * member.orientation;

        // toroidage
        if (*toroïdal) {
            member.position += sf::Vector2f{
                world_width * (static_cast<float>(member.position.x < 0.f) - static_cast<float>(member.position.x > world_width)),
                world_height * (static_cast<float>(member.position.y < 0.f) - static_cast<float>(member.position.y > world_height))
            };
        }
    }
}


void Flock::set_vertices() {
    static std::array<sf::Vector2f const, 4> const texture_anchors{{
        {0.f, 0.f},
        {1.f, 0.f},
        {1.f, 1.f},
        {0.f, 1.f}
    }};
    for (int index{0}; index < members.size(); ++index) {
        sf::Vector2f member_position = members[index].position;
        float state{static_cast<float>(members[index].state)};
        float age{static_cast<float>(members[index].age / ANIMATION_FRAME_TIME_MS)};
        for (int j{0}; j < 4; ++j) {
            vertex_array[4 * index + j].position = member_position + size * (sf::Vector2f{-.5f, -1.f} + texture_anchors[j]) * sf::Vector2f{1.f - 2.f * static_cast<float>(members[index].orientation.x < 0.), 1.f};
            vertex_array[4 * index + j].texCoords = anchor[static_cast<size_t>(animal)] + (sf::Vector2f{age, state} + texture_anchors[j]) * frame_size[static_cast<size_t>(animal)];
        }
    }
}


void Flock::move(sf::Vector2f v) {
    for (auto& member : members) {
        member.position += v;
    }
}

sf::Vector2f Flock::difference(const sf::Vector2f &v1, const sf::Vector2f &v2) const {
    sf::Vector2f v = v1 - v2;
    return v + sf::Vector2f{
        world_width * (static_cast<float>(v.x < -world_width * .5f) - static_cast<float>(v.x > world_width * .5f)),
        world_height * (static_cast<float>(v.y < -world_height * .5f) - static_cast<float>(v.y > world_height * .5f))
    };
}

std::array<uint8_t const, TotalStates> const Flock::frame_number = {
        // Squirrel
        6, 6, 8, 4, 2, 4, 4,
        // Rabbit
        4, 6, 2, 3,
        // Bird
        4, 4, 3,
        // FemaleDeer
        4, 4, 3, 5, 2, 4,
        // MaleDeer
        4, 4, 3, 5, 7, 2, 4,
        // Wolf
        4, 6, 4, 5, 5, 7, 2, 4,
        // Bear
        4, 6, 2, 6, 5, 10, 2, 4,
        // Boar
        4, 4, 3, 4, 2, 4,
        // Fox
        4, 4, 3, 5, 2, 4
};

std::array<sf::Vector2f const, static_cast<size_t>(Animal::Count)> const Flock::anchor = {{
      // Squirrel
      {0.f, 0.f},
      // Rabbit
      {192.f, 0.f},
      // Bird
      {256.f, 80.f},
      // FemaleDeer
      {320.f, 0.f},
      // MaleDeer
      {320.f, 208.f},
      // Wolf
      {128.f, 112.f},
      // Bear
      {0.f, 240.f},
      // Boar
      {192.f, 240.f},
      // Fox
      {128.f, 416.f},
}};

std::array<sf::Vector2f const, static_cast<size_t>(Animal::Count)> const Flock::frame_size = {{
      // Squirrel
      {32.f, 32.f},
      // Rabbit
      {16.f, 16.f},
      // Bird
      {16.f, 16.f},
      // FemaleDeer
      {32.f, 32.f},
      // MaleDeer
      {32.f, 32.f},
      // Wolf
      {32.f, 16.f},
      // Bear
      {32.f, 32.f},
      // Boar
      {32.f, 16.f},
      // Fox
      {32.f, 16.f},
}};

std::array<uint8_t, static_cast<size_t>(Animal::Count)> const Flock::accumulated_state_counts{
    0,

     static_cast<uint8_t>(Squirrel::Count),

     static_cast<uint8_t>(Squirrel::Count)
    +static_cast<uint8_t>(Rabbit::Count),

     static_cast<uint8_t>(Squirrel::Count)
    +static_cast<uint8_t>(Rabbit::Count)
    +static_cast<uint8_t>(Bird::Count),

     static_cast<uint8_t>(Squirrel::Count)
    +static_cast<uint8_t>(Rabbit::Count)
    +static_cast<uint8_t>(Bird::Count)
    +static_cast<uint8_t>(FemaleDeer::Count),

     static_cast<uint8_t>(Squirrel::Count)
    +static_cast<uint8_t>(Rabbit::Count)
    +static_cast<uint8_t>(Bird::Count)
    +static_cast<uint8_t>(FemaleDeer::Count)
    +static_cast<uint8_t>(MaleDeer::Count),

     static_cast<uint8_t>(Squirrel::Count)
    +static_cast<uint8_t>(Rabbit::Count)
    +static_cast<uint8_t>(Bird::Count)
    +static_cast<uint8_t>(FemaleDeer::Count)
    +static_cast<uint8_t>(MaleDeer::Count)
    +static_cast<uint8_t>(Wolf::Count),

     static_cast<uint8_t>(Squirrel::Count)
    +static_cast<uint8_t>(Rabbit::Count)
    +static_cast<uint8_t>(Bird::Count)
    +static_cast<uint8_t>(FemaleDeer::Count)
    +static_cast<uint8_t>(MaleDeer::Count)
    +static_cast<uint8_t>(Wolf::Count)
    +static_cast<uint8_t>(Bear::Count),

     static_cast<uint8_t>(Squirrel::Count)
    +static_cast<uint8_t>(Rabbit::Count)
    +static_cast<uint8_t>(Bird::Count)
    +static_cast<uint8_t>(FemaleDeer::Count)
    +static_cast<uint8_t>(MaleDeer::Count)
    +static_cast<uint8_t>(Wolf::Count)
    +static_cast<uint8_t>(Bear::Count)
    +static_cast<uint8_t>(Boar::Count)
};
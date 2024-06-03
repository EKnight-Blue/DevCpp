#include "Flock.h"
#include <array>
#include <cmath>

sf::Vector2f operator^(sf::Vector2f const& v1, sf::Vector2f const& v2) {
    // complex product
    return {v1.x * v2.x - v1.y * v2.y, v1.x * v2.y + v1.y * v2.x};
}

Flock::Flock(const size_t nb_members, float w, float h) : triangles{sf::Triangles, 3 * nb_members}, members(nb_members), world_width{w}, world_height{h} {
    texture.loadFromFile("./resources/texture.png");
    static std::array<sf::Vector2f const, 3> const texture_triangle {{
         {0.0f, 0.0f},
         {0.0f, 200.0f},
         {400.0f, 100.0f}
    }};
    for (int index{0}; index < nb_members; ++index) {
        for (int j{0}; j < 3; ++j) {
            triangles[3 * index + j].texCoords = texture_triangle[j];
            triangles[3 * index + j].color = sf::Color::Red;
        }
    }
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
    target.draw(triangles, &texture);
}

void Flock::update(sf::Time const delta_time) {
    for (auto& member : members) {
        float force = sqrtf(member.force.x * member.force.x + member.force.y * member.force.y);
        if (force == 0.f) {
            member.position += (member.speed) * member.orientation;
            member.force = {0., 0.};
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

        member.position += (delta_time.asSeconds() * member.speed) * member.orientation;
    }
}


void Flock::set_vertices() {
    static std::array<sf::Vector2f const, 3> const texture_anchors{{
        {-0.15f, -0.25f},
        {-0.15f, 0.25f},
        {0.85f, 0.0f}
    }};
    for (int index{0}; index < members.size(); ++index) {
        sf::Vector2f member_position = members[index].position;
        sf::Vector2f member_orientation = members[index].orientation;
        for (int j{0}; j < 3; ++j) {
            triangles[3 * index + j].position = member_position + (20.f * texture_anchors[j] ^ member_orientation);
        }
    }
}

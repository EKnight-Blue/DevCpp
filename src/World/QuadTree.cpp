#include "QuadTree.h"
#include "World.h"
#include "utils.h"

QuadTree::QuadTree(sf::Vector2f const top_left, sf::Vector2f const bottom_right, QuadTree * parent) : top_left(top_left), bottom_right(bottom_right), center{0.5f * (top_left + bottom_right)}, parent{parent} {

}
/**
 * Chose the right child to put requested point
 * @param point
 * @return
 */
size_t QuadTree::chose_quadrant(const sf::Vector2f &point) const {
    return static_cast<size_t>(point.x >= center.x) | (static_cast<size_t>(point.y >= center.y) << 1);
}

/**
 * Create the 4 children
 */
void QuadTree::divide() {
    children.emplace_back(top_left, center, this); // NW
    children.emplace_back(sf::Vector2f{center.x, top_left.y}, sf::Vector2f{bottom_right.x, center.y}, this); // NE
    children.emplace_back(sf::Vector2f{top_left.x, center.y}, sf::Vector2f{center.x, bottom_right.y}, this); // SW
    children.emplace_back(center, bottom_right, this); // SE
}

/**
 * Put an element into the structure
 * @param element
 */
void QuadTree::insert(const QuadTreeElement &element) {
    if (cnt < QuadTreeSize) {
        elements[cnt] = element;
        ++cnt;
        return;
    }
    if (children.empty())
        divide();
    children[chose_quadrant(element.position)].insert(element);
}

/**
 * Only called by the root, destroy every children, and invalidates elements data
 * @param width
 * @param height
 */
void QuadTree::reset(float width, float height) {
    cnt = 0;
    center.x = width * .5f;
    center.y = height * .5f;
    bottom_right.x = width;
    bottom_right.y = height;
    children.clear();
}

/**
 * Process intersections of a line of the F.O.V. with the lines of the rectangle
 * @param v1
 * @param v2
 * @param sq_radius
 * @param director
 * @return
 */
bool QuadTree::line_line(sf::Vector2f const &v1, sf::Vector2f const &v2, float sq_radius, sf::Vector2f const &director) const {
    if (director.x != 0.f) {
        float t{v1.x / director.x};
        float dy{-v1.y + director.y * t};
        if (0.f < t && t * t < sq_radius && 0.f <= dy && top_left.y + dy <= bottom_right.y)
            return true;
        t = v2.x / director.x;
        dy = -v2.y + director.y * t;
        if (0.f < t && t * t < sq_radius && dy <= 0.f && top_left.y <= dy + bottom_right.y)
            return true;
    }
    if (director.y != 0.f) {
        float t{v1.y / director.y};
        float dx{-v1.x + director.x * t};
        if (0.f < t && t * t < sq_radius && 0.f <= dx && top_left.x + dx <= bottom_right.x)
            return true;
        t = v2.y / director.y;
        dx = -v2.x + director.x * t;
        if (0.f < t && t * t < sq_radius && dx <= 0.f && top_left.x <= dx + bottom_right.x)
            return true;
    }
    return false;
}

/**
 * Process the intersection of the F.O.V. arc with the lines of the rectangle
 * @param v1
 * @param v2
 * @param sq_radius
 * @param cos_fov
 * @param orientation
 * @return
 */
bool QuadTree::arc_line(const sf::Vector2f &v1, sf::Vector2f const &v2, float sq_radius, float cos_fov, const sf::Vector2f &orientation) const {
    if (v1.x * v1.x < sq_radius) {
        float dy{sqrtf(sq_radius - v1.x * v1.x)};
        float ny{top_left.y + v1.y + dy};
        if (0.f <= v1.y + dy && ny <= bottom_right.y && dot(orientation, sf::Vector2f{v1.x, dy}) >= cos_fov * sqrtf(sq_radius))
            return true;
    }
    if (v2.x * v2.x < sq_radius) {
        float dy{sqrtf(sq_radius - v2.x * v2.x)};
        float ny{top_left.y + v2.y + dy};
        if (0.f <= v2.y + dy && ny <= bottom_right.y && dot(orientation, sf::Vector2f{v2.x, dy}) >= cos_fov * sqrtf(sq_radius))
            return true;
    }
    if (v1.y * v1.y < sq_radius) {
        float dx{sqrtf(sq_radius - v1.y * v1.y)};
        float nx{top_left.y + v1.y + dx};
        if (0.f <= v1.y + dx && nx <= bottom_right.y && dot(orientation, sf::Vector2f{dx, v1.x}) >= cos_fov * sqrtf(sq_radius))
            return true;
    }
    if (v2.y * v2.y < sq_radius) {
        float dx{sqrtf(sq_radius - v2.y * v2.y)};
        float nx{top_left.y + v2.y + dx};
        if (0.f <= v2.y + dx && nx <= bottom_right.y && dot(orientation, sf::Vector2f{dx, v2.x}) >= cos_fov * sqrtf(sq_radius))
            return true;
    }
    return false;
}

/**
 * Check whether the rectangle is completely inside the F.O.V. when all other tests have failed
 * @param v1
 * @param v2
 * @param sq_radius
 * @param cos_fov
 * @param orientation
 * @return
 */
bool QuadTree::completely_inside(const sf::Vector2f &v1, sf::Vector2f const &v2, float sq_radius, float cos_fov, const sf::Vector2f &orientation) {
    float m{sq_magnitude(v1)};
    if (!(m < sq_radius && dot(orientation, v1) >= cos_fov * sqrtf(m)))
        return false;
    m = sq_magnitude(v2);
    if (!(m < sq_radius && dot(orientation, v2) >= cos_fov * sqrtf(m)))
        return false;
    m = sq_magnitude({v1.x, v2.y});
    if (!(m < sq_radius && dot(orientation, {v1.x, v2.y}) >= cos_fov * sqrtf(m)))
        return false;
    m = sq_magnitude({v2.x, v1.y});
    if (!(m < sq_radius && dot(orientation, {v2.x, v1.y}) >= cos_fov * sqrtf(m)))
        return false;
    return true;
}

bool QuadTree::intersects_fov(const sf::Vector2f &point, const sf::Vector2f &orientation, float const sq_radius, float cos_fov, World const * const world) const {
    // point is in the rectangle
    if (top_left.x <= point.x && top_left.y <= point.y && point.x <= bottom_right.x && point.y <= bottom_right.y) {
        return true;
    }
    sf::Vector2f v1 = world->position_difference(top_left, point);
    sf::Vector2f v2 = world->position_difference(bottom_right, point);

    float sin_fov = sqrtf(1.f - cos_fov * cos_fov);
    if (line_line(v1, v2, sq_radius, orientation ^ sf::Vector2f{cos_fov, sin_fov}))
        return true;
    if (line_line(v1, v2, sq_radius, orientation ^ sf::Vector2f{cos_fov, -sin_fov}))
        return true;
    if (arc_line(v1, v2, sq_radius, cos_fov, orientation))
        return true;
    // entire rectangle in fov
    return completely_inside(v1, v2, sq_radius, cos_fov, orientation);
}

/**
 * For visual test purposes
 * @param target
 */
void QuadTree::draw(sf::RenderTarget &target) {
    static sf::RectangleShape shape{};
    shape.setPosition(top_left);
    shape.setSize(bottom_right - top_left);
    shape.setOutlineColor(sf::Color::Red);
    shape.setFillColor(sf::Color::Transparent);
    shape.setOutlineThickness(1);
    target.draw(shape);
    for (auto & child : children) {
        child.draw(target);
    }
}
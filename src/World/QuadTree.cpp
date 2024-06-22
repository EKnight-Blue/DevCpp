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

size_t QuadTree::count() const {
    size_t result = cnt;
    for (auto const& child : children) {
        result += child.count();
    }
    return result;
}

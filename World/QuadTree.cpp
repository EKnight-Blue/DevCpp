#include "QuadTree.h"
#include "World.h"
#include "utils.h"

QuadTree::QuadTree(sf::Vector2f const top_left, sf::Vector2f const bottom_right, QuadTree * parent) : top_left(top_left), bottom_right(bottom_right), center{0.5f * (top_left + bottom_right)}, parent{parent} {

}

size_t QuadTree::chose_quadrant(const sf::Vector2f &point) const {
    return static_cast<size_t>(point.x >= center.x) | (static_cast<size_t>(point.y >= center.y) << 1);
}

void QuadTree::divide() {
    children.emplace_back(top_left, center, this); // NW
    children.emplace_back(sf::Vector2f{center.x, top_left.y}, sf::Vector2f{bottom_right.x, center.y}, this); // NE
    children.emplace_back(sf::Vector2f{top_left.x, center.y}, sf::Vector2f{center.x, bottom_right.y}, this); // SW
    children.emplace_back(center, bottom_right, this); // SE
}

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

void QuadTree::reset() {
    cnt = 0;
    children.clear();
}

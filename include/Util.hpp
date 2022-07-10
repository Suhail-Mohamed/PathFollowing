#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <vector>
#include <iostream>

enum Direction {
    Left, Right, Up, Down,
    TopLeft, TopRight,
    BottomLeft, BottomRight
};

constexpr float PI = 3.14159265f;

const float C_DEG_TO_RAD = PI / 180.f;
const float C_RAD_TO_DEG = 180.f / PI;

float        map(const float value, const float start1, const float stop1, 
                 const float start2, const float stop2);
float        dot(const sf::Vector2f v1, const sf::Vector2f v2);
float        a2b2(const sf::Vector2f v);
float        magnitude(const sf::Vector2f v);
sf::Vector2f normalizeVector(const sf::Vector2f v);
sf::Vector2f bound(sf::Vector2f vec, const float upperBound);
float        getAngle(const sf::Vector2f v);
sf::Vector2f getNormalizedPoint(const sf::Vector2f p, const sf::Vector2f a, 
                                const sf::Vector2f b);
float        distance(const sf::Vector2f p1, const sf::Vector2f p2);
Direction    getDirection(const sf::Vector2f vecStart, const sf::Vector2f vecEnd);

template <typename T>
void printVec(const std::vector<T> vec) {
    std::cout << "{";
    for (const auto& v : vec)
        std::cout << v << ", ";
    std::cout << "}\n";
}

template <typename T>
void printSFVec(const sf::Vector2<T>& sfVec) {
    std::cout << "{" << sfVec.x << ", " << sfVec.y << "}";
}

template <typename T>
void printVec(const std::vector<sf::Vector2<T>>& vec) {
    std::cout << "{";
    for (const auto& v : vec) {
        printSFVec(v);
        std::cout << ", ";
    }
    std::cout << "}\n";
}

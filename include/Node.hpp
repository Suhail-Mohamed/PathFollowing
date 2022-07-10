#pragma once

#include <SFML/Graphics.hpp>
#include "Constants.hpp"
#include <utility>

static constexpr int   OUT_OF_BOUNDS = -1;
static constexpr float HALF_LENGTH_SQUARE = cons::BOX_SIZE / 2;

struct Node {
    sf::Vector2i       prev         = {OUT_OF_BOUNDS, OUT_OF_BOUNDS}; 
    sf::RectangleShape obstacle;
    float              weight       = INFINITY;
    bool               visited      = false;
    bool               renderSquare = false;
    
    Node() {
        obstacle = sf::RectangleShape({cons::BOX_SIZE, cons::BOX_SIZE});
        obstacle.setFillColor(sf::Color::White);
        obstacle.setOutlineColor(sf::Color::White);
        obstacle.setOutlineThickness(2.0f);
    }

    bool isObstacle() {
        return (obstacle.getFillColor() == sf::Color::Red);
    }

    bool isGoal() {
        return (obstacle.getFillColor() == sf::Color::Green);
    }

    sf::Vector2f getMidPoint() {
        auto obsPos = obstacle.getPosition();
        
        return {obsPos.x + HALF_LENGTH_SQUARE,
                obsPos.y + HALF_LENGTH_SQUARE};
    }

    void softReset() {
        prev    = {OUT_OF_BOUNDS, OUT_OF_BOUNDS};
        weight  = INFINITY;
        visited = false;
    }

    void hardReset() {
        softReset();
        renderSquare = false;
        obstacle.setFillColor(sf::Color::White);
    }
};

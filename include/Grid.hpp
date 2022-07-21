#pragma once

#include <vector>
#include <array>
#include <utility>
#include <iostream>
#include <queue>

#include <SFML/Graphics.hpp>

#include "Constants.hpp"
#include "Util.hpp"
#include "Node.hpp"
#include "Vehicle.hpp"

static constexpr int WEIGHT_STRAIGHT = 1; /* The distance between two axis aligned grid Nodes */
static constexpr int WEIGHT_DIAG     = 2; /* The distance between two non-axis aligned grid Nodes */
static constexpr int CIRCLE_RAD      = 3;

/* Used for accessing parts of the dirs std::array */
static constexpr int DIR_ROW_SIZE    = 2; 
static constexpr int DIR_COL_SIZE    = 5;
static constexpr int NSEW_DIRS       = 0; 
static constexpr int DIAG_DIRS       = 1;

const sf::Color GRAY (128, 128, 128);

struct DebugNode {
    sf::CircleShape circle;
    sf::Vertex      line[2];
    sf::Vector2i    prev;
    bool            lineSet = false;

    DebugNode() {
        circle = sf::CircleShape(CIRCLE_RAD);
        circle.setFillColor(GRAY);
        circle.setOutlineColor(sf::Color::White);
    }

    void highlight() {
        circle.setFillColor(sf::Color::Magenta);
    }

    void unHighlight() {
        circle.setFillColor(GRAY);
    }

    void setLine(sf::Vector2f neighbour, sf::Vector2i prevVal) {
        if (prevVal.x == OUT_OF_BOUNDS) return;
        auto circlePos = circle.getPosition();

        lineSet = true;
        line[0] = sf::Vertex({circlePos.x + 3, circlePos.y + 3});
        line[1] = sf::Vertex({neighbour.x + 3, neighbour.y + 3});
        prev    = prevVal;
    }
};

class Grid {
    public:
        Grid();
        void draw(sf::RenderWindow& window);
        void addObstacleOrGoal(const sf::Vector2f position, bool isGoal, bool isDrawing = false);
        void resetGrid(bool hardReset);

        bool goalSet = false;
        
    private:
        sf::Vector2i getClosestNode(const sf::Vector2f position);
        void         checkCollisions(const sf::Vector2i& closestObstacle);
        bool         hasCollided(const sf::RectangleShape& obstacle,
                                 const std::vector<sf::Vector2f>& vehVert);
        void         createOptimalPath(sf::Vector2i start);

        std::vector<std::vector<Node>>      gridNodes;
        std::vector<std::vector<DebugNode>> debug;
        Vehicle                             vehicle;
        sf::Vector2i                        goalPos = {OUT_OF_BOUNDS, OUT_OF_BOUNDS};
        
        const std::array<std::array<int, 5>, 2> dirs {{{0, 1,  0, -1, 0},   // used for checking down (0, 1), right(1, 0), up(0, -1) and left (-1, 0) from some spot in grid
                                                       {1, 1, -1, -1, 1}}}; // used for checking upperRight(1, 1), lowerRight(1, -1), lowerLeft(-1. -1) and upperLeft
};
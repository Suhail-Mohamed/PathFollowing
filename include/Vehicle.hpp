#pragma once

#include <SFML/Graphics.hpp>

#include <vector>
#include <array>
#include <iostream>

#include "Util.hpp"
#include "Node.hpp"
#include "Constants.hpp"

static constexpr float MAX_SPEED        = 3.5f;
static constexpr float MAX_FORCE        = 0.5f;
static constexpr float MAX_STOP_SPEED   = 0.1f;
static constexpr float MAX_STOP_FORCE   = 0.1f;
static constexpr float PATH_RADIUS      = 4.0f;
static constexpr float TOO_CLOSE        = 150.0f;
static constexpr int   SLOW_DOWN_RADIUS = 128;
static constexpr int   GIVE             = 128;

struct Vehicle {
    Vehicle();
    void moveVehicle();
    void seek(const sf::Vector2f point, bool arrive = false);
    int  followPath();
    void dodgeObstacle(const sf::Vector2f obsPos);
    void collideObstacle(const sf::Vector2f colAngle);

    std::vector<sf::Vector2f> path;
    sf::ConvexShape           shape;
    sf::Vector2f              velocity;
};
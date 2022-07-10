#pragma once

#include <SFML/System/Vector2.hpp>
#include <vector>

namespace cons {
    static constexpr int            WIND_SIZE       = 750;                        /* Height and width of our window */
    static constexpr int            BOX_SIZE        = 25;                         /* each square is 1/25 the width and height of window */
    static constexpr int            GRID_SIZE       = WIND_SIZE / BOX_SIZE;       /* number of boxes we can fit in our grid. */
    const std::vector<sf::Vector2f> CARTESIAN_PLANE {{1.0f, 0.0f}, {0.0f, 1.0f}}; /* for squares perpendiculars are x and y axis */
};
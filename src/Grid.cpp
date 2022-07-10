#include "Grid.hpp"

Grid::Grid() {
    gridNodes = std::vector<std::vector<Node>>     (cons::GRID_SIZE, std::vector<Node>(cons::GRID_SIZE));
    debug     = std::vector<std::vector<DebugNode>>(cons::GRID_SIZE, std::vector<DebugNode>(cons::GRID_SIZE));

    for (int x = 0; x < cons::GRID_SIZE; ++x) {
        for (int y = 0; y < cons::GRID_SIZE; ++y) {
            gridNodes[x][y].obstacle.setPosition({static_cast<float>(x * cons::BOX_SIZE), 
                                                  static_cast<float>(y * cons::BOX_SIZE)});

            debug[x][y].circle.setPosition({static_cast<float>(x * cons::BOX_SIZE + HALF_LENGTH_SQUARE - 2), 
                                            static_cast<float>(y * cons::BOX_SIZE + HALF_LENGTH_SQUARE - 3)});
        }
    }
}

/*****************************************************************************/

void Grid::draw(sf::RenderWindow& window) {
    int pathIdx = vehicle.followPath();
    const auto   closestObstacle = getClosestNode(vehicle.shape.getPosition());
    sf::Vector2i closestDNode {OUT_OF_BOUNDS, OUT_OF_BOUNDS};

    /* We are driving through path if pathIdx != -1 */
    if (pathIdx != -1)
        closestDNode = getClosestNode(vehicle.path[pathIdx]);

    for (int x = 0; x < cons::GRID_SIZE; ++x) {
        for (int y = 0; y < cons::GRID_SIZE; ++y) {
            if (gridNodes[x][y].renderSquare)
                window.draw(gridNodes[x][y].obstacle);

            debug[x][y].unHighlight();

            if (sf::Vector2i{x, y} == closestDNode)
                debug[x][y].highlight();
            
            if (debug[x][y].lineSet)
                window.draw(debug[x][y].line, 2, sf::Lines);

            window.draw(debug[x][y].circle);
        }
    }
    
    if (closestObstacle.x != OUT_OF_BOUNDS)
        checkCollisions(closestObstacle);

    window.draw(vehicle.shape);
}

/*****************************************************************************/

void Grid::addObstacleOrGoal(const sf::Vector2f position, bool isGoal, bool isDrawing) {
    const sf::Vector2i coord = getClosestNode(position);

    if (coord.x == OUT_OF_BOUNDS) return;
    
    Node& node = gridNodes[coord.x][coord.y];
    
    if (!node.renderSquare) {
        resetGrid(false);
        node.renderSquare = true;
        node.obstacle.setFillColor(sf::Color::Red);

        if (isGoal && !goalSet) {
            goalSet = true;
            goalPos = coord;

            node.obstacle.setFillColor(sf::Color::Green);
            createOptimalPath(getClosestNode(vehicle.shape.getPosition()));
        }
    } else {
        if (isDrawing) return;
        node.renderSquare = false;
        node.prev         = {OUT_OF_BOUNDS, OUT_OF_BOUNDS};
        node.weight       = INFINITY;
        node.visited      = false;

        /* If the coordinate we are removing was formerly a goal obstacle */
        if (node.isGoal()) {
            goalSet = false;
            goalPos = {OUT_OF_BOUNDS, OUT_OF_BOUNDS};
            vehicle.path.clear();
        }
        /* Make node the neutral colour */
        node.obstacle.setFillColor(sf::Color::White);
    }
}

/*****************************************************************************/
/*
    hardReset --> removes all obstacles and lines are board
                  done when user presses 'C'
    softReset --> removes all lines but not obstacles
                  done before the creation of any path
*/
void Grid::resetGrid(bool hardReset) {
    for (int x = 0; x < cons::GRID_SIZE; ++x) {
        for (int y = 0; y < cons::GRID_SIZE; ++y) {
            debug[x][y].lineSet = false;

            (hardReset) ? gridNodes[x][y].hardReset() : 
                          gridNodes[x][y].softReset(); 
        }
    }

    if (hardReset) goalSet = false;
}

/*****************************************************************************/

/* 
   Returns the closest grid index to the vehicle. If the closest grid value
   is out of bounds the coordinate returned is {-1, -1}
*/
sf::Vector2i Grid::getClosestNode(const sf::Vector2f position) {
    sf::Vector2i obstacleCoord;

    obstacleCoord.x = map(position.x, 0, cons::WIND_SIZE, 0, cons::GRID_SIZE);
    obstacleCoord.y = map(position.y, 0, cons::WIND_SIZE, 0, cons::GRID_SIZE);

    if (obstacleCoord.x < 0 || obstacleCoord.x >= cons::GRID_SIZE || 
        obstacleCoord.y < 0 || obstacleCoord.y >= cons::GRID_SIZE)
        return {OUT_OF_BOUNDS, OUT_OF_BOUNDS};

    return obstacleCoord;
}

/*****************************************************************************/

void Grid::checkCollisions(const sf::Vector2i& closestObstacle) {
    /* 
        All this loop does is check all eight areas around our vehicle
        looks a bit scary but it is quite simple.
        Only loops 10 iterations so not as in-efficient as it looks
    */
    for (int i = 0; i < DIR_ROW_SIZE; ++i) {
        for (int j = 0; j < DIR_COL_SIZE - 1; ++j) {
            const int x = closestObstacle.x + dirs[i][j], 
                      y = closestObstacle.y + dirs[i][j + 1];

            if (x < 0 || x >= cons::GRID_SIZE || 
                y < 0 || y >= cons::GRID_SIZE) continue;

            Node& node = gridNodes[x][y];

            if (node.isObstacle())
                vehicle.dodgeObstacle(node.obstacle.getPosition());
        }
    }

     /* 
        The use of SFML transform is to get the vehicles (or obstacles) vertice coordinates relative
        to the actual game window, NOT relative to the shapes origin. For example the first vertice for
        the triangle relative to its origin is (10, 0) [triangle is generated lying on side, pointing right] 
        as we move 10 pixels to the right of the midpoint of the triangle then draw its head.
    */

    const sf::Transform             vehT    = vehicle.shape.getTransform(); 
    const std::vector<sf::Vector2f> vehVert {vehT.transformPoint(vehicle.shape.getPoint(0)),
                                             vehT.transformPoint(vehicle.shape.getPoint(1)),
                                             vehT.transformPoint(vehicle.shape.getPoint(2))};
    
    for (int i = 0; i < DIR_ROW_SIZE; ++i) {
        for (int j = 0; j < DIR_COL_SIZE - 1; ++j) {
            const int x = closestObstacle.x + dirs[i][j], 
                      y = closestObstacle.y + dirs[i][j + 1];

            if (x < 0 || x >= cons::GRID_SIZE || 
                y < 0 || y >= cons::GRID_SIZE) continue;

            Node& node = gridNodes[x][y];

            if (node.isObstacle() && hasCollided(node.obstacle, vehVert))
                vehicle.collideObstacle({static_cast<float>(dirs[i][j]), 
                                         static_cast<float>(dirs[i][j + 1])});
        }
     }
}

/*****************************************************************************/

/* 
    Implementation of Seperated Axis Theorem:
        Essentially we crush 2 shapes onto each of their normal axis's.
        if we find one axis in which there is no intersection we are
        assured there will be no intersection between the two shapes 
        and we can return false.
    
    Good video: https://www.youtube.com/watch?v=7Ik2vowGcU0
*/
bool Grid::hasCollided(const sf::RectangleShape& obstacle,
                       const std::vector<sf::Vector2f>& vehVert) {
    std::pair<float, float> obsProjection;
    std::pair<float, float> vehProjection;

    /* Get absolute position of obstacle vertices */
    const sf::Transform             obsT    = obstacle.getTransform();
    const std::vector<sf::Vector2f> obsVert {obsT.transformPoint(obstacle.getPoint(0)), obsT.transformPoint(obstacle.getPoint(1)),
                                             obsT.transformPoint(obstacle.getPoint(2)), obsT.transformPoint(obstacle.getPoint(3))};
    
    /* go through the 2 potentially overlapping shapes */
    for (int shape = 0; shape < 2; ++shape) {
        /* since our obstacles are axis aligned we will be projecting onto the 
           cartesion plane to see if there is a overlap between the vehicle and obstacle */
        const auto& vert = (shape == 0) ? cons::CARTESIAN_PLANE : vehVert;

        for (int p1 = 0; p1 < vert.size(); ++p1) {
            const int          p2       = (p1 + 1) % vert.size(); /* wrap around to get next point to create nrml axis for projection */
            const sf::Vector2f nrmlAxis = { -(vert[p2].y - vert[p1].y), 
                                             (vert[p2].x - vert[p1].x) };

            obsProjection = {INFINITY, -INFINITY};
            vehProjection = {INFINITY, -INFINITY};

            for (const auto& v : obsVert) {
                const float dotPrd   = dot(nrmlAxis, v); /* Dot product is a projection of vectors */
                obsProjection.first  = std::min(obsProjection.first , dotPrd);
                obsProjection.second = std::max(obsProjection.second, dotPrd);
            }

            for (const auto& v : vehVert) {
                const float dotPrd   = dot(nrmlAxis, v);
                vehProjection.first  = std::min(vehProjection.first , dotPrd);
                vehProjection.second = std::max(vehProjection.second, dotPrd);
            }

            /* if no overlap return */
            if (!(vehProjection.second >= obsProjection.first && 
                  obsProjection.second >= vehProjection.first)) 
                return false; 
        }
    }

    return true;
}

/*****************************************************************************/

/* 
    Creates an optimal path from the current vehicle position to the position 
    of the placed goal. Uses Dijkstras.
*/
void Grid::createOptimalPath(const sf::Vector2i start) {
    if (!goalSet) return;
    
    using Data = std::pair<sf::Vector2i, int>;
    
    const auto cmp = [](const Data a, const Data b) -> bool 
               { return a.second > b.second; };

    std::priority_queue<Data, std::vector<Data>, decltype(cmp)> heap(cmp);
    heap.push({start, 0});

    while (!heap.empty()) {
        const auto [coord, currWeight] = heap.top(); heap.pop();
        Node& curr = gridNodes[coord.x][coord.y];

        if (curr.visited)     continue;
        if (coord == goalPos) break;

        curr.visited = true;

        for (int i = 0; i < DIR_ROW_SIZE; ++i) {
            for (int j = 0; j < DIR_COL_SIZE - 1; ++j) {
                const int x = coord.x + dirs[i][j], 
                          y = coord.y + dirs[i][j + 1];
                
                if (x < 0 || x >= cons::GRID_SIZE ||
                    y < 0 || y >= cons::GRID_SIZE || 
                    gridNodes[x][y].isObstacle() || 
                    gridNodes[x][y].visited)
                    continue;

                Node& neighbour = gridNodes[x][y];
                int edgeWeight  = (i == NSEW_DIRS) ? WEIGHT_STRAIGHT : 
                                                     WEIGHT_DIAG;

                if (edgeWeight == WEIGHT_DIAG && (gridNodes[x][coord.y].isObstacle() ||
                    gridNodes[coord.x][y].isObstacle()))
                    continue;
                    
                if (neighbour.weight > currWeight + edgeWeight) {
                    neighbour.weight = currWeight + edgeWeight;
                    neighbour.prev   = coord;
                }

                heap.push({{x, y}, neighbour.weight});
            }
        }
    }

    auto pathNode = goalPos;
    while (gridNodes[pathNode.x][pathNode.y].prev.x != OUT_OF_BOUNDS) {
        Node&      node  = gridNodes[pathNode.x][pathNode.y];
        DebugNode& dNode = debug[pathNode.x][pathNode.y];

        vehicle.path.push_back(node.getMidPoint());
        pathNode = node.prev;
        dNode.setLine(debug[node.prev.x][node.prev.y].circle.getPosition(), node.prev);
    }

    /* 
        Since we create our path backwards ie: from goal to start, we have to 
        reverse our path. although we could just work backwards from end to front,
        this just makes things later on a little nicer to work with
    */
    std::reverse(vehicle.path.begin(), vehicle.path.end());
}

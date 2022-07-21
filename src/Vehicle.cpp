#include "Vehicle.hpp"

Vehicle::Vehicle() :
            shape   (sf::ConvexShape(3)),
            velocity(sf::Vector2f(1.0, 1.0))
{   
    shape.setPosition(sf::Vector2f{50, 50});     /* midpoint of the shape, we build from midpoint */
    shape.setPoint(0, sf::Vector2f(10.0,  0.0)); /* front point of triangle */
    shape.setPoint(1, sf::Vector2f(-5.0, -4.0)); /* top point of triangle, ie: left point from tringles perspective */
    shape.setPoint(2, sf::Vector2f(-5.0,  4.0)); /* bottom point of traingle, right point from triangles perspective */
    shape.setFillColor(sf::Color::Cyan);

    /* Vector can never get larger than this */
    path.reserve(cons::BOX_SIZE * cons::BOX_SIZE);
}

/****************************************************************************/

void Vehicle::moveVehicle() {
    shape.setRotation(getAngle(velocity) * C_RAD_TO_DEG);
    shape.move(velocity); 
}

/****************************************************************************/

/*
    Makes the vehicle seek to a point, ie: go towards it but in a natural way
*/
void Vehicle::seek(const sf::Vector2f point, const bool arrive) {
    /* steering = desired velocity - current velocity */
    sf::Vector2f desired = point - shape.getPosition();
    sf::Vector2f steering;

    /* When arriving at our last point we slow down our vehicle, as if it is parking */
    if (arrive) {
        float dist = magnitude(desired);

        if (dist < SLOW_DOWN_RADIUS) 
            desired = normalizeVector(desired) * 
                      map(dist, 0, 100, 0, MAX_SPEED); /* map speed to dist vehicle */
    }

    steering = bound(desired - velocity , MAX_FORCE);
    velocity = bound(velocity + steering, MAX_SPEED);
    moveVehicle();
}

/****************************************************************************/

/*
    This version of path following has to be used as it is able to correct 
    itself in the case it gets off path, which can happen if a vehicle collides
    with an obstacle.

    Essential idea is:

        Vehicle
          |
          | <- normal, which is perp line from vehicle to path
    a ____|__________b <- Path

    The smallest normal calculated from all possible normals in the path
    should be at most PATH_RADIUS away, if not we have to make vehicle seek
    to a point on the path in which is had the smallest normal with.
*/

int Vehicle::followPath() {
    if (path.empty()) return -1;
    if (path.size() == 1) {
        seek(path.back(), true);
        return 0;
    }

    const auto   futPos  = shape.getPosition() + velocity;
    float        closest = INFINITY;
    int          idx     = 0;
    sf::Vector2f target;
    
    for (size_t i = 0; i < path.size() - 1; ++i) {
        auto a         = path[i];
        auto b         = path[i + 1];
        auto normPoint = getNormalizedPoint(futPos, a, b);
        
        /*
            Checking if normal point is NOT on the line (a, b).
            This happens at hard turns example:
                
               * <- closest perp, still closes path vertice is b
               b ___________________ .... [Goal]
               |
               V <- vehicle position here, if vehicle is moving up straight
               |    its closest perp will always be a little past point b
               |    although we want to continue following path instead of 
               |    moving towards vertice b

            Solution is just use the next edge (so horizontal edge in example above) 
            as our next edge we will use for calculation
        */
        if (std::min(a.x, b.x) > normPoint.x || std::max(a.x, b.x) < normPoint.x ||
            std::min(a.y, b.y) > normPoint.y || std::max(a.y, b.y) < normPoint.y) {
            normPoint = b;
            a         = path[i + 1];
            b         = (i + 2 > path.size()) ? path.back() : path[i + 2];
        }
        
        if (const float dist = distance(normPoint, futPos);
            dist < closest) {
            closest = dist;
            /* Target is a point on path (a, b) that is a little in front of the vehicle  */
            target  = normPoint + (normalizeVector(b - a) * 10.0f);
            idx     = i;
        }
    }

    /* 
        If the edge closest to use is the last edge then we will seek to it and do a special animation
        hence the boolean flag
        Otherwise if our smallest normal is still to far away or the vehicle is moving too slowly
        we will seek to our calculated target, otherwise of all things are good we will keep moving
        the vehicle with its current velocity 
    */
    if (idx == path.size() - 2)
        seek(path.back(), true);
    else 
        (closest > PATH_RADIUS || 
        magnitude(velocity) < 1.0f) ? seek(target) : 
                                      moveVehicle();
    return idx + 1;
}

/****************************************************************************/

void Vehicle::dodgeObstacle(const sf::Vector2f obsPos) {
    const auto vehPos = shape.getPosition();

    /* 
        Essentially the same code as seek except we add the opposite vector to
        vehicle velocity
    */
    if (const float dist = distance(obsPos, vehPos);
        dist < TOO_CLOSE) {
        sf::Vector2f desired = normalizeVector(obsPos - vehPos) * 
                               map(dist, 0, TOO_CLOSE, 0, MAX_STOP_FORCE);
        velocity = velocity + -1.2f * bound(desired - velocity, MAX_STOP_SPEED);
    }
}

/****************************************************************************/

void Vehicle::collideObstacle(const sf::Vector2f colAngle) {
    velocity += (colAngle * -1.16f * magnitude(velocity));
}
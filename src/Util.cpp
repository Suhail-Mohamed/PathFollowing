#include "Util.hpp"

/*
  Re-maps a number from one range to another. Very useful
 
  @param  value   the incoming value to be converted
  @param  start1  lower bound of the value's current range
  @param  stop1   upper bound of the value's current range
  @param  start2  lower bound of the value's target range
  @param  stop2   upper bound of the value's target range
  @return float   remapped number
 */
float map(const float value, const float start1, const float stop1, 
         const float start2, const float stop2) {
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

float dot(const sf::Vector2f v1, const sf::Vector2f v2) {
    return v1.x * v2.x + v1.y * v2.y;
}

/* Does a^2 b^2 on a given vector*/
float a2b2(const sf::Vector2f v) {
    return v.x * v.x + v.y * v.y;
}

float magnitude(const sf::Vector2f v) {
    return sqrt(a2b2(v));
}

/* Returns the normalized version of vector */
sf::Vector2f normalizeVector(const sf::Vector2f v) {
    return v / magnitude(v);
}

/* bound the magnitude of a vector by some upperbound */
sf::Vector2f bound(sf::Vector2f vec, const float upperBound) {
    if(a2b2(vec) > upperBound * upperBound)
        vec = normalizeVector(vec) * upperBound;
    return vec;
}

/*
    Returns the angle of a vector in RADIANS, IF you want
    to use this for rotating something in SFML multiply
    output by cRadToDeg.

    This is for when moving objects using vectors, as opposed
    to angles. Only thing that changed was that we don't
    expected negative y value
*/
float getAngle(const sf::Vector2f v) {
    return std::atan2(v.y, v.x);
}

/*
    returns the point of intersection of the normal line from point p, and the
    line segment ab.
        
          p
          |
          |
    a ____|__________b
          ^-- this point
*/
sf::Vector2f getNormalizedPoint(const sf::Vector2f p, const sf::Vector2f a, const sf::Vector2f b) {
    const auto ap = p - a;
    sf::Vector2f ab = normalizeVector(b - a);

    ab *= dot(ap, ab);

    return a + ab;
}

float distance(const sf::Vector2f p1, const sf::Vector2f p2) {
    return sqrt(pow(p1.x - p2.x, 2) + 
                pow(p1.y - p2.y, 2));
}

Direction getDirection(const sf::Vector2f vecStart, const sf::Vector2f vecEnd) {
    int xDiff = vecStart.x - vecEnd.x;
    int yDiff = vecStart.y - vecEnd.y;

    if (xDiff == 0)
        return (yDiff > 0) ? Down : Up;
    
    return (xDiff > 0) ? Right : Left;
}
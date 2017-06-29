#include "utility.h"


namespace utility {

    sf::Vector2f normalize(const sf::Vector2f &source) {
        float length = sqrt((source.x * source.x) + (source.y * source.y));

        if(length != 0)
            return sf::Vector2f(source.x / length, source.y / length);
        else
            return source;
    }

    float magnitude(sf::Vector2f point, sf::Vector2f origin = sf::Vector2f(0.f,0.f)) {
       return sqrt( pow((point.x - origin.x), 2) + pow((point.y - origin.y), 2) );
    }

    sf::Vector2f rotatePoint(sf::Vector2f point, sf::Vector2f origin, float angle) {
        sf::Vector2f displacement = point - origin;
        angle = -angle * PI / 180.0;        //sfml uses clockwise rotation

        point.x = displacement.x * cos(angle) + displacement.y * sin(angle);
        point.y = displacement.y * cos(angle) - displacement.x * sin(angle);
        return point + origin;
    }

    sf::Vector2f lerp(float t, const sf::Vector2f &a, const sf::Vector2f &b) {
        return (1-t)*a + t*b;
    }

    float clamp(float n, float lower, float upper) {
        return std::max(lower, std::min(n, upper));
    }
}
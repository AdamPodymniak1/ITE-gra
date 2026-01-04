#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <string>
#include "game_texture.h"
#include "constants.h"

class Projectile {
public:
    double x, y;
    double angle;
    double speed;
    std::string owner;
    std::string type;
    GameTexture* texture;

    Projectile(double x, double y, double angle, const std::string& type, GameTexture* texture, const std::string& owner)
        : x(x), y(y), angle(angle), speed(0.2), owner(owner), type(type), texture(texture) {
    }

    void update() {
        x += cos(degreeToRadians(angle)) * speed;
        y += sin(degreeToRadians(angle)) * speed;
    }
};

#endif
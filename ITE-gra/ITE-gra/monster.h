#ifndef MONSTER_H
#define MONSTER_H

#include <string>
#include "game_texture.h"

struct Monster {
    std::string id;
    std::string type;
    std::string skin;
    std::string audio;
    double x, y;
    int health;
    bool isDead;
    int width, height;
    GameTexture* texture;
    int damage;
    double lastAttack;
    double attackCooldown;

    Monster() : x(0), y(0), health(100), isDead(false), width(512), height(512),
        texture(nullptr), damage(1), lastAttack(0), attackCooldown(1.0) {
    }
};

#endif
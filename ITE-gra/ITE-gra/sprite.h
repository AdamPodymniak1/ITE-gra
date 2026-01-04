#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include "game_texture.h"

struct Sprite {
    std::string id;
    double x, y;
    int width, height;
    GameTexture* texture;

    Sprite() : x(0), y(0), width(0), height(0), texture(nullptr) {}
    Sprite(const std::string& id, double x, double y, int w, int h)
        : id(id), x(x), y(y), width(w), height(h), texture(nullptr) {
    }
};

#endif
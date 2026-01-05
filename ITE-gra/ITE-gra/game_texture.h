#ifndef GAME_TEXTURE_H
#define GAME_TEXTURE_H

#include "raylib.h"

struct GameTexture {
    Texture2D texture;
    int width, height;
    Color* pixels = nullptr;

    GameTexture() { texture.id = 0; texture.width = 0; texture.height = 0; texture.mipmaps = 0; texture.format = 0; width = 0; height = 0; }
    ~GameTexture() { if (texture.id != 0) UnloadTexture(texture); }

    int getWidth() const { return width > 0 ? width : texture.width; }
    int getHeight() const { return height > 0 ? height : texture.height; }
};

#endif
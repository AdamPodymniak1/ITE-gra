#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include "raylib.h"
#include "settings.h"


class Button {
private:
	Texture2D texture;
	Vector2 position;
	Vector2 scale;
public:
	Button(const char* imagePath = "" , Vector2 imagePosition = {0,0}, Vector2 scale = {1,1});
	~Button();
	void Draw();
	bool isPressed(Vector2 mousePos, bool mousePressed);
};

#endif
#pragma once
#ifndef MENU_H
#define MENU_H

#include "raylib.h"



class Menu {
private:
	Texture2D texture;
	Vector2 position;
public:
	Button(const char* imagePath, Vector2 imagePosition);
	~Button();
	void Draw();
};

#endif
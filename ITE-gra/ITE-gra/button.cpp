#include "button.h"


Button::Button(const char* imagePath, Vector2 imagePosition, Vector2 scale)
{
	if (imagePath != "")
	{
		Image image = LoadImage(imagePath);
		int resWidth = image.width * scale.x;
		int resHeight = image.height * scale.y;

		ImageResize(&image, resWidth, resHeight);
		texture = LoadTextureFromImage(image);
	}
	
	position = imagePosition;
}
Button::~Button() 
{
	UnloadTexture(texture);
}

void Button::Draw()
{
	DrawTextureV(texture, position, WHITE);
}

bool Button::isPressed(Vector2 mousePos, bool mousePressed) 
{
	Rectangle rect = { position.x, position.y,texture.width,texture.height};
	if (CheckCollisionPointRec(mousePos, rect) && mousePressed) {
		return true;
	}
	return false;
}
#ifndef SETTINGS_H
#define SETTINGS_H

#include "nlohmann/json.hpp"
#include <fstream>
#include <filesystem>
#include "raylib.h"
#include <string>
#include <iostream>
#include <variant>
#include "button.h"


class Settings {

private:
	const char *path = "Settings.json";
	
	void save();

	void inputText(int width,int height, int posX,int posY, int restr);
	void volumeSlider(int posX, int posY);
	void diffChoose(int posX, int posY);
	void resolutionChoose(int posX,int posY);

public:

	float volume;
	int fov;
	int difficulty;
	Vector2 resolution;
	Vector2 scale;

	void loadAll();
	void open();
	bool checkForFile();
};

#endif


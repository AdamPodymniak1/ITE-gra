#ifndef EDITOR_H
#define EDITOR_H

#include "raylib.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

class Editor{
	private:
	public:
		float volume;
		int fov;
		int difficulty;
		Vector2 resolution;
		Vector2 scale;
		void open();
};

#endif
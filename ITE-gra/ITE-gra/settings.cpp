#include "settings.h"



using json = nlohmann::json;
json jSet;



float minV=0.0;
float maxV=1.0;


void Settings::loadAll()
{
	Settings::checkForFile();
	if (std::filesystem::exists(path))
	{
		std::ifstream file(path);
		if (file.is_open())
		{
			file >> jSet;

			fov = jSet["fov"];
			volume = jSet["volume"];
			difficulty = jSet["difficulty"];
			resolution = { jSet["sWidth"], jSet["sHeight"] };
			scale.x = jSet["sWidth"] / 800.0;
			scale.y = jSet["sHeight"] / 600.0;
			file.close();
		
		}else { std::cout << "Cannot open settings files" << std::endl; }	
	}
	else { std::cout << "Settings file doesn't exist" << std::endl; }
		
}

bool Settings::checkForFile()
{
	if (!std::filesystem::exists(path))
	{
		std::ofstream file(path, std::ios::trunc);

		if (file.is_open())
		{
			jSet["sWidth"] = 800;
			jSet["sHeight"] = 600;
			jSet["fov"] = 60;
			jSet["difficulty"] = 1; // 0 - easy, 1 - normal 2 - hard
			jSet["volume"] = 1.0;

			file << jSet.dump(4);
			file.close();
			return true;

		} else { return false; }	
	}
	return true;
}

void Settings::save() {

	std::ofstream file(path, std::ios::trunc);

	if (file.is_open())
	{
		jSet["sWidth"] = resolution.x;
		jSet["sHeight"] = resolution.y;
		jSet["fov"] = fov==0 ? 60 : fov;
		jSet["difficulty"] = difficulty; // 0 - easy, 1 - normal 2 - hard
		jSet["volume"] = volume;

		file << jSet.dump(4);

		file.close();
	}
}



bool Editing = false;
std::string valueF;
void Settings::inputText(int width, int height, int posX, int posY,int restr) {

	Rectangle rect = { posX,posY,width,height };
	DrawRectangleRec(rect, WHITE);
	DrawRectangleLinesEx(rect,2, Editing?RED:GRAY);
	if (fov == 0)
	{
		valueF = "";
	}
	else {
		valueF = std::to_string(fov);
	}
	
	

	if ((CheckCollisionPointRec(GetMousePosition(), rect)&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON))||Editing)
		{
			SetMouseCursor(MOUSE_CURSOR_IBEAM);
			Editing = !IsKeyPressed(KEY_ENTER);

			int key = GetCharPressed();
			if (key >= 48 && key <= 57)
			{
				
					valueF += (char)key;
						
			}

			if (IsKeyPressed(KEY_BACKSPACE) && !valueF.empty()) {
				valueF.pop_back();
			}
			
			
			
				
		}else {
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
		}
		DrawText(valueF.c_str(), rect.x+5, rect.y + 3, height - 4, BLACK);
		
		


		if (!valueF.empty())
		{
			if (stoi(valueF) > 180)
			{
				valueF = "180";
			}
			fov = stoi(valueF);
		}
		else {
			fov = 0;
		}
		
		
		
	
}

void Settings::volumeSlider(int posX, int posY){

	Vector2 mousePos = GetMousePosition();
	Rectangle rectBack = { posX,posY,100*scale.x,10*scale.y };
	Rectangle rectMain = { posX,posY,rectBack.width*volume,10*scale.y};

	DrawRectangleRec(rectBack, LIGHTGRAY);
	DrawRectangleRec(rectMain, BLUE);
	DrawRectangleLinesEx(rectBack, 2, GRAY);

	if (CheckCollisionPointRec(mousePos, rectBack)&& IsMouseButtonDown(MOUSE_LEFT_BUTTON))
	{
		float temp = (mousePos.x - rectBack.x) / rectBack.width;
		if (temp > minV && temp < maxV )
		{
			volume = temp;
		}
	}
}
	
void Settings::diffChoose(int posX,int posY) {

	Vector2 mousePos = GetMousePosition();

	int width = 100*scale.x;
	int height = 40*scale.y;

	Rectangle easy{posX,posY, width,height };
	Rectangle normal{ posX + easy.width + 10*scale.x,posY,width,height };
	Rectangle hard{ posX + easy.width*2 + 20*scale.x,posY,width,height };

	switch (difficulty)
	{
	case 0:
		DrawRectangleRec(easy, DARKGRAY);
		DrawRectangleRec(normal, GRAY);
		DrawRectangleRec(hard, GRAY);
		break;
	case 1:
		DrawRectangleRec(easy, GRAY);
		DrawRectangleRec(normal, DARKGRAY);
		DrawRectangleRec(hard, GRAY);
		break;
	case 2:
		DrawRectangleRec(easy, GRAY);
		DrawRectangleRec(normal, GRAY);
		DrawRectangleRec(hard, DARKGRAY);
		break;
	}

	DrawText("Easy", easy.x + 25*scale.x, easy.y + 10*scale.y , 20*scale.y, WHITE);
	DrawText("Normal", normal.x + 25*scale.x, normal.y + 10*scale.y, 16*scale.y, WHITE);
	DrawText("Hard", hard.x + 25*scale.x, hard.y + 10*scale.y, 20*scale.y, WHITE);


	if (CheckCollisionPointRec(mousePos, easy) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{difficulty = 0;}

	if (CheckCollisionPointRec(mousePos, normal) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{difficulty = 1;}

	if (CheckCollisionPointRec(mousePos, hard) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{difficulty = 2;}

	
}

bool isOpen = false;
void Settings::resolutionChoose(int posX, int posY) {
	Vector2 mousePos = GetMousePosition();
	int marginX = 5*scale.x;
	int marginY = 5 * scale.y;
	int font = 16 * scale.y;
	int width = 100 * scale.x;
	int height = 30 * scale.y;

	Rectangle dropdown = { posX, posY, width, height };
	
	

	DrawRectangleRec(dropdown, GRAY);
	DrawRectangleLinesEx(dropdown, 2, DARKGRAY);
	DrawText(TextFormat("%ix%i", (int)resolution.x, (int)resolution.y), posX + marginX, posY + marginY, font, RAYWHITE);
	DrawText(TextFormat("%ix%i", (int)resolution.x, (int)resolution.y), posX + marginX, posY + marginY, font, RAYWHITE);
	
	
	if ((CheckCollisionPointRec(mousePos, dropdown) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))||isOpen)
	{
		isOpen = true;
		Rectangle res800 = { posX, posY + dropdown.height, width, height };
		Rectangle res1280 = { posX, res800.y + dropdown.height, width, height };
		Rectangle res1366 = { posX, res1280.y + dropdown.height, width, height };
		

		DrawRectangleRec(res800, GRAY);
		DrawRectangleRec(res1280, GRAY);
		DrawRectangleRec(res1366, GRAY);
	

		DrawRectangleLinesEx(res800, 2 ,DARKGRAY);
		DrawRectangleLinesEx(res1280, 2, DARKGRAY);
		DrawRectangleLinesEx(res1366, 2, DARKGRAY);

		DrawText("800x600", res800.x + marginX, res800.y + marginY, font, RAYWHITE);
		DrawText("1280x720", res1280.x + marginX, res1280.y + marginY, font, RAYWHITE);
		DrawText("1366x768", res1366.x + marginX, res1366.y + marginY, font, RAYWHITE);

		if (CheckCollisionPointRec(mousePos, res800) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			resolution.x = 800;
			resolution.y = 600;
			isOpen = false;
		}

		if (CheckCollisionPointRec(mousePos, res1280) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			resolution.x = 1280;
			resolution.y = 720;
			isOpen = false;
		}

		if (CheckCollisionPointRec(mousePos, res1366) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
		{
			resolution.x = 1366;
			resolution.y = 768;
			isOpen = false;
		}
		
	}


}

void Settings::reload()
{
	std::ifstream file(path);
	if (file.is_open())
	{
		file >> jSet;

		fov = jSet["fov"];
		volume = jSet["volume"];
		difficulty = jSet["difficulty"];
		file.close();
	}
	else { std::cout << "Cannot open settings files" << std::endl; }
	SetMasterVolume(volume);
}





void Settings::open() {

	
	Button saveButton{ "./Resources/Textures/ButtonSave.png", {400*scale.x ,300*scale.y}, scale };
	Button exitButton{ "./Resources/Textures/ButtonExitSett.png", { 400*scale.x,400*scale.y}, scale };
	

	bool exit = false;
	float timeBlock = 0.2;

	Music soundtrack = LoadMusicStream("Resources/Audio/SettingsMusic.mp3");
	PlayMusicStream(soundtrack);


	while (!WindowShouldClose() && exit != true)
	{
	UpdateMusicStream(soundtrack);
	BeginDrawing();
	ClearBackground(BLACK);
	DrawText("FOV: ", 60*scale.x,103*scale.y,16*scale.y,WHITE);
	Settings::inputText(80*scale.x, 20*scale.y, 100*scale.x, 100*scale.y, 3);
	DrawText("Volume: ", 60*scale.x, 140*scale.y, 16*scale.y, WHITE);
	Settings::volumeSlider(120*scale.x, 144*scale.y);
	DrawText("Difficulty: ", 60*scale.x, 180*scale.y, 16*scale.y, WHITE);
	Settings::diffChoose(140*scale.x,180*scale.y);
	DrawText("Resolution: ", 60*scale.x , 240*scale.y, 16*scale.y, WHITE);
	resolutionChoose(150*scale.x,240*scale.y);
	saveButton.Draw();
	exitButton.Draw();
	
	if (timeBlock <= 0)
	{
		if (saveButton.isPressed(GetMousePosition(), IsMouseButtonReleased(MOUSE_BUTTON_LEFT)))
		{Settings::save();}
	
		if(exitButton.isPressed(GetMousePosition(), IsMouseButtonReleased(MOUSE_BUTTON_LEFT)))
		{exit = true;}
	}
	else {
		timeBlock -= 1.0 / 60.0;
	}
	
	

	EndDrawing();
	}
	
}



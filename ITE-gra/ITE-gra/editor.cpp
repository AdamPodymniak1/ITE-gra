#include "editor.h"

using json = nlohmann::json;

enum AppState { // Prze³¹czanie scen
    select_mode,
    load_file,
    create_file,
    editor
};

// Generowanie wyœwietlanego obrazu
void Editor::open() {

    const int cellSize = 30; // Rozmiar kafelków
    const std::string basePath = "Resources/Levels/"; // Œcie¿ka do plików json
    bool isActive = true;
     
    AppState state = select_mode; // Pocz¹tkowy stan

    std::string filenameInput;
    std::string loadedFilename;
    std::string errorMessage;

    int newWidth = 10, newHeight = 10; // Domyœlne wymiary nowej mapy
    bool editSize = true; // Prze³¹cznik wyboru wysokoœci i szerokoœci

    std::vector<std::vector<int>> map;
    json j;

    while (!WindowShouldClose() && isActive) {

        // Tworzenie menu
        if (state == select_mode) {
            if (IsKeyPressed(KEY_ONE)) {
                filenameInput.clear();
                errorMessage.clear();
                state = load_file;
            }
            if (IsKeyPressed(KEY_TWO)) {
                filenameInput.clear();
                editSize = true;
                state = create_file;
            }
            if (IsKeyPressed(KEY_THREE)) {
                isActive = false;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("EDYTOR MAP", (GetScreenWidth()-MeasureText("EDYTOR MAP",32))/2, 100, 32, DARKGRAY);
            DrawText("1 - Wczytaj plik", (GetScreenWidth() - MeasureText("1 - Wczytaj plik", 24))/2, 220, 24, BLACK);
            DrawText("2 - Nowy plik", (GetScreenWidth() - MeasureText("2 - Nowy plik", 24))/2, 260, 24, BLACK);
            DrawText("3 - Menu", (GetScreenWidth() - MeasureText("3 - Menu", 24))/2, 300, 24, BLACK);
            EndDrawing();
            continue;
        }

        // Tworzenie wczytywania pliku
        if (state == load_file) {
            int key = GetCharPressed();
            while (key > 0) {
                if (key >= 32 && key <= 125 && filenameInput.size() < 24)
                    filenameInput += (char)key;
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !filenameInput.empty())
                filenameInput.pop_back();

            if (IsKeyPressed(KEY_ENTER)) {
                std::ifstream file(basePath + filenameInput + ".json");
                if (!file.is_open()) {
                    errorMessage = "Nie znaleziono pliku!";
                }
                else {
                    file >> j;
                    map = j["data"];
                    loadedFilename = filenameInput;
                    state = editor;
                }
            }

            if (IsKeyPressed(KEY_LEFT_CONTROL))
                state = select_mode;

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("WCZYTAJ PLIK", (GetScreenWidth()-MeasureText("WCZYTAJ PLIK", 30))/2, 100, 30, DARKGRAY);
            DrawText("Nazwa pliku:", (GetScreenWidth() - MeasureText("Nazwa pliku:", 22))/2, 180, 22, BLACK);
            DrawRectangle((GetScreenWidth()/2) - 200, 220, 400, 40, LIGHTGRAY);
            DrawText(filenameInput.c_str(), (GetScreenWidth() / 2)-190, 230, 24, BLACK);
            DrawText(".json", (GetScreenWidth() / 2)+210, 230, 24, DARKGRAY);

            if (!errorMessage.empty())
                DrawText(errorMessage.c_str(), 300, 470, 22, RED);

            DrawText("ENTER - wczytaj | CTRL - powrot", 10, GetScreenHeight()-30, 20, DARKGRAY);
            EndDrawing();
            continue;
        }

        // Tworzenie nowegp pliku
        if (state == create_file) {

            int key = GetCharPressed();
            while (key > 0) { // Zabezpieczenie siê na poprawnoœæ nazwy dokumentu
                if (key >= 32 && key <= 125 && filenameInput.size() < 24)
                    filenameInput += (char)key;
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !filenameInput.empty()) { // Wykasowanie tekstu
                filenameInput.pop_back();
            }

            // Edycja szerokoœci lub wysokoœci nowego pola
            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) {
                editSize = !editSize;
            }

            // Nadanie maksymalnych wymiarów mapy
            if (IsKeyPressed(KEY_RIGHT)) {
                if (editSize && newWidth < 20) newWidth++;
                if (!editSize && newHeight < 20) newHeight++;
            }

            // Nadanie minimalnych wymiarów mapy
            if (IsKeyPressed(KEY_LEFT)) {
                if (editSize && newWidth > 5) newWidth--;
                if (!editSize && newHeight > 5) newHeight--;
            }

            if (IsKeyPressed(KEY_ENTER) && !filenameInput.empty()) {
                map.assign(newHeight, std::vector<int>(newWidth, 0));
                loadedFilename = filenameInput;
                state = editor;
            }

            if (IsKeyPressed(KEY_LEFT_CONTROL)) {
                state = select_mode;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("NOWA MAPA", (GetScreenWidth()-MeasureText("NOWA MAPA", 30))/2, 100, 30, DARKGRAY);

            DrawText("Nazwa pliku:", (GetScreenWidth()-MeasureText("Nazwa pliku:", 22))/2, 180, 22, BLACK);
            DrawRectangle((GetScreenWidth()/2)-200, 220, 400, 40, LIGHTGRAY);
            DrawText(filenameInput.c_str(), (GetScreenWidth()/2)-190, 230, 24, BLACK);

            DrawText(TextFormat("Szerokosc: %d %s", newWidth, editSize ? "<" : ""), (GetScreenWidth()-MeasureText("Szerokosc: 10 <",24))/2, 430, 24, BLACK);
            DrawText(TextFormat("Wysokosc: %d %s", newHeight, !editSize ? "<" : ""), (GetScreenWidth() - MeasureText("Wysokosc: 10 <", 24)) / 2, 470, 24, BLACK);

            DrawText("GORA/DOL - zmien pole | LEWO/PRAWO - zmiana", 10, GetScreenHeight()-60, 18, DARKGRAY);
            DrawText("ENTER - utworz | CTRL - powrot", 10, GetScreenHeight()-30, 18, DARKGRAY);

            EndDrawing();
            continue;
        }

        // Tworzenie edytora
        int rows = map.size();
        int cols = map[0].size();

        int mapWidthPx = cols * cellSize;
        int mapHeightPx = rows * cellSize;

        int offsetX = (GetScreenWidth() - mapWidthPx) / 2;
        int offsetY = (GetScreenHeight() - mapHeightPx) / 2;

        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {

            int col = (mouse.x - offsetX) / cellSize;
            int row = (mouse.y - offsetY) / cellSize;

            if (row >= 0 && row < rows && col >= 0 && col < cols) { // Zamiania wartoœci pola po naciœniêciu na ni¹
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    map[row][col] = (map[row][col] == 2) ? 0 : 2;
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                    map[row][col] = (map[row][col] == 5) ? 0 : 5;
            }
        }

        if (IsKeyPressed(KEY_ENTER)) { // Zapisywanie mapy
            json out;
            out["data"] = map;
            std::ofstream file(basePath + loadedFilename + ".json");
            file << out.dump(4);
        }

        if (IsKeyPressed(KEY_LEFT_CONTROL))
            state = select_mode;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                Color col = LIGHTGRAY;
                if (map[r][c] == 5) col = RED;
                if (map[r][c] == 2) col = GREEN;

                DrawRectangle(offsetX + c * cellSize,
                    offsetY + r * cellSize,
                    cellSize - 2,
                    cellSize - 2,
                    col);
            }
        }

        DrawText("CTRL - powrot | ENTER - zapisz", 10, GetScreenHeight()-60, 20, DARKGRAY);
        DrawText(TextFormat("Plik: %s.json", loadedFilename.c_str()), 10, GetScreenHeight()-30, 20, DARKGRAY);

        EndDrawing();
    }

}
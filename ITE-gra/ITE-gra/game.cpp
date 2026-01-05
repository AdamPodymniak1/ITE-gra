#include "game.h"
#include "constants.h"
#include <cmath>
#include <iostream>
#include <algorithm>

#include "game_texture.h"
#include "projectile.h"
#include "monster.h"
#include "sprite.h"
#include "level.h"
#include "keystate.h"
#include "button.h"
#include "settings.h"

#include <nlohmann/json.hpp>
#include <vector>
#include <fstream>

Settings settings;

using json = nlohmann::json;


Game::Game() : currentLevel(0), weaponSprite(nullptr), isShooting(false),
equippedWeapon(1), ammo(100), rocketammo(10),
lastShot(0), shootCooldown(0.6), bulletHitboxRadius(0.25),
bulletRange(400), knifeRange(1), bulletStartDistance(0.5),
monsterMoveSpeed(0.02), activationDistance(1.0),
monsterTotal(0), monsterDefeated(0), pickupTotal(0), pickupCollected(0),
wallTexture(nullptr), floorTexture(nullptr), skyTexture(nullptr),
bulletTexture(nullptr), rocketTexture(nullptr), inboundrocketTexture(nullptr),
knifeTexture(nullptr), gunTexture(nullptr), machinegunTexture(nullptr),
rocketlauncherTexture(nullptr), skeletonTexture(nullptr),
ammoTexture(nullptr), rocketammoTexture(nullptr), bonesTexture(nullptr),
cloudTexture(nullptr) {

    settings.loadAll();

    SetMasterVolume(settings.volume);

    screen.width = settings.resolution.x;
    screen.height = settings.resolution.y;
    screen.scale = 4;
    screen.halfWidth = screen.width / 2;
    screen.halfHeight = screen.height / 2;

    projection.width = screen.width / screen.scale;
    projection.height = screen.height / screen.scale;
    projection.halfWidth = projection.width / 2;
    projection.halfHeight = projection.height / 2;

    render.frameTime = 1.0 / 60.0;
    render.lastUpdate = 0;

    rayCasting.precision = 64;

    player.fov = settings.fov;
    player.halfFov = player.fov / 2;
    player.x = 2;
    player.y = 2;
    player.angle = 0;
    player.radius = 20;
   
    switch (settings.difficulty)
    {
    case 0:
        player.health = 200;
        player.maxHealth = 200;
        break;
    case 1:
        player.health = 100;
        player.maxHealth = 100;
        break;
    case 2:
        player.health = 50;
        player.maxHealth = 50;
        break;
    }


    player.speed.movement = 0.08;
    player.speed.rotation = 1.5;

    keys["up"] = KeyState(KEY_UP);
    keys["down"] = KeyState(KEY_DOWN);
    keys["left"] = KeyState(KEY_LEFT);
    keys["right"] = KeyState(KEY_RIGHT);
    keys["space"] = KeyState(KEY_SPACE);
    keys["one"] = KeyState(KEY_ONE);
    keys["two"] = KeyState(KEY_TWO);
    keys["three"] = KeyState(KEY_THREE);
    keys["four"] = KeyState(KEY_FOUR);
    keys["strafeleft"] = KeyState(KEY_A);
    keys["straferight"] = KeyState(KEY_D);

    std::ifstream file("Resources/Levels/level1.json");
    if (!file.is_open()) {
        std::cerr << "Nie mozna otworzyc pliku level1.json" << std::endl;
    }

    json j;
    file >> j;

    Level level1;
    level1.name = "Default";
    level1.map = j["data"];
    levels.push_back(level1);

    rayCasting.incrementAngle = player.fov / projection.width;
}

Game::~Game() {
    cleanup();
}

int Game::menu() {
   

    Vector2 startButPos;
    startButPos.x = screen.halfWidth-(int)(screen.halfWidth*0.2);
    startButPos.y = (int)(screen.height * 0.25-screen.halfHeight*0.35);

    Vector2 mapButPos;
    mapButPos.x = screen.halfWidth - (int)(screen.halfWidth * 0.2);
    mapButPos.y = (int)(screen.height * 0.5 - screen.halfHeight * 0.35);

    Vector2 settingsButPos;
    settingsButPos.x = screen.halfWidth - (int)(screen.halfWidth * 0.2);
    settingsButPos.y = (int)(screen.height * 0.75 - screen.halfHeight * 0.35);

    Vector2 exitButPos;
    exitButPos.x = screen.halfWidth - (int)(screen.halfWidth * 0.2);
    exitButPos.y = screen.height - (int)screen.halfHeight * 0.35;

    Button startButton("Resources/Textures/ButtonStart.png", startButPos,settings.scale);
    Button mapBuildButton("Resources/Textures/ButtonMapBuilder.png", mapButPos,settings.scale);
    Button settingsButton("Resources/Textures/ButtonSettings.png", settingsButPos, settings.scale);
    Button exitButton("Resources/Textures/ButtonExit.png", exitButPos, settings.scale);

  
    Vector2 mousePos;
    bool isLPress;

        while (!WindowShouldClose()) {

        mousePos = GetMousePosition();
        isLPress = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

        BeginDrawing();
        ClearBackground(BLACK);
        startButton.Draw();
        mapBuildButton.Draw();
        settingsButton.Draw();
        exitButton.Draw();
            if (startButton.isPressed(mousePos,isLPress))
            {
                return 1;
            }
            if (mapBuildButton.isPressed(mousePos, isLPress))
            {
                return 2;
            }
            if (settingsButton.isPressed(mousePos, isLPress))
            {
                return 3;
            }
            if (exitButton.isPressed(mousePos, isLPress))
            {
                return 4;
            }
       
        EndDrawing();
     }
        return 0;
   
}

bool Game::initialize() {
    InitWindow(screen.width, screen.height, "ITE gra");
    InitAudioDevice();
    SetTargetFPS(60);

    if (!IsWindowReady()) {
        return false;
    }

    projection.buffer = GenImageColor(projection.width, projection.height, BLACK);
    projection.bufferData = LoadImageColors(projection.buffer);

    if (!loadAllTextures()) {
        std::cout << "Warning: Some textures failed to load" << std::endl;
    }

    if (!loadAllSounds()) {
        std::cout << "Warning: Some sounds failed to load" << std::endl;
    }

    setWeaponTexture(equippedWeapon);

    return true;
}

GameTexture* Game::loadTexture(const std::string& path, const std::string& name) {
    std::string textureName = name.empty() ? path : name;

    if (textureMap.find(textureName) != textureMap.end())
        return textureMap[textureName];

    Image img = LoadImage(path.c_str());
    if (img.data == nullptr)
        return nullptr;

    GameTexture* tex = new GameTexture();
    tex->texture = LoadTextureFromImage(img);
    tex->width = img.width;
    tex->height = img.height;
    tex->pixels = LoadImageColors(img);

    UnloadImage(img);

    textureMap[textureName] = tex;
    return tex;
}


bool Game::loadAllTextures() {
    wallTexture = loadTexture("Resources/Textures/texture.png", "wall");
    skyTexture = loadTexture("Resources/Textures/night.png", "sky");

    knifeTexture = loadTexture("Resources/Textures/knife.png", "knife");
    gunTexture = loadTexture("Resources/Textures/gun.png", "gun");
    machinegunTexture = loadTexture("Resources/Textures/machinegun.png", "machinegun");
    rocketlauncherTexture = loadTexture("Resources/Textures/rocketlauncher.png", "rocketlauncher");

    bulletTexture = loadTexture("Resources/Textures/bullet.png", "bullet");
    rocketTexture = loadTexture("Resources/Textures/rocket.png", "rocket");
    inboundrocketTexture = loadTexture("Resources/Textures/inboundrocket.png", "inboundrocket");

    skeletonTexture = loadTexture("Resources/Textures/skeleton.png", "skeleton");
    ammoTexture = loadTexture("Resources/Textures/ammo.png", "ammo");
    rocketammoTexture = loadTexture("Resources/Textures/rocketammo.png", "rocketammo");
    bonesTexture = loadTexture("Resources/Textures/bones.png", "bones");

    return true;
}

bool Game::loadAllSounds() {
    audioCache["shoot"] = LoadSound("Resources/Sounds/shoot.mp3");
    audioCache["knife"] = LoadSound("Resources/Sounds/knife.mp3");
    audioCache["rocketlaunch"] = LoadSound("Resources/Sounds/rocketlaunch.mp3");
    audioCache["explosion"] = LoadSound("Resources/Sounds/explosion.mp3");
    audioCache["gunclick"] = LoadSound("Resources/Sounds/gunclick.mp3");
    audioCache["pickup"] = LoadSound("Resources/Sounds/pickup.mp3");
    audioCache["injured"] = LoadSound("Resources/Sounds/injured.mp3");
    audioCache["death"] = LoadSound("Resources/Sounds/death.mp3");
    audioCache["skeleton-pain-1"] = LoadSound("Resources/Sounds/skeleton-pain-1.mp3");
    audioCache["skeleton-pain-2"] = LoadSound("Resources/Sounds/skeleton-pain-2.mp3");
    audioCache["skeleton-pain-3"] = LoadSound("Resources/Sounds/skeleton-pain-3.mp3");
    audioCache["skeleton-death"] = LoadSound("Resources/Sounds/skeleton-death.mp3");

    return true;
}

GameTexture* Game::getTexture(const std::string& name) {
    auto it = textureMap.find(name);
    if (it != textureMap.end()) {
        return it->second;
    }
    return nullptr;
}

void Game::setWeaponTexture(int weaponId) {
    switch (weaponId) {
    case 1: weaponSprite = knifeTexture; break;
    case 2: weaponSprite = gunTexture; break;
    case 3: weaponSprite = machinegunTexture; break;
    case 5: weaponSprite = rocketlauncherTexture; break;
    default: weaponSprite = knifeTexture; break;
    }

    if (!weaponSprite || weaponSprite->texture.id == 0) {
        weaponSprite = new GameTexture();
        Image img = GenImageColor(100, 100, RED);
        weaponSprite->texture = LoadTextureFromImage(img);
        weaponSprite->width = 100;
        weaponSprite->height = 100;
        UnloadImage(img);
    }
}

void Game::drawTexturedWall(int rayCount, int wallHeight, double rayX, double rayY) {
    if (!wallTexture || !wallTexture->pixels) return;

    double hitX = rayX - floor(rayX);
    double hitY = rayY - floor(rayY);

    int texX;
    if (hitX < 0.01 || hitX > 0.99)
        texX = (int)(hitY * wallTexture->width);
    else
        texX = (int)(hitX * wallTexture->width);

    texX = std::clamp(texX, 0, wallTexture->width - 1);

    float texStep = (float)wallTexture->height / (wallHeight * 2);
    float texPos = 0;

    int startY = projection.halfHeight - wallHeight;
    if (startY < 0) startY = 0;
    int endY = projection.halfHeight + wallHeight;
    if (endY > projection.height) endY = projection.height;

    for (int y = startY; y < endY; y++) {
        int texY = (int)texPos & (wallTexture->height - 1);
        int index = texY * wallTexture->width + texX;

        drawPixel(rayCount, y, wallTexture->pixels[index]);
        texPos += texStep;
    }
}


void Game::loadLevel(int levelIdx) {
    if (levelIdx < 0 || levelIdx >= levels.size()) return;

    currentLevel = levelIdx;
    player.x = levels[levelIdx].startlocation.x;
    player.y = levels[levelIdx].startlocation.y;
    player.angle = 0;
    equippedWeapon = levels[levelIdx].equippedweapon;
    setWeapon(equippedWeapon);

    monsters.clear();
    sprites.clear();
    projectiles.clear();

    monsterTotal = 0;
    monsterDefeated = 0;
    pickupTotal = 0;
    pickupCollected = 0;

    std::vector<std::vector<int>>& map = levels[levelIdx].map;
    int mapy = (int)map.size();
    int mapx = (int)map[0].size();
    monsterMoveSpeed = levels[levelIdx].monstermovespeed;

    for (int i = 0; i < mapy; i++) {
        for (int j = 0; j < mapx; j++) {
            switch (map[i][j]) {
            case 1:
            {
                Sprite tree("tree", (double)j, (double)i, 8, 16);
                tree.texture = cloudTexture;
                sprites.push_back(tree);
                break;
            }
            case 3:
            {
                Sprite ammoSprite("ammo", (double)j, (double)i, 100, 81);
                ammoSprite.texture = ammoTexture;
                sprites.push_back(ammoSprite);
                pickupTotal++;
                break;
            }
            case 4:
            {
                Sprite rocketSprite("rocketammo", (double)j, (double)i, 35, 18);
                rocketSprite.texture = rocketammoTexture;
                sprites.push_back(rocketSprite);
                pickupTotal++;
                break;
            }
            case 5:
            {
                Monster skeleton;
                skeleton.id = "monster_" + std::to_string(monsterTotal);
                skeleton.type = "skeleton";
                skeleton.skin = "skeleton";
                skeleton.audio = "skeleton";
                skeleton.x = (double)j;
                skeleton.y = (double)i;
                skeleton.health = 100;
                skeleton.isDead = false;
                skeleton.width = 512;
                skeleton.height = 512;
                skeleton.damage = 1;
                skeleton.attackCooldown = 1.0;
                skeleton.texture = skeletonTexture;
                monsters.push_back(skeleton);
                monsterTotal++;
                break;
            }
            }
        }
    }
}

void Game::run() {
    loadLevel(0);

    

    RenderTexture2D renderTex = LoadRenderTexture(projection.width, projection.height);
    Texture2D projectionTexture = LoadTextureFromImage(projection.buffer);
  
        while (!WindowShouldClose()) {
       

        double currentTime = GetTime();
        render.lastUpdate = currentTime;

        for (std::map<std::string, KeyState>::iterator it = keys.begin(); it != keys.end(); ++it) {
            it->second.active = IsKeyDown(it->second.key);
        }

        movePlayer();
        updateGameObjects();

        clearScreen();
        rayCastingFunc();

        for (size_t i = 0; i < sprites.size(); i++) {
            drawSpriteInWorld(sprites[i]);
        }

        for (size_t i = 0; i < projectiles.size(); i++) {
            drawBulletSprite(projectiles[i]);
        }

        UpdateTexture(projectionTexture, projection.bufferData);

        BeginTextureMode(renderTex);
        ClearBackground(BLACK);
        DrawTexture(projectionTexture, 0, 0, WHITE);
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexturePro(renderTex.texture,
            Rectangle{ 0, 0, (float)projection.width, -(float)projection.height },
            Rectangle{ 0, 0, (float)screen.width, (float)screen.height },
            Vector2{ 0, 0 }, 0, WHITE);

        for (size_t i = 0; i < monsters.size(); i++) {
            drawSpriteInWorld(monsters[i]);
        }

        if (weaponSprite && weaponSprite->texture.id > 0) {
            int weaponY = screen.height - 155 * screen.scale;
            if (isShooting) {
                weaponY = screen.height - 170 * screen.scale;
                isShooting = false;
            }
            DrawTexturePro(weaponSprite->texture,
                Rectangle{ 0, 0, (float)weaponSprite->getWidth(), (float)weaponSprite->getHeight() },
                Rectangle{ screen.width / 2.0f - 80 * screen.scale, (float)weaponY,
                 160.0f * screen.scale, 160.0f * screen.scale },
                Vector2{ 0, 0 }, 0, WHITE);
        }

        DrawRectangle(0, 0, 80 * screen.scale, 22 * screen.scale, Color{ 0, 0, 0, 128 });

        std::string healthText = "Health: " + std::to_string(player.health);
        std::string weaponText;
        switch (equippedWeapon) {
        case 1: weaponText = "Knife"; break;
        case 2: weaponText = "Pistol"; break;
        case 3: weaponText = "Machine Gun"; break;
        case 5: weaponText = "Rocket Launcher"; break;
        default: weaponText = "Unknown"; break;
        }
        std::string ammoText = "Ammo: ";
        if (equippedWeapon == 1 || equippedWeapon == 4 || equippedWeapon == 6) {
            ammoText += "INF";
        }
        else if (equippedWeapon == 5) {
            ammoText += std::to_string(rocketammo);
        }
        else {
            ammoText += std::to_string(ammo);
        }

        DrawText(healthText.c_str(), 5 * screen.scale, 5 * screen.scale, 5 * screen.scale, WHITE);
        DrawText(("Weapon: " + weaponText).c_str(), 5 * screen.scale, 25 * screen.scale, 5 * screen.scale, WHITE);
        DrawText(ammoText.c_str(), 5 * screen.scale, 45 * screen.scale, 5 * screen.scale, WHITE);

        EndDrawing();
        
    }

    UnloadTexture(projectionTexture);
    UnloadRenderTexture(renderTex);
}

void Game::cleanup() {
    if (weaponSprite && weaponSprite != knifeTexture &&
        weaponSprite != gunTexture && weaponSprite != machinegunTexture &&
        weaponSprite != rocketlauncherTexture) {
        if (weaponSprite->texture.id > 0) {
            UnloadTexture(weaponSprite->texture);
        }
        delete weaponSprite;
    }

    for (auto& pair : textureMap) {
        if (pair.second && pair.second->texture.id > 0) {
            UnloadTexture(pair.second->texture);
        }
        delete pair.second;
    }
    textureMap.clear();

    for (std::map<std::string, Sound>::iterator it = audioCache.begin(); it != audioCache.end(); ++it) {
        UnloadSound(it->second);
    }

    UnloadImageColors(projection.bufferData);
    UnloadImage(projection.buffer);

    CloseAudioDevice();
    CloseWindow();
}

void Game::drawPixel(int x, int y, Color color) {
    if (color.r == 255 && color.g == 0 && color.b == 255) return;
    if (x >= 0 && x < projection.width && y >= 0 && y < projection.height) {
        projection.bufferData[y * projection.width + x] = color;
    }
}

void Game::clearScreen() {
    for (int i = 0; i < projection.width * projection.height; i++) {
        projection.bufferData[i] = BLACK;
    }
}

bool Game::isVisibleToPlayer(const Monster& monster) {
    const std::vector<std::vector<int>>& map = levels[currentLevel].map;
    double x0 = player.x;
    double y0 = player.y;
    double x1 = monster.x;
    double y1 = monster.y;
    double dx = x1 - x0;
    double dy = y1 - y0;
    double steps = std::max(fabs(dx), fabs(dy)) * 4;

    for (int step = 0; step < steps; step++) {
        double t = step / steps;
        double x = x0 + dx * t;
        double y = y0 + dy * t;
        int mapX = (int)x;
        int mapY = (int)y;

        if (mapY >= 0 && mapY < (int)map.size() && mapX >= 0 && mapX < (int)map[mapY].size()) {
            if (map[mapY][mapX] == 2) {
                return false;
            }

            if ((int)x == (int)x1 && (int)y == (int)y1) {
                return true;
            }
        }
    }
    return false;
}

void Game::rayCastingFunc() {
    const std::vector<std::vector<int>>& currentMap = levels[currentLevel].map;
    int rayPrecision = rayCasting.precision;
    int projectionHalfHeight = projection.halfHeight;

    double rayAngle = player.angle - player.halfFov;
    double angleIncrement = rayCasting.incrementAngle;

    double playerX = player.x;
    double playerY = player.y;

    for (int rayCount = 0; rayCount < projection.width; rayCount++) {
        double rayAngleRad = degreeToRadians(rayAngle);
        double rayCos = cos(rayAngleRad) / rayPrecision;
        double raySin = sin(rayAngleRad) / rayPrecision;

        double rayX = playerX;
        double rayY = playerY;

        int wall;
        do {
            rayX += rayCos;
            rayY += raySin;
            int mapX = (int)rayX;
            int mapY = (int)rayY;

            if (mapY >= 0 && mapY < (int)currentMap.size() &&
                mapX >= 0 && mapX < (int)currentMap[mapY].size()) {
                wall = currentMap[mapY][mapX];
            }
            else {
                wall = 2;
            }
        } while (wall != 2);

        double dx = rayX - playerX;
        double dy = rayY - playerY;
        double distance = sqrt(dx * dx + dy * dy) *
            cos(degreeToRadians(rayAngle - player.angle));

        int wallHeight = (int)(projectionHalfHeight / distance);

        for (int y = 0; y < projectionHalfHeight - wallHeight; y++) {
            int skyX = (int)((rayAngle / player.fov) * skyTexture->width) % skyTexture->width;
            int skyY = (int)((float)y / projection.halfHeight * skyTexture->height);
            drawPixel(rayCount, y, skyTexture->pixels[skyY * skyTexture->width + skyX]);
        }

        drawTexturedWall(rayCount, wallHeight, rayX, rayY);

        for (int y = projectionHalfHeight + wallHeight; y < projection.height; y++) {
            drawPixel(rayCount, y, Color{ 50, 50, 50, 255 });
        }

        rayAngle += angleIncrement;
    }
}

void Game::drawSpriteInWorld(const Sprite& sprite) {
    if (!sprite.texture || sprite.texture->texture.id == 0) return;

    double dx = sprite.x - player.x;
    double dy = sprite.y - player.y;
    double distance = sqrt(dx*dx + dy*dy);
    if (distance < 0.5) return;

    double angle = atan2(dy, dx);
    double screenX =
        (radiansToDegrees(angle) - (player.angle - player.halfFov)) *
        projection.width / player.fov;

    int size = (int)(projection.halfHeight / distance);

    Rectangle src {
        0, 0,
        (float)sprite.texture->getWidth(),
        (float)sprite.texture->getHeight()
    };

    Rectangle dst {
        (float)(screenX - size / 2),
        (float)(projection.halfHeight - size / 2),
        (float)size,
        (float)size
    };

    DrawTexturePro(sprite.texture->texture, src, dst, {0,0}, 0, WHITE);
}


void Game::drawSpriteInWorld(const Monster& monster) {
    if (!monster.texture || monster.texture->texture.id == 0 || monster.isDead) return;

    if (!isVisibleToPlayer(monster)) return;

    double relX = monster.x - player.x;
    double relY = monster.y - player.y;

    double distance = sqrt(relX * relX + relY * relY);
    if (distance < 0.5) return;

    double angleToMonster = atan2(relY, relX);
    double angleDiff = angleToMonster - degreeToRadians(player.angle);

    while (angleDiff > PI) angleDiff -= 2.0 * PI;
    while (angleDiff < -PI) angleDiff += 2.0 * PI;

    double halfFovRad = degreeToRadians(player.halfFov);
    if (angleDiff < -halfFovRad || angleDiff > halfFovRad) return;

    double screenXProj =
        (angleDiff + halfFovRad) / (2.0 * halfFovRad) * projection.width;

    double screenX = screenXProj * screen.scale;
    double screenY = projection.halfHeight * screen.scale;

    int monsterHeight = (int)(projection.halfHeight / distance);
    int monsterWidth =
        (int)((double)monster.texture->width / monster.texture->height * monsterHeight);

    monsterHeight *= screen.scale;
    monsterWidth *= screen.scale;

    Rectangle src = {
        0, 0,
        (float)monster.texture->width,
        (float)monster.texture->height
    };

    Rectangle dst = {
        (float)(screenX - monsterWidth / 2),
        (float)(screenY - monsterHeight / 2),
        (float)monsterWidth,
        (float)monsterHeight
    };

    DrawTexturePro(monster.texture->texture, src, dst, { 0, 0 }, 0, WHITE);
}

void Game::drawBulletSprite(const Projectile& projectile) {
    double projXRelative = projectile.x - player.x;
    double projYRelative = projectile.y - player.y;

    double distance = sqrt(projXRelative * projXRelative + projYRelative * projYRelative);
    if (distance < 0.5) return;

    double angle = atan2(projYRelative, projXRelative);
    double screenX = (radiansToDegrees(angle) - (player.angle - player.halfFov)) * projection.width / player.fov;

    while (screenX < 0) screenX += projection.width;
    while (screenX >= projection.width) screenX -= projection.width;

    int projSize = 4;
    if (projectile.type == "rocket") projSize = 6;
    if (distance > 10) {
        projSize = (int)(4 / (distance / 10));
        if (projSize < 1) projSize = 1;
    }

    int drawX = (int)screenX - projSize / 2;
    int drawY = projection.halfHeight - projSize / 2;

    Color color = YELLOW;
    if (projectile.type == "rocket") color = ORANGE;

    for (int x = 0; x < projSize; x++) {
        for (int y = 0; y < projSize; y++) {
            int pixelX = drawX + x;
            int pixelY = drawY + y;

            if (pixelX >= 0 && pixelX < projection.width &&
                pixelY >= 0 && pixelY < projection.height) {
                drawPixel(pixelX, pixelY, color);
            }
        }
    }
}

void Game::movePlayer() {
    std::vector<std::vector<int>>& map = levels[currentLevel].map;
    int mapHeight = (int)map.size();
    int mapWidth = (int)map[0].size();

    if (keys["up"].active) {
        double playerCos = cos(degreeToRadians(player.angle)) * player.speed.movement;
        double playerSin = sin(degreeToRadians(player.angle)) * player.speed.movement;
        double newX = player.x + playerCos;
        double newY = player.y + playerSin;

        int checkX = (int)(newX + playerCos * player.radius);
        int checkY = (int)(newY + playerSin * player.radius);
        int mathfloorX = (int)player.x;
        int mathfloorY = (int)player.y;

        if (checkY >= 0 && checkY < mapHeight && mathfloorX >= 0 && mathfloorX < mapWidth &&
            map[checkY][mathfloorX] != 2) {
            player.y = newY;
        }
        if (mathfloorY >= 0 && mathfloorY < mapHeight && checkX >= 0 && checkX < mapWidth &&
            map[mathfloorY][checkX] != 2) {
            player.x = newX;
        }
    }

    if (keys["down"].active) {
        double playerCos = cos(degreeToRadians(player.angle)) * player.speed.movement;
        double playerSin = sin(degreeToRadians(player.angle)) * player.speed.movement;
        double newX = player.x - playerCos;
        double newY = player.y - playerSin;

        int checkX = (int)(newX - playerCos * player.radius);
        int checkY = (int)(newY - playerSin * player.radius);
        int mathfloorX = (int)player.x;
        int mathfloorY = (int)player.y;

        if (checkY >= 0 && checkY < mapHeight && mathfloorX >= 0 && mathfloorX < mapWidth &&
            map[checkY][mathfloorX] != 2) {
            player.y = newY;
        }
        if (mathfloorY >= 0 && mathfloorY < mapHeight && checkX >= 0 && checkX < mapWidth &&
            map[mathfloorY][checkX] != 2) {
            player.x = newX;
        }
    }

    if (keys["left"].active) {
        player.angle -= player.speed.rotation;
        if (player.angle < 0) player.angle += 360;
        while (player.angle >= 360) player.angle -= 360;
    }

    if (keys["right"].active) {
        player.angle += player.speed.rotation;
        if (player.angle < 0) player.angle += 360;
        while (player.angle >= 360) player.angle -= 360;
    }

    if (keys["space"].active) {
        handleShooting();
    }

    if (keys["strafeleft"].active) {
        double strafeAngle = player.angle - 90;
        double playerCos = cos(degreeToRadians(strafeAngle)) * player.speed.movement;
        double playerSin = sin(degreeToRadians(strafeAngle)) * player.speed.movement;
        double newX = player.x + playerCos;
        double newY = player.y + playerSin;

        int checkX = (int)newX;
        int checkY = (int)newY;
        int mathfloorX = (int)player.x;
        int mathfloorY = (int)player.y;

        if (checkY >= 0 && checkY < mapHeight && mathfloorX >= 0 && mathfloorX < mapWidth &&
            map[checkY][mathfloorX] != 2) {
            player.y = newY;
        }
        if (mathfloorY >= 0 && mathfloorY < mapHeight && checkX >= 0 && checkX < mapWidth &&
            map[mathfloorY][checkX] != 2) {
            player.x = newX;
        }
    }

    if (keys["straferight"].active) {
        double strafeAngle = player.angle + 90;
        double playerCos = cos(degreeToRadians(strafeAngle)) * player.speed.movement;
        double playerSin = sin(degreeToRadians(strafeAngle)) * player.speed.movement;
        double newX = player.x + playerCos;
        double newY = player.y + playerSin;

        int checkX = (int)newX;
        int checkY = (int)newY;
        int mathfloorX = (int)player.x;
        int mathfloorY = (int)player.y;

        if (checkY >= 0 && checkY < mapHeight && mathfloorX >= 0 && mathfloorX < mapWidth &&
            map[checkY][mathfloorX] != 2) {
            player.y = newY;
        }
        if (mathfloorY >= 0 && mathfloorY < mapHeight && checkX >= 0 && checkX < mapWidth &&
            map[mathfloorY][checkX] != 2) {
            player.x = newX;
        }
    }

    int playerMapX = (int)player.x;
    int playerMapY = (int)player.y;

    if (playerMapY >= 0 && playerMapY < mapHeight &&
        playerMapX >= 0 && playerMapX < mapWidth) {
        int tile = map[playerMapY][playerMapX];
        if (tile == 8 || tile == 9 || tile == 10 || tile == 11 || tile == 13) {
            map[playerMapY][playerMapX] = 0;
            itemPickup(playerMapY, playerMapX);
            pickupCollected++;

            if (tile == 8) {
                ammo += 8;
            }
            else if (tile == 13) {
                rocketammo += 4;
            }
        }
    }

    if (keys["one"].active) {
        equippedWeapon = 1;
        shootCooldown = 0.6;
        setWeaponTexture(1);
    }
    if (keys["two"].active) {
        equippedWeapon = 2;
        shootCooldown = 0.85;
        setWeaponTexture(2);
    }
    if (keys["three"].active) {
        equippedWeapon = 3;
        shootCooldown = 0.4;
        setWeaponTexture(3);
    }
    if (keys["four"].active) {
        equippedWeapon = 5;
        shootCooldown = 1.2;
        setWeaponTexture(5);
    }
}

void Game::handleShooting() {
    double currentTime = GetTime();
    if (currentTime - lastShot >= shootCooldown) {
        isShooting = true;
        lastShot = currentTime;

        if ((equippedWeapon == 2 || equippedWeapon == 3) && ammo <= 0) {
            playSound("gunclick");
            return;
        }
        if (equippedWeapon == 5 && rocketammo <= 0) {
            playSound("gunclick");
            return;
        }

        double startX = player.x + cos(degreeToRadians(player.angle)) * bulletStartDistance;
        double startY = player.y + sin(degreeToRadians(player.angle)) * bulletStartDistance;

        std::string type;
        GameTexture* projTexture = nullptr;

        if (equippedWeapon == 1) {
            type = "knife";
            playSound("knife");
            projTexture = nullptr;
        }
        else if (equippedWeapon == 5) {
            type = "rocket";
            playSound("rocketlaunch");
            rocketammo--;
            projTexture = rocketTexture;
        }
        else {
            type = "bullet";
            playSound("shoot");
            ammo--;
            projTexture = bulletTexture;
        }

        if (projTexture) {
            projectiles.push_back(Projectile(startX, startY, player.angle, type, projTexture, "player"));
        }
    }
}

void Game::updateGameObjects() {
    std::vector<std::vector<int>>& map = levels[currentLevel].map;
    int mapHeight = (int)map.size();
    int mapWidth = (int)map[0].size();

    std::vector<int> projectilesToRemove;
    for (int i = 0; i < (int)projectiles.size(); i++) {
        Projectile& projectile = projectiles[i];
        projectile.update();

        int mapX = (int)projectile.x;
        int mapY = (int)projectile.y;

        if (mapY >= 0 && mapY < mapHeight && mapX >= 0 && mapX < mapWidth) {
            if (map[mapY][mapX] == 2) {
                if (projectile.type == "rocket") {
                    playSound("explosion");
                }
                projectilesToRemove.push_back(i);
                continue;
            }
        }

        if (projectile.owner == "player") {
            for (size_t j = 0; j < monsters.size(); j++) {
                Monster& monster = monsters[j];
                if (!monster.isDead) {
                    double dx = monster.x - projectile.x;
                    double dy = monster.y - projectile.y;
                    double distanceSq = dx * dx + dy * dy;

                    if (distanceSq < bulletHitboxRadius * bulletHitboxRadius) {
                        if (projectile.type == "rocket") {
                            monster.health -= 100;
                        }
                        else {
                            monster.health -= 25;
                        }

                        if (monster.health <= 0) {
                            monster.isDead = true;
                            monsterDefeated++;
                            playSound("skeleton-death");
                        }
                        else {
                            int rnd = rand() % 3;
                            if (rnd == 0) playSound("skeleton-pain-1");
                            else if (rnd == 1) playSound("skeleton-pain-2");
                            else playSound("skeleton-pain-3");
                        }

                        projectilesToRemove.push_back(i);
                        break;
                    }
                }
            }
        }

        double distSq = (projectile.x - player.x) * (projectile.x - player.x) +
            (projectile.y - player.y) * (projectile.y - player.y);

        if (projectile.type == "knife") {
            if (distSq > knifeRange * knifeRange) {
                projectilesToRemove.push_back(i);
            }
        }
        else {
            if (distSq > bulletRange * bulletRange) {
                if (projectile.type == "rocket") {
                    playSound("explosion");
                }
                projectilesToRemove.push_back(i);
            }
        }
    }

    for (int i = (int)projectilesToRemove.size() - 1; i >= 0; i--) {
        int idx = projectilesToRemove[i];
        if (idx < (int)projectiles.size()) {
            projectiles.erase(projectiles.begin() + idx);
        }
    }

    double currentTime = GetTime();
    for (size_t i = 0; i < monsters.size(); i++) {
        Monster& monster = monsters[i];
        if (!monster.isDead) {
            double dx = player.x - monster.x;
            double dy = player.y - monster.y;
            double distSq = dx * dx + dy * dy;

            if (distSq > 0.25 && distSq < 100) {
                double distance = sqrt(distSq);
                double invDist = 1.0 / distance;
                double dirX = dx * invDist * monsterMoveSpeed;
                double dirY = dy * invDist * monsterMoveSpeed;

                double newX = monster.x + dirX;
                double newY = monster.y + dirY;

                int mapX = (int)newX;
                int mapY = (int)monster.y;
                if (mapY >= 0 && mapY < mapHeight && mapX >= 0 && mapX < mapWidth) {
                    if (map[mapY][mapX] != 2) {
                        monster.x = newX;
                    }
                }

                mapX = (int)monster.x;
                mapY = (int)newY;
                if (mapY >= 0 && mapY < mapHeight && mapX >= 0 && mapX < mapWidth) {
                    if (map[mapY][mapX] != 2) {
                        monster.y = newY;
                    }
                }
            }

            if (distSq < 0.5 && (currentTime - monster.lastAttack >= monster.attackCooldown)) {
                player.health -= monster.damage;
                monster.lastAttack = currentTime;
                playSound("injured");

                if (player.health <= 0) {
                    playSound("death");
                }
            }
        }
    }
}

void Game::setWeapon(int id) {
    equippedWeapon = id;
    switch (id) {
    case 1:
        shootCooldown = 0.6;
        setWeaponTexture(1);
        break;
    case 2:
        shootCooldown = 0.85;
        setWeaponTexture(2);
        break;
    case 3:
        shootCooldown = 0.4;
        setWeaponTexture(3);
        break;
    case 5:
        shootCooldown = 1.2;
        setWeaponTexture(5);
        break;
    }
}

void Game::itemPickup(int ycoords, int xcoords) {
    playSound("pickup");
    for (std::vector<Sprite>::iterator it = sprites.begin(); it != sprites.end(); ++it) {
        if ((int)it->x == xcoords && (int)it->y == ycoords) {
            sprites.erase(it);
            break;
        }
    }
}

void Game::playSound(const std::string& id) {
    std::map<std::string, Sound>::iterator it = audioCache.find(id);
    if (it != audioCache.end()) {
        PlaySound(it->second);
    }
}
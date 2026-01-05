#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "game_texture.h"
#include "projectile.h"
#include "monster.h"
#include "sprite.h"
#include "level.h"
#include "keystate.h"

class Game {
private:
    int currentLevel;
    std::vector<Monster> monsters;
    std::vector<Sprite> sprites;
    std::vector<Projectile> projectiles;
    GameTexture* weaponSprite;
    bool isShooting;
    int equippedWeapon;
    int ammo;
    int rocketammo;
    double lastShot;
    double shootCooldown;
    double bulletHitboxRadius;
    double bulletRange;
    double knifeRange;
    double bulletStartDistance;
    double monsterMoveSpeed;
    double activationDistance;
    int monsterTotal;
    int monsterDefeated;
    int pickupTotal;
    int pickupCollected;
    int currentWave = 0;
    bool waveActive = false;
    double nextWaveTime = 0;
    double waveDelay = 5.0;
    std::vector<Vector2> skeletonSpawnPoints;
    int skeletonsPerWave = 3;

    bool gameOver = false;

    struct {
        int width;
        int height;
        int halfWidth;
        int halfHeight;
        int scale;
    } screen;

    struct {
        int width;
        int height;
        int halfWidth;
        int halfHeight;
        Image buffer;
        Color* bufferData;
    } projection;

    struct {
        double frameTime;
        double lastUpdate;
    } render;

    struct {
        double incrementAngle;
        int precision;
    } rayCasting;

    struct {
        double fov;
        double halfFov;
        double x, y;
        double angle;
        double radius;
        int health;
        int maxHealth;
        struct {
            double movement;
            double rotation;
        } speed;
    } player;

    std::vector<Level> levels;
    std::map<std::string, KeyState> keys;
    std::map<std::string, GameTexture*> textureMap;
    std::map<std::string, Sound> audioCache;

    GameTexture* wallTexture;
    GameTexture* floorTexture;
    GameTexture* skyTexture;
    GameTexture* bulletTexture;
    GameTexture* rocketTexture;
    GameTexture* inboundrocketTexture;
    GameTexture* knifeTexture;
    GameTexture* gunTexture;
    GameTexture* machinegunTexture;
    GameTexture* rocketlauncherTexture;
    GameTexture* skeletonTexture;
    GameTexture* ammoTexture;
    GameTexture* rocketammoTexture;
    GameTexture* bonesTexture;
    GameTexture* cloudTexture;

    void drawPixel(int x, int y, Color color);
    void clearScreen();
    GameTexture* loadTexture(const std::string& path, const std::string& name = "");
    bool loadAllTextures();
    bool loadAllSounds();
    GameTexture* getTexture(const std::string& name);
    void setWeaponTexture(int weaponId);
    void drawTexturedWall(int rayCount, int wallHeight, double rayX, double rayY);
    bool isVisibleToPlayer(const Monster& monster);
    void rayCastingFunc();
    void drawDebugSprites();
    void drawSpriteInWorld(const Sprite& sprite);
    void drawSpriteInWorld(const Monster & monster);
    void drawBulletSprite(const Projectile& projectile);
    void movePlayer();
    void updateGameObjects();
    void handleShooting();
    void setWeapon(int id);
    void itemPickup(int ycoords, int xcoords);
    void playSound(const std::string& id);
    void spawnWave();
    void drawPickupInWorld(const Sprite& sprite);
    
    
public:
    Game();
    ~Game();

    bool initialize();
    int menu();
    void loadLevel(int levelIdx);
    void run();
    void cleanup();
};

#endif
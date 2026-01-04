#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>

struct Level {
    std::string name;
    std::vector<std::vector<int>> map;
    bool unlocked;
    int floor;
    int wall;
    int background;
    struct StartLocation { double x, y; } startlocation;
    int equippedweapon;
    double monstermovespeed;

    Level() : unlocked(true), floor(5), wall(1), background(1),
        equippedweapon(1), monstermovespeed(0.02) {
        startlocation.x = 2;
        startlocation.y = 9;
    }
};

#endif
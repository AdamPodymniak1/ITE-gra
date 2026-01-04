#include "game.h"
#include <iostream>
#include "settings.h"

int main() {
    Game game;
    Settings settings;


    int menuState;
    bool keepRunning = true;
    if(settings.checkForFile()){
        settings.loadAll();
    if (game.initialize()) {
        
        while (!WindowShouldClose() && keepRunning)
        {
        menuState = game.menu();
        switch (menuState)
        {
        case 1: //Start game 
            game.run();
            break;
        case 2: // Map editor
            std::cout<<"Settings"<<std::endl;
            break;
        case 3: //Settings
            settings.open();
            break;
        case 4: // exit
            keepRunning = false;
            break;
        default:
            std::cout << "Menu Case switch Error" << std::endl;
        }
        }
        
        
            
    } 
    }else { std::cout << "Settings file cannot be found or created" << std::endl; }

    return 0;
}
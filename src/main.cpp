#include "GameManager.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {    
    try {
        GameManager game;
        game.initialize_game();
        game.start_game();
        
        std::this_thread::sleep_for(std::chrono::seconds(30));
        
        game.stop_game();
        game.print_survivors();
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
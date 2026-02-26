#include "Game.h"
#include <ctime>

int main() {
    std::srand((unsigned)std::time(nullptr));
    Game g; 
    g.run(); 
    return 0;
}
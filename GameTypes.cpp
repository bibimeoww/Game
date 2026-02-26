#include "GameTypes.h"

const sf::Color C_BG(15,10,25), C_WALL(60,50,90), C_FLOOR(30,25,50);
const sf::Color C_PC(100,200,255), C_ENE(220,80,80), C_BOSS(255,50,200);
const sf::Color C_ITEM(255,220,60), C_STAIR(100,255,150), C_UI(20,15,35,220);
const sf::Color C_HP(80,200,80), C_HPL(200,60,60), C_EXP(80,120,255);
const sf::Color C_TXT(220,210,255), C_GOLD(255,215,0), C_HI(255,200,50);

int rnd(int lo, int hi) { return lo + std::rand() % (hi - lo + 1); }
std::string ts(int v) { std::ostringstream o; o << v; return o.str(); }
#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>

// ── Constants ──────────────────────────────────────────────
const unsigned W = 1180, H = 768;
const int COLS = 18, ROWS = 13, TS = 48, OX = 32, OY = 80;

// ── Colors (Extern declaration) ────────────────────────────
extern const sf::Color C_BG, C_WALL, C_FLOOR, C_PC, C_ENE, C_BOSS;
extern const sf::Color C_ITEM, C_STAIR, C_UI, C_HP, C_HPL, C_EXP;
extern const sf::Color C_TXT, C_GOLD, C_HI;

// ── Enums ──────────────────────────────────────────────────
enum class GS { INTRO, NAME, AGE, CLASS, MAP, BATTLE, EVENT, SHOP, DEAD, WIN };
enum class Job { WAR, MAG, ROG, NONE };
enum class TT { FLOOR, WALL, STAIR, ITEM, ENE, BOSS, SHOP };
enum class BS { PTURN, RESULT };

// ── Structs ────────────────────────────────────────────────
struct Stats { int hp, mhp, atk, def, spd; };

struct Enemy {
    std::string name; Stats s;
    int exp, gold; bool boss; sf::Color col;
};

struct Item {
    std::string name; int hp, atk, def; std::string desc;
};

struct Tile {
    TT t = TT::WALL; bool vis = false; int ei = -1; Item item{};
};

struct Player {
    std::string name; int age = 0; Job job = Job::NONE;
    Stats s{}; int exp = 0, nexp = 100, lv = 1, gold = 0, score = 0, floor = 1;
    int x = 2, y = 2, scd = 0;
    std::vector<Item> inv;
};

// ── Helpers ────────────────────────────────────────────────
int rnd(int lo, int hi);
std::string ts(int v);

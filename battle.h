#ifndef BATTLE_H
#define BATTLE_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// ===== โครงสร้างผู้เล่น =====
struct Player{
    string name;
    int age;
    int hp;
    int atk;
    int def;
    int exp;
    int level;
    int score;
};

// ===== โครงสร้างบอส =====
struct Boss{
    string name;
    int hp;
    int atk;
    int def;
};

// ===== ฟังก์ชัน =====

// สร้างบอสตามด่าน (1-10)
Boss createBoss(int level);

// เริ่มการต่อสู้
void startBattle(Player &player, Boss boss);

// ระบบเลเวลอัพ
void levelUp(Player &player);

#endif

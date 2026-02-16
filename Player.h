#pragma once 
#include <iostream>
#include <string>

using namespace std;

class Player {
public:
    string name;
    int age;
    string jobClass;
    int hp;
    int maxHp;
    int attack;

    // Constructor
    Player(string n, int a) : name(n), age(a) {
        hp = 100;
        maxHp = 100;
        attack = 10;
        jobClass = "Novice";
    }

    // ฟังก์ชันเลือกอาชีพ
    void setJob(int choice) {
        if (choice == 1) {
            jobClass = "Warrior";
            maxHp = 150;
            hp = 150;
            attack = 25;
        } else if (choice == 2) {
            jobClass = "Mage";
            maxHp = 80;
            hp = 80;
            attack = 40;
        }
    }

    // ฟังก์ชันโชว์สถานะ
    void showStatus() {
        cout << "\n-----------------------------" << endl;
        cout << "STATUS: " << name << " (" << jobClass << ")" << endl;
        cout << "Age: " << age << endl;
        cout << "HP: " << hp << "/" << maxHp << endl;
        cout << "Attack: " << attack << endl;
        cout << "-----------------------------" << endl;
    }
};
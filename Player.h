#include <iostream>
#include <string>

using namespace std;

// สร้างพิมพ์เขียวสำหรับผู้เล่น
class Player {
public:
    string name;
    int age;
    string jobClass; // อาชีพ
    int hp;
    int maxHp;
    int attack;
    int exp;

    // Constructor (ฟังก์ชันสร้างตัวละคร)
    Player(string n, int a) {
        name = n;
        age = a;
        hp = 100;      // ค่าเริ่มต้น
        maxHp = 100;
        attack = 10;
        exp = 0;
    }

    // ฟังก์ชันแสดงสถานะ (เอาไว้เช็คค่า)
    void showStatus() {
        cout << "Name: " << name << " | Age: " << age << endl;
        cout << "HP: " << hp << "/" << maxHp << " | ATK: " << attack << endl;
        cout << "-------------------------" << endl;
    }
};

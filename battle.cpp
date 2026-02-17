#include "battle.h"

Boss createBoss(int level){
    Boss boss;

    if(level == 1){ boss = {"Slime King",40,8,2}; }
    else if(level == 2){ boss = {"Goblin Chief",60,10,3}; }
    else if(level == 3){ boss = {"Orc Warrior",80,12,4}; }
    else if(level == 4){ boss = {"Skeleton Knight",100,15,5}; }
    else if(level == 5){ boss = {"Dark Mage",120,18,6}; }
    else if(level == 6){ boss = {"Shadow Beast",150,20,7}; }
    else if(level == 7){ boss = {"Demon Guard",180,22,8}; }
    else if(level == 8){ boss = {"Warlock Lord",210,25,9}; }
    else if(level == 9){ boss = {"Ancient Knight",250,28,10}; }
    else{ boss = {"🔥 FINAL BOSS: Dragon Emperor 🔥",350,35,12}; }

    return boss;
}

// ===== ระบบเลเวลอัพ =====
void levelUp(Player &player){
    if(player.exp >= 50){
        player.level++;
        player.exp = 0;

        player.hp += 30;
        player.atk += 5;
        player.def += 2;

        cout << "\n✨ LEVEL UP! → Level " << player.level << endl;
        cout << "HP +30 | ATK +5 | DEF +2\n";
    }
}

// ===== ระบบต่อสู้ =====
void startBattle(Player &player, Boss boss){
    cout << "\n===== BATTLE START =====\n";
    cout << "Encounter: " << boss.name << endl;

    while(player.hp > 0 && boss.hp > 0){
        cout << "\nYour HP: " << player.hp
             << " | Boss HP: " << boss.hp << endl;

        cout << "1. Attack\n";
        cout << "Choose: ";
        int choice;
        cin >> choice;

        if(choice == 1){
            // ผู้เล่นโจมตี
            int damage = (player.atk - boss.def) + rand()%6;

            // critical hit สุ่ม
            if(rand()%5 == 0){
                damage *= 2;
                cout << "💥 Critical Hit! ";
            }

            if(damage < 1) damage = 1;

            boss.hp -= damage;
            cout << "You attack: -" << damage << " HP\n";
        }

        if(boss.hp <= 0) break;

        // บอสโจมตี
        int bossDamage = (boss.atk - player.def) + rand()%6;
        if(bossDamage < 1) bossDamage = 1;

        player.hp -= bossDamage;
        cout << boss.name << " attacks: -" << bossDamage << " HP\n";
    }

    // ===== ผลลัพธ์ =====
    if(player.hp > 0){
        cout << "\n🏆 You defeated " << boss.name << "!\n";
        player.exp += 20;
        player.score += 100;

        cout << "EXP +20 | Score +100\n";

        levelUp(player);
    }else{
        cout << "\n💀 You were defeated... Game Over\n";
    }
}

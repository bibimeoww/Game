#include "Game.h"
#include <cmath>

void Game::dt_(const std::string& s, unsigned sz, sf::Color c, float x, float y) {
    sf::Text t(fnt, s, sz); t.setFillColor(c); t.setPosition({x, y}); win.draw(t);
}

void Game::dtc(const std::string& s, unsigned sz, sf::Color c, float cx, float y) {
    sf::Text t(fnt, s, sz); t.setFillColor(c);
    t.setPosition({cx - t.getGlobalBounds().size.x / 2.f, y}); win.draw(t);
}

void Game::dr(float x, float y, float w, float h, sf::Color c, bool ol, sf::Color oc) {
    sf::RectangleShape r({w, h}); r.setPosition({x, y});
    if(ol) { r.setFillColor(sf::Color::Transparent); r.setOutlineThickness(2); r.setOutlineColor(oc); }
    else r.setFillColor(c);
    win.draw(r);
}

void Game::bar(float x, float y, float w, float h, float ratio, sf::Color fg, const std::string& lb) {
    ratio = std::max(0.f, std::min(1.f, ratio));
    dr(x, y, w, h, sf::Color(30,30,50));
    dr(x, y, w * ratio, h, fg);
    dr(x, y, w, h, sf::Color::Transparent, true, sf::Color(80,70,120));
    if(!lb.empty()) dt_(lb, 12, sf::Color::White, x + 3, y);
}

void Game::draw() {
    float sx = shk > 0 ? (float)rnd(-3,3) : 0.f;
    float sy = shk > 0 ? (float)rnd(-2,2) : 0.f;
    win.clear(C_BG);
    switch(gs) {
        case GS::INTRO: dIntro();  break;
        case GS::NAME:  dInput("Enter your character's name:", false); break;
        case GS::AGE:   dInput("Enter your character's age (numbers only):", true); break;
        case GS::CLASS: dClass();  break;
        case GS::MAP:   dMap(sx, sy); break;
        case GS::BATTLE:dBattle(sx, sy); break;
        case GS::EVENT: dEvent();  break;
        case GS::DEAD:  dEnd(false); break;
        case GS::WIN:   dEnd(true);  break;
    }
    
    win.display(); 
}

void Game::dIntro() {
    for(int i=0; i<50; i++) {
        float px = (float)((i*137 + (int)(at*20)) % W);
        float py = (float)((i*79 + (int)(at*15)) % H);
        dr(px, py, 2, 2, sf::Color(80,60,120,100));
    }

    float boxW = 664.0f;
    float boxH = 230.0f;
    float boxX = (W - boxW) / 2.0f;
    float boxY = 140.0f;

    dr(boxX, boxY, boxW, boxH, sf::Color(20,15,40,220));
    dr(boxX, boxY, boxW, boxH, sf::Color::Transparent, true, sf::Color(120,80,200));
    
    dtc("DUNGEON OF FATE", 48, sf::Color(180,140,255), W / 2.0f, 162);
    dtc("A Turn-Based Adventure", 22, sf::Color(140,120,200), W / 2.0f, 255);

    uint8_t a = (uint8_t)(150 + (int)(80 * std::sin(at * 3)));
    dtc("Press ENTER or SPACE to begin", 20, sf::Color(200,180,255,a), W / 2.0f, 420);
    dt_("WASD/Arrows=Move  ENTER=Confirm  Arrows=Select  N=Next Floor", 13, sf::Color(100,90,150), 180, 560);
}

void Game::dInput(const std::string& prompt, bool isAge) {

    float boxW = 720.0f;
    float boxH = 230.0f;
    float boxX = (W - boxW) / 2.0f;
    float boxY = 250.0f;
    float textX = boxX + 20.0f; //ระยะขอบตัวหนังสือในกล่อง

    dr(boxX, boxY, boxW, boxH, C_UI);
    dr(boxX, boxY, boxW, boxH, sf::Color::Transparent, true, sf::Color(120,80,200));
    
    dt_(prompt, 22, sf::Color(200,180,255), textX, boxY + 25.0f);
    
    std::string d = inp + (cur < 1.f ? "_" : " ");
    dt_(d, 34, C_GOLD, textX, boxY + 72.0f);

    dt_("Press ENTER to confirm", 16, sf::Color(120,110,160), textX, boxY + 170.0f);

    if(isAge && !pl.name.empty()){
        dt_("Name: " + pl.name, 18, C_TXT, textX, boxY + 205.0f);
    } 
}

void Game::dClass() {
    dtc("Choose Your Class", 36, sf::Color(200,180,255), W / 2.0f, 25);
    dtc("Hero: " + pl.name + "  Age: " + ts(pl.age), 18, C_TXT, W / 2.0f, 78);

    Job cls[] = {Job::WAR, Job::MAG, Job::ROG};
    sf::Color cols[] = {sf::Color(220,100,80), sf::Color(80,120,255), sf::Color(120,220,100)};
    
    const char* desc[] = {"High HP+DEF\nSkill: Power Strike\n(2x ATK, 1-turn CD)",
                          "High ATK\nSkill: Fireball\n(3x, ignore DEF, 2-turn CD)",
                          "High SPD\nSkill: Backstab\n(2.5x true dmg, 1-turn CD)"};
    Stats st[] = {{120,120,18,12,8}, {80,80,28,6,10}, {100,100,20,8,14}};
    
    float boxWidth = 250.0f;
    float gap = 40.0f; 
    float totalWidth = (boxWidth * 3) + (gap * 2);
    float startX = (W - totalWidth) / 2.0f; 

    // 1. จัดกลุ่ม Sprite ไว้ใน Array เพื่อดึงไปวาดตามรอบ
    sf::Sprite* sprClass[] = {&sprWar, &sprMag, &sprRog};

    for(int i=0; i<3; i++) {
        float cx = startX + (float)(i * (boxWidth + gap)); 
        float cy = 130.0f;
        
        bool sel = (selClass == i);
        sf::Color c = sel ? cols[i] : sf::Color(cols[i].r, cols[i].g, cols[i].b, 140);
        dr(cx, cy, boxWidth, 390, sel ? sf::Color(40,35,70) : sf::Color(20,15,40));
        dr(cx, cy, boxWidth, 390, sf::Color::Transparent, true, c);
        if(sel) dr(cx, cy, boxWidth, 4, c);
        
        // --- 2. ส่วนที่ใช้วาดรูปตัวละครแทนตัวหนังสือ [W] [M] [R] ---
        sf::Sprite* spr = sprClass[i];
        
        // คำนวณจุดกึ่งกลาง (รูปเราขนาด 80x80 ครึ่งนึงคือ 40)
        float sprX = cx + (boxWidth / 2.0f) - 40.0f; 
        float sprY = cy + 20.0f; 
        
        // ใส่ลูกเล่นให้รูปที่โดนเลือก
        if(sel) {
            spr->setColor(sf::Color::White); // สว่าง 100%
            sprY += std::sin(at * 5) * 5.0f; // เด้งขึ้นลงดึ๋งๆ
        } else {
            spr->setColor(sf::Color(255, 255, 255, 100)); // รูปที่ไม่ได้เลือกให้สีจางลง
        }
        
        spr->setPosition({sprX, sprY});
        win.draw(*spr);
        // ---------------------------------------------------
        
        // 3. วาดข้อความต่างๆ (ผมขยับพิกัด Y ลงมาเพื่อหลบรูปภาพให้แล้ว)
        dtc(jname(cls[i]), 22, c, cx + (boxWidth / 2.0f), cy + 120);
        dt_(desc[i], 11, sf::Color(180,170,200), cx + 10, cy + 155);
        
        // วาดหลอดพลัง (ขยับลงมานิดนึงให้สวยงาม)
        float ys = cy + 240; 
        dt_("HP:", 12, C_TXT, cx+8, ys); bar(cx+36, ys+2, 200, 11, (float)st[i].mhp/140.f, C_HP); ys+=20;
        dt_("ATK:",12, C_TXT, cx+8, ys); bar(cx+36, ys+2, 200, 11, (float)st[i].atk/30.f, C_ENE); ys+=20;
        dt_("DEF:",12, C_TXT, cx+8, ys); bar(cx+36, ys+2, 200, 11, (float)st[i].def/15.f, sf::Color(80,140,220)); ys+=20;
        dt_("SPD:",12, C_TXT, cx+8, ys); bar(cx+36, ys+2, 200, 11, (float)st[i].spd/15.f, C_STAIR);
    }
    
    dtc("LEFT/RIGHT select  |  ENTER confirm", 18, sf::Color(150,130,200), W / 2.0f, 548);
}

void Game::dMap(float sx, float sy) {
    // 1. วาดแผนที่ (Floor, Wall, Item, Enemy, Boss, Stair)
    for(int r=0; r<ROWS; r++) {
        for(int c=0; c<COLS; c++) {
            float tx = OX + sx + c * TS;
            float ty = OY + sy + r * TS;
            Tile& t = map[r][c];
            
            // ถ้าเป็นพื้นที่ที่ยังไม่เคยเดินไปเปิด (หมอกดำ)
            if(!t.vis) { dr(tx, ty, TS-1, TS-1, sf::Color(5,5,10)); continue; }

            // --- 1. วาดพื้นห้อง ---
            dr(tx, ty, TS-1, TS-1, sf::Color(30, 25, 50)); 

            // --- 2. วาดสิ่งต่างๆ ทับบนพื้น ---
            if(t.t == TT::WALL) {
                // กำแพง
                dr(tx, ty, TS-1, TS-1, sf::Color(65, 55, 100)); 
            }
            else if(t.t == TT::STAIR) {
                // บันได (กล่องสีเขียว)
                dr(tx + 8.f, ty + 8.f, TS - 16.f, TS - 16.f, C_STAIR);
            }
            else if(t.t == TT::ITEM) {
                // ไอเทม (กล่องสีทอง ลอยขึ้นลงได้)
                float bob = std::sin(at*3 + r)*3.f; 
                dr(tx + 12.f, ty + 12.f + bob, TS - 24.f, TS - 24.f, C_GOLD);
            }
            else if(t.t == TT::ENE) {
                // ศัตรู 
                float bob = std::sin(at*4 + c)*2.f; 
                sprEne.setPosition({tx + 4.f, ty + 4.f + bob});
                win.draw(sprEne);
            }
            else if(t.t == TT::BOSS) {
                // บอส 
                float bob = std::sin(at*4)*2.f;
                sprBoss.setPosition({tx + 4.f, ty + 4.f + bob}); 
                win.draw(sprBoss);
            }
        }
    } 

    // 2. วาดตัวละครผู้เล่น 
    float pp = OX + sx + pl.x * TS;
    float py2 = OY + sy + pl.y * TS;
    float bob = std::sin(at*4)*2.f;
    sprPlayer.setPosition({pp + 4.f, py2 + 4.f + bob});
    win.draw(sprPlayer);

    // 3. วาดแผงเมนูด้านขวามือ
    float ux = OX + sx + (COLS * TS) + 20.0f;
    float uy = OY + sy;
    float y = uy + 6.0f;
    
    dr(ux, uy, 210, (float)(ROWS * TS), C_UI);
    dr(ux, uy, 210, (float)(ROWS * TS), sf::Color::Transparent, true, sf::Color(80,60,120));
    
    dt_(pl.name, 18, C_GOLD, ux+10, y); y+=26;
    dt_(jname(pl.job) + " Age:" + ts(pl.age), 13, C_TXT, ux+10, y); y+=22;
    dt_("HP:", 12, C_TXT, ux+10, y); bar(ux+34, y+2, 166, 11, (float)pl.s.hp/pl.s.mhp, C_HP);
    dt_(ts(pl.s.hp) + "/" + ts(pl.s.mhp), 10, sf::Color::White, ux+100, y+1); y+=18;
    
    dt_("EXP:", 12, C_TXT, ux+10, y); bar(ux+34, y+2, 166, 11, (float)pl.exp/100.f, sf::Color(100,180,255));
    dt_(ts(pl.exp) + "/100", 10, sf::Color::White, ux+100, y+1); y+=24;
    
    dt_("Lv." + ts(pl.lv), 24, C_PC, ux+10, y); y+=30;
    dt_("ATK:" + ts(pl.s.atk) + " DEF:" + ts(pl.s.def) + " SPD:" + ts(pl.s.spd), 11, C_TXT, ux+10, y); y+=16;
    dt_("Gold: " + ts(pl.gold), 12, C_GOLD, ux+10, y); y+=20;
    
    int ec = enemyCount();
    sf::Color ecC = (ec == 0) ? C_STAIR : C_ENE;
    dt_("Enemies: " + ts(ec), 12, ecC, ux+10, y); y+=16;
    if(ec == 0) { dt_("-> Stairs Open!", 12, C_STAIR, ux+10, y); y+=16; }
    
    dt_("Items: " + ts(pl.inv.size()), 12, C_GOLD, ux+10, y); y+=16;
    for(auto& p : pl.inv) { dt_("- " + p.name, 11, sf::Color(200,200,200), ux+10, y); y+=14; }
    
    dt_("LEGEND:", 11, sf::Color(150,150,150), ux+10, y); y+=16;
    dr(ux+10, y+2, 8, 8, C_ENE); dt_("Enemy", 11, C_TXT, ux+22, y); y+=14;
    dr(ux+10, y+2, 8, 8, C_BOSS); dt_("Boss", 11, C_TXT, ux+22, y); y+=14;
    dr(ux+10, y+2, 8, 8, C_GOLD); dt_("Item", 11, C_TXT, ux+22, y); y+=14;
    dr(ux+10, y+2, 8, 8, C_STAIR); dt_("Stairs", 11, C_TXT, ux+22, y); y+=14;
    
    // 4. ข้อความ Log ด้านล่างซ้าย
    float cy = OY + sy + (ROWS * TS) + 10.f;
    int ec2 = std::min((int)log.size(), 4);
    std::string cl = "";
    for(int i=0; i<ec2; i++) cl += log[log.size()-ec2+i] + "\n";
    dt_(cl, 14, C_GOLD, OX + 10.f, cy);
}

void Game::dBattle(float sx, float sy) {
    dr(0, 0, (float)W, 65, sf::Color(12,8,25));
    dt_("BATTLE!", 30, batt.boss ? C_BOSS : C_ENE, 20, 15);
    //Enemy
    if(batt.boss){
        sprBossBatt.setPosition({540.f + sx, 85.f + sy});
        win.draw(sprBossBatt);
    }else{
        sprEneBatt.setPosition({540.f + sx, 85.f + sy});
        win.draw(sprEneBatt);
    }
    //player
    sprPlBatt.setPosition({110.f + sx, 130.f + sy});
    win.draw(sprPlBatt);


    dr(20, 480, (float)(W - 40), 108, C_UI);
    if(bs == BS::PTURN) {
        dt_("[1] Attack  [2] Skill  [3] Item  [4] Run (Use arrows to select)", 18, C_HI, 40, 500);
        dt_("Selected Action: " + ts(selAct + 1), 18, C_GOLD, 40, 530);
    } else {
        dt_(res, 22, batt.s.hp <= 0 ? C_ITEM : C_HPL, 40, 500);
    }
}

void Game::dEvent() {
    dr(152, 225, 720, 290, C_UI);
    dtc("! EVENT !", 32, C_ITEM, 512, 245);
    dt_(evtMsg, 18, C_TXT, 175, 308);
}

void Game::dEnd(bool isWin) {
    isWin ? win.clear(sf::Color(20,50,20)) : win.clear(sf::Color(50,20,20));
    dtc(isWin ? "VICTORY!" : "GAME OVER", 56, isWin ? C_GOLD : C_HPL, 512, 120);
}
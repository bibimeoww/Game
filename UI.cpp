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
    dr(0, 0, (float)W, 70, sf::Color(15,10,35));
    dt_("DUNGEON OF FATE", 22, sf::Color(150,120,220), 20, 10);
    dt_("Floor:" + ts(pl.floor) + "/9  " + pl.name + " Lv." + ts(pl.lv) + "  Score:" + ts(pl.score), 18, C_TXT, 20, 38);

    // 1. วาดแผนที่
    for(int r=0; r<ROWS; r++) for(int c=0; c<COLS; c++) {
        float tx = OX + sx + c * TS, ty = OY + sy + r * TS;
        Tile& t = map[r][c];
        if(!t.vis) { dr(tx, ty, TS-1, TS-1, sf::Color(5,5,10)); continue; }
        sf::Color bg = t.t == TT::WALL ? C_WALL : (t.t == TT::STAIR ? sf::Color(20,60,30) : C_FLOOR);
        dr(tx, ty, TS-1, TS-1, bg);
        if(t.t == TT::WALL) dr(tx+4, ty+4, TS-9, TS-9, sf::Color(45,38,72,160));
        else if(t.t == TT::STAIR) { for(int i=0; i<3; i++) dr(tx+8+i*10, ty+28-i*7, 8, 7, C_STAIR); dt_("v", 14, C_STAIR, tx+16, ty+4); }
        else if(t.t == TT::ITEM) { dr(tx+14, ty+14, 20, 20, C_ITEM); dt_("*", 20, C_ITEM, tx+16, ty+12); }
        else if(t.t == TT::ENE) { float p = 0.85f+0.15f*std::sin(at*3); int s = (int)(30*p); dr(tx+(TS-s)/2, ty+(TS-s)/2, (float)s, (float)s, C_ENE); dt_("E", 18, sf::Color::White, tx+15, ty+12); }
        else if(t.t == TT::BOSS) { float p = 0.8f+0.2f*std::sin(at*4); int s = (int)(38*p); dr(tx+(TS-s)/2, ty+(TS-s)/2, (float)s, (float)s, C_BOSS); dt_("B", 20, sf::Color::White, tx+14, ty+10); }
    }

    // 2. วาดตัวละคร
    float pp = OX + sx + pl.x * TS, py2 = OY + sy + pl.y * TS;
    float bob = std::sin(at*4)*2.f;

    sprPlayer.setPosition({pp + 8, py2 + 8 + bob});
    win.draw(sprPlayer);

    // 3. Side Panel (เมนูฝั่งขวา)
    float ux = OX + COLS * TS + 16.f, uy = (float)OY;
    dr(ux, uy, 210, (float)(ROWS * TS), C_UI);
    dr(ux, uy, 210, (float)(ROWS * TS), sf::Color::Transparent, true, sf::Color(80,60,120));
    
    float y = uy + 10;
    dt_(pl.name, 18, C_GOLD, ux+10, y); y+=26;
    dt_(jname(pl.job) + " Age:" + ts(pl.age), 13, C_TXT, ux+10, y); y+=22;
    
    sf::Color hc = pl.s.hp > pl.s.mhp/3 ? C_HP : C_HPL;
    dt_("HP:", 13, C_TXT, ux+10, y);
    bar(ux+34, y+2, 162, 13, (float)pl.s.hp/pl.s.mhp, hc, ts(pl.s.hp) + "/" + ts(pl.s.mhp)); y+=22;
    
    dt_("EXP:", 13, C_TXT, ux+10, y);
    bar(ux+38, y+2, 158, 13, (float)pl.exp/pl.nexp, C_EXP, ts(pl.exp) + "/" + ts(pl.nexp)); y+=22;
    
    dt_("Lv." + ts(pl.lv), 18, sf::Color(200,180,255), ux+10, y); y+=24;
    dt_("ATK:" + ts(pl.s.atk) + " DEF:" + ts(pl.s.def) + " SPD:" + ts(pl.s.spd), 12, C_TXT, ux+10, y); y+=18;
    dt_("Gold: " + ts(pl.gold), 13, C_GOLD, ux+10, y); y+=20;

    int ec = enemyCount();
    sf::Color ecc = ec == 0 ? C_STAIR : C_ENE;
    dt_("Enemies: " + ts(ec), 13, ecc, ux+10, y); y+=16;
    if(ec == 0) { dt_("[N] Next Floor!", 13, C_GOLD, ux+10, y); y+=16; }

    dt_("Items:" + ts(pl.inv.size()), 12, C_TXT, ux+10, y); y+=14;
    for(auto& item : pl.inv) { dt_("- " + item.name, 11, C_ITEM, ux+10, y); y+=13; }
    y+=6;
    
    dt_("LEGEND:", 12, sf::Color(100,90,140), ux+10, y); y+=16;
    dr(ux+10, y+2, 10, 10, C_ENE);   dt_("Enemy",  11, C_TXT, ux+24, y); y+=14;
    dr(ux+10, y+2, 10, 10, C_BOSS);  dt_("Boss",   11, C_TXT, ux+24, y); y+=14;
    dr(ux+10, y+2, 10, 10, C_ITEM);  dt_("Item",   11, C_TXT, ux+24, y); y+=14;
    dr(ux+10, y+2, 10, 10, C_STAIR); dt_("Stairs", 11, C_TXT, ux+24, y);

    // 4. Bottom bar & Log messages
    float cy2 = (float)(OY + ROWS * TS + 5);
    int ec2 = enemyCount();
    std::string hint = ec2 == 0 ? 
        "ALL ENEMIES CLEARED! >>> Press [N] to go to next floor <<< " : 
        "WASD/Arrows:Move | Step on [v] to descend | Enemies left: " + ts(ec2);
    sf::Color hcol = ec2 == 0 ? C_GOLD : sf::Color(100,90,140);
    dt_(hint, 13, hcol, (float)OX, cy2);

    int ls = (int)log.size(); int st = std::max(0, ls-3);
    for(int i = st; i < ls; i++) {
        sf::Color lc = (i == ls-1) ? C_TXT : sf::Color(100,90,130);
        dt_(log[i], 12, lc, (float)OX, cy2 + 16 + (i-st) * 15.f);
    }
}

void Game::dBattle(float sx, float sy) {
    dr(0, 0, (float)W, 65, sf::Color(12,8,25));
    dt_("BATTLE!", 30, batt.boss ? C_BOSS : C_ENE, 20, 15);
    dr(540 + sx, 85 + sy, 100, 100, batt.col);
    dr(110 + sx, 130 + sy, 80, 80, C_PC);
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
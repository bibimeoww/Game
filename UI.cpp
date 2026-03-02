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
    dr(180, 140, 664, 230, sf::Color(20,15,40,220));
    dr(180, 140, 664, 230, sf::Color::Transparent, true, sf::Color(120,80,200));
    dtc("DUNGEON OF FATE", 48, sf::Color(180,140,255), 512, 162);
    dtc("A Turn-Based Adventure", 22, sf::Color(140,120,200), 512, 255);
    uint8_t a = (uint8_t)(150 + (int)(80 * std::sin(at * 3)));
    dtc("Press ENTER or SPACE to begin", 20, sf::Color(200,180,255,a), 512, 420);
    dt_("WASD/Arrows=Move  ENTER=Confirm  Arrows=Select  N=Next Floor", 13, sf::Color(100,90,150), 180, 560);
}

void Game::dInput(const std::string& prompt, bool isAge) {
    dr(152, 250, 720, 230, C_UI);
    dr(152, 250, 720, 230, sf::Color::Transparent, true, sf::Color(120,80,200));
    dt_(prompt, 22, sf::Color(200,180,255), 172, 275);
    std::string d = inp + (cur < 1.f ? "_" : " ");
    dt_(d, 34, C_GOLD, 172, 322);
    dt_("Press ENTER to confirm", 16, sf::Color(120,110,160), 172, 420);
    if(isAge && !pl.name.empty()) dt_("Name: " + pl.name, 18, C_TXT, 172, 455);
}

void Game::dClass() {
    dtc("Choose Your Class", 36, sf::Color(200,180,255), 512, 25);
    Job cls[] = {Job::WAR, Job::MAG, Job::ROG};
    sf::Color cols[] = {sf::Color(220,100,80), sf::Color(80,120,255), sf::Color(120,220,100)};
    const char* icons[] = {"[W]", "[M]", "[R]"};
    const char* desc[] = {"High HP+DEF\nSkill: Power Strike", "High ATK\nSkill: Fireball", "High SPD\nSkill: Backstab"};
    Stats st[] = {{120,120,18,12,8}, {80,80,28,6,10}, {100,100,20,8,14}};
    for(int i=0; i<3; i++) {
        float cx = 110 + (float)(i * 270), cy = 130;
        bool sel = (selClass == i);
        sf::Color c = sel ? cols[i] : sf::Color(cols[i].r, cols[i].g, cols[i].b, 140);
        dr(cx, cy, 250, 390, sel ? sf::Color(40,35,70) : sf::Color(20,15,40));
        dr(cx, cy, 250, 390, sf::Color::Transparent, true, c);
        dtc(icons[i], 48, c, cx + 125, cy + 10);
        dtc(jname(cls[i]), 22, c, cx + 125, cy + 78);
        dt_(desc[i], 12, sf::Color(180,170,200), cx + 10, cy + 115);
    }
}

void Game::dMap(float sx, float sy) {
    dr(0, 0, (float)W, 70, sf::Color(15,10,35));
    dt_("Floor:" + ts(pl.floor) + "  " + pl.name + " Lv." + ts(pl.lv), 18, C_TXT, 20, 38);
    for(int r=0; r<ROWS; r++) for(int c=0; c<COLS; c++) {
        float tx = OX + sx + c * TS, ty = OY + sy + r * TS;
        Tile& t = map[r][c];
        if(!t.vis) { dr(tx, ty, TS-1, TS-1, sf::Color(5,5,10)); continue; }
        sf::Color bg = t.t == TT::WALL ? C_WALL : (t.t == TT::STAIR ? sf::Color(20,60,30) : C_FLOOR);
        dr(tx, ty, TS-1, TS-1, bg);
        if(t.t == TT::STAIR) dt_("v", 14, C_STAIR, tx + 16, ty + 4);
        else if(t.t == TT::ITEM) dr(tx + 14, ty + 14, 20, 20, C_ITEM);
        else if(t.t == TT::ENE) dr(tx + 10, ty + 10, 20, 20, C_ENE);
        else if(t.t == TT::BOSS) dr(tx + 5, ty + 5, 30, 30, C_BOSS);
    }
    float pp = OX + sx + pl.x * TS, py2 = OY + sy + pl.y * TS;
    dr(pp + 8, py2 + 8, 30, 30, C_PC);
    
    // Side panel & log UI (simplified for brevity, matching previous style)
    float ux = OX + COLS * TS + 16.f, uy = (float)OY;
    dr(ux, uy, 210, (float)(ROWS * TS), C_UI);
    dr(ux, uy, 210, (float)(ROWS * TS), sf::Color::Transparent, true, sf::Color(80,60,120));
    dt_("HP: " + ts(pl.s.hp) + "/" + ts(pl.s.mhp), 16, C_HP, ux + 10, uy + 50);
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
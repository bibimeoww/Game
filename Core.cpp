#include "Game.h"
#include <iostream>

Game::Game() : win(sf::VideoMode({W,H}), "DUNGEON OF FATE", sf::Style::Titlebar|sf::Style::Close),
               gs(GS::INTRO), selClass(0), selAct(0), cur(0), at(0), shk(0), mt(0), it(0) {
    win.setFramerateLimit(60);
    loadFont();
}

void Game::run() {
    sf::Clock clk;
    while(win.isOpen()) {
        float dt = clk.restart().asSeconds();
        events(); update(dt); draw();
    }
}

void Game::loadFont() {
    const char* p[] = {
        "/opt/homebrew/share/fonts/dejavu-fonts/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "arial.ttf", nullptr
    };
    for(int i=0; p[i]; i++) if(fnt.openFromFile(p[i])) return;
    std::cerr << "Font not found!\n";
}

void Game::addLog(const std::string& s) {
    log.push_back(s);
    if((int)log.size() > 8) log.erase(log.begin());
}

void Game::events() {
    while(auto eo = win.pollEvent()) {
        auto& ev = *eo;
        if(ev.is<sf::Event::Closed>()) { win.close(); return; }
        if(auto* kp = ev.getIf<sf::Event::KeyPressed>()) handleKey(kp->code);
        if(auto* te = ev.getIf<sf::Event::TextEntered>()) {
            if(gs == GS::NAME || gs == GS::AGE) {
                char c = (char)te->unicode;
                if(c >= 32 && c < 127 && inp.size() < 16) {
                    if(gs == GS::AGE && (c < '0' || c > '9')) continue;
                    inp += c;
                }
            }
        }
    }
}

void Game::handleKey(sf::Keyboard::Key k) {
    using K = sf::Keyboard::Key;
    switch(gs) {
        case GS::INTRO: if(k == K::Enter || k == K::Space) gs = GS::NAME; break;
        case GS::NAME:
            if(k == K::Enter && !inp.empty()) { pl.name = inp; inp = ""; gs = GS::AGE; }
            else if(k == K::Backspace && !inp.empty()) inp.pop_back();
            break;
        case GS::AGE:
            if(k == K::Enter && !inp.empty()) { pl.age = std::stoi(inp); inp = ""; gs = GS::CLASS; }
            else if(k == K::Backspace && !inp.empty()) inp.pop_back();
            break;
        case GS::CLASS:
            if(k == K::Left) selClass = (selClass + 2) % 3;
            if(k == K::Right) selClass = (selClass + 1) % 3;
            if(k == K::Enter) { pl.job = (Job)selClass; initPlayer(); genMap(); gs = GS::MAP; }
            break;
        case GS::MAP:
            if(k == K::W || k == K::Up) move(0,-1);
            if(k == K::S || k == K::Down) move(0, 1);
            if(k == K::A || k == K::Left) move(-1,0);
            if(k == K::D || k == K::Right) move(1, 0);
            if(k == K::N) {
                if(enemyCount() == 0) descend();
                else addLog("Still enemies left on this floor!");
            }
            if(pl.s.hp <= 0) gs = GS::DEAD;
            break;
        case GS::BATTLE:
            if(bs == BS::PTURN) {
                if(k == K::Left) selAct = (selAct + 3) % 4;
                if(k == K::Right) selAct = (selAct + 1) % 4;
                if(k == K::Up || k == K::Down) selAct = (selAct + 2) % 4;
                if(k == K::Enter) doPlayerAction(selAct);
            } else {
                if(k == K::Enter) gs = (pl.s.hp <= 0) ? GS::DEAD : GS::MAP;
            }
            break;
        case GS::EVENT:
            if(k == K::Enter || k == K::Space) gs = (pl.s.hp <= 0) ? GS::DEAD : GS::MAP;
            break;
        case GS::DEAD:
        case GS::WIN:
            if(k == K::Enter) { gs = GS::INTRO; it = 0; log.clear(); }
            break;
    }
}

void Game::update(float dt) {
    cur += dt * 2.f; if(cur > 2.f) cur = 0;
    at += dt;
    if(shk > 0) shk -= dt;
}

void Game::move(int dx, int dy) {
    int nx = pl.x + dx, ny = pl.y + dy;
    if(nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return;
    Tile& t = map[ny][nx];
    if(t.t == TT::WALL) return;

    if(t.t == TT::ENE || t.t == TT::BOSS) {
        if(t.ei >= 0 && t.ei < (int)elist.size()) {
            Enemy e = elist[t.ei];
            t.t = TT::FLOOR; t.ei = -1;
            startBattle(e);
        }
        return;
    }

    pl.x = nx; pl.y = ny;

    if(t.t == TT::ITEM) {
        if(t.item.atk > 0) pl.s.atk += t.item.atk;
        if(t.item.def > 0) pl.s.def += t.item.def;
        if(t.item.hp > 0) pl.inv.push_back(t.item);
        evtMsg = "Found: " + t.item.name + "! " + t.item.desc;
        t.t = TT::FLOOR; t.item = {};
        updateVis(); gs = GS::EVENT; return;
    }
    if(t.t == TT::STAIR) { descend(); return; }

    if(rnd(1, 100) <= 5) trigEvt();
    updateVis();
}
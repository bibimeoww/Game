#include "Game.h"
#include <algorithm>

void Game::genMap() {
    map.assign(ROWS, std::vector<Tile>(COLS));
    const int GC = 3, GR = 3; 
    struct Room { int r1, c1, r2, c2; int cr() const { return (r1+r2)/2; } int cc() const { return (c1+c2)/2; } };
    std::vector<Room> rooms;
    int cellW = COLS / GC, cellH = ROWS / GR;

    for(int gr = 0; gr < GR; gr++) for(int gc = 0; gc < GC; gc++) {
        int cr1 = gr * cellH + 1, cc1 = gc * cellW + 1;
        int cr2 = (gr + 1) * cellH - 1, cc2 = (gc + 1) * cellW - 1;
        cr2 = std::min(cr2, ROWS - 2); cc2 = std::min(cc2, COLS - 2);
        int rh = rnd(2, cr2 - cr1), rw = rnd(3, cc2 - cc1);
        int r1 = cr1 + rnd(0, std::max(0, cr2 - cr1 - rh));
        int c1 = cc1 + rnd(0, std::max(0, cc2 - cc1 - rw));
        int r2 = std::min(r1 + rh, cr2);
        int c2 = std::min(c1 + rw, cc2);
        rooms.push_back({r1, c1, r2, c2});
        for(int r = r1; r <= r2; r++) for(int c = c1; c <= c2; c++) map[r][c].t = TT::FLOOR;
    }

    // --- แก้บัคด้วยระบบสร้างทางเดิน L-Shape ---
    auto carveCorridor = [&](int r1, int c1, int r2, int c2) {
        int sc = std::min(c1, c2), ec = std::max(c1, c2);
        for (int c = sc; c <= ec; c++) map[r1][c].t = TT::FLOOR;
        int sr = std::min(r1, r2), er = std::max(r1, r2);
        for (int r = sr; r <= er; r++) map[r][c2].t = TT::FLOOR;
    };

    for(int gr=0; gr<GR; gr++) for(int gc=0; gc<GC-1; gc++) {
        carveCorridor(rooms[gr*GC+gc].cr(), rooms[gr*GC+gc].cc(), rooms[gr*GC+gc+1].cr(), rooms[gr*GC+gc+1].cc());
    }
    for(int gc=0; gc<GC; gc++) for(int gr=0; gr<GR-1; gr++) {
        carveCorridor(rooms[gr*GC+gc].cr(), rooms[gr*GC+gc].cc(), rooms[(gr+1)*GC+gc].cr(), rooms[(gr+1)*GC+gc].cc());
    }
    // ----------------------------------------

    pl.x = rooms[0].cc(); pl.y = rooms[0].cr();
    map[pl.y][pl.x].t = TT::FLOOR;

    int sx = rooms[8].cc(), sy = rooms[8].cr();
    if(sx == pl.x && sy == pl.y) { sx = rooms[8].c2; sy = rooms[8].r2; }
    map[sy][sx].t = TT::STAIR;

    std::vector<std::pair<int,int>> floors;
    for(int r=0; r<ROWS; r++) for(int c=0; c<COLS; c++) {
        if(map[r][c].t == TT::FLOOR && !(r==pl.y && c==pl.x)) floors.push_back({r,c});
    }

    elist.clear();
    int fi = 0, ne = std::min(4 + pl.floor * 2, (int)floors.size() / 2);
    for(int i = 0; i < ne && fi < (int)floors.size(); i++, fi++) {
        elist.push_back(mkEnemy(false));
        map[floors[fi].first][floors[fi].second].t = TT::ENE;
        map[floors[fi].first][floors[fi].second].ei = (int)elist.size() - 1;
    }
    if((pl.floor % 3 == 0 || pl.floor >= 9) && fi < (int)floors.size()) {
        elist.push_back(mkEnemy(true));
        map[floors[fi].first][floors[fi].second].t = TT::BOSS;
        map[floors[fi].first][floors[fi].second].ei = (int)elist.size() - 1;
    }

    revealAll();
    updateVis();
}

void Game::revealAll() {
    for(int r=0; r<ROWS; r++) for(int c=0; c<COLS; c++) map[r][c].vis = true;
}

void Game::updateVis() {
    for(int r = std::max(0, pl.y - 5); r <= std::min(ROWS - 1, pl.y + 5); r++)
        for(int c = std::max(0, pl.x - 5); c <= std::min(COLS - 1, pl.x + 5); c++)
            map[r][c].vis = true;
}

int Game::enemyCount() {
    int n = 0;
    for(auto& row : map) for(auto& t : row) if(t.t == TT::ENE || t.t == TT::BOSS) n++;
    return n;
}

void Game::descend() {
    pl.floor++; pl.score += 200;
    if(pl.floor > 9) { gs = GS::WIN; return; }
    pl.s.hp = std::min(pl.s.mhp, pl.s.hp + pl.s.mhp / 4);
    genMap();
    addLog(">>> Floor " + ts(pl.floor) + "/9 <<<");
}

Enemy Game::mkEnemy(bool boss) {
    Enemy e; int f = pl.floor;
    if(boss) {
        e.name = "Boss"; e.s = {100+f*40, 100+f*40, 20+f*6, 10+f*3, 6+f*2};
        e.exp = 150+f*80; e.gold = 80+f*40; e.boss = true; e.col = C_BOSS;
    } else {
        e.name = "Monster"; e.s = {20+f*12, 20+f*12, 8+f*3, 4+f*2, 4+f};
        e.exp = 20+f*10; e.gold = 5+f*3; e.boss = false; e.col = C_ENE;
    }
    return e;
}

Item Game::mkItem() {
    return {"Potion", 40, 0, 0, "Restores HP"};
}

void Game::trigEvt() {
    int h = rnd(10, 25); pl.s.hp = std::min(pl.s.mhp, pl.s.hp + h);
    evtMsg = "Glowing rune! +" + ts(h) + " HP.";
    gs = GS::EVENT;
}

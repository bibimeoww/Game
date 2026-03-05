#include "Game.h"
#include <iostream>

Game::Game() : win(sf::VideoMode({W,H}), "DUNGEON OF FATE", sf::Style::Titlebar|sf::Style::Close),
               sprPlayer(texPlayer), sprWall(texWall), sprFloor(texFloor), 
               sprEne(texEne), sprBoss(texBoss), sprItem(texItem), sprStair(texStair), 
               sprWar(texWar), sprMag(texMag), sprRog(texRog),
               sprPlBatt(texPlBatt), sprEneBatt(texEneBatt), sprBossBatt(texBossBatt),
               gs(GS::INTRO), selClass(0), selAct(0), cur(0), at(0), shk(0), mt(0), it(0) {
    
    win.setFramerateLimit(60);
    loadFont();

    // ฟังก์ชันตัวช่วยโหลดรูปและปรับขนาด
    auto setupSprite = [](sf::Texture& tex, sf::Sprite& spr, const std::string& file, float targetSize) {
        if(tex.loadFromFile(file)) {
            tex.setSmooth(false);
            spr.setTexture(tex, true);
            spr.setScale({targetSize / tex.getSize().x, targetSize / tex.getSize().y});
        } else {
            printf("WARNING: Image %s not found!\n", file.c_str());
        }
    };

    // โหลดรูปแผนที่และตัวละคร
    setupSprite(texPlayer, sprPlayer, "warrior.png", 40.0f); 
    setupSprite(texWall, sprWall, "wall.png", 48.0f);
    setupSprite(texFloor, sprFloor, "floor.png", 48.0f);
    setupSprite(texEne, sprEne, "enemy.png", 40.0f);
    setupSprite(texBoss, sprBoss, "boss.png", 48.0f);
    setupSprite(texItem, sprItem, "item.png", 32.0f);
    setupSprite(texStair, sprStair, "stair.png", 40.0f);
    
    // โหลดรูป 3 อาชีพหน้าเลือกตัวละคร
    setupSprite(texWar, sprWar, "warrior.png", 80.0f);
    setupSprite(texMag, sprMag, "mage.png", 80.0f);
    setupSprite(texRog, sprRog, "rouge.png", 80.0f);

    //ฉากต่อสู้ 
    setupSprite(texPlBatt, sprPlBatt, "player_battle.png", 120.0f);
    setupSprite(texEneBatt, sprEneBatt, "enemy.png", 120.0f);
    setupSprite(texBossBatt, sprBossBatt, "boss.png", 120.0f);
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
        "pixel.ttf", //font ใหม่ที่เพิ่มเข้ามา ถ้าอยากเปลี่ยนก็เอามาเปลี่ยนได้เลยนะ 
        "/opt/homebrew/share/fonts/dejavu-fonts/DejaVuSans.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        nullptr
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
            if(k == K::Enter) { 
                pl.job = (Job)selClass; 
                initPlayer(); 
                genMap(); 
                gs = GS::MAP;
                
                if(pl.job == Job::WAR) {
                    //ตัวละครผู้เล่นในแมพ
                    sprPlayer.setTexture(texWar, true);
                    sprPlayer.setScale({40.0f / texWar.getSize().x, 40.0f / texWar.getSize().y});
                    //ตัวละครผู้เล่นตอนต่อสสู้
                    sprPlBatt.setTexture(texWar, true);
                    sprPlBatt.setScale({120.0f / texWar.getSize().x, 120.0f / texWar.getSize().y});
                } else if(pl.job == Job::MAG) {
                    //
                    sprPlayer.setTexture(texMag, true);
                    sprPlayer.setScale({40.0f / texMag.getSize().x, 40.0f / texMag.getSize().y});
                    //
                    sprPlBatt.setTexture(texMag, true);
                    sprPlBatt.setScale({120.0f / texMag.getSize().x, 120.0f / texMag.getSize().y});
                } else if(pl.job == Job::ROG) {
                    //
                    sprPlayer.setTexture(texRog, true);
                    sprPlayer.setScale({40.0f / texRog.getSize().x, 40.0f / texRog.getSize().y});
                    //
                    sprPlBatt.setTexture(texRog, true);
                    sprPlBatt.setScale({120.0f / texRog.getSize().x, 120.0f / texRog.getSize().y});
                }
            }
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
    int nx = pl.x + dx;
    int ny = pl.y + dy;

    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return;

    Tile& t = map[ny][nx];

    // 2. ป้องกันเดินทะลุกำแพง
    if (t.t == TT::WALL) return;

    pl.x = nx;
    pl.y = ny;

    if (t.t == TT::ENE || t.t == TT::BOSS) {
        bool isBoss = (t.t == TT::BOSS);
        startBattle(mkEnemy(isBoss));
        t.t = TT::FLOOR; 
    }
    else if (t.t == TT::ITEM) {
        pl.inv.push_back(mkItem());
        t.t = TT::FLOOR;
        addLog("Found an Item!");
    }
    else if (t.t == TT::STAIR) {
        if (enemyCount() == 0) {
            descend();
        } else {
            addLog("Defeat all enemies first!");
        }
    }

    t.vis = true;
    updateVis(); 
}
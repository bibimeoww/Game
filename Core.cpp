#include "Game.h"
#include <algorithm>
#include <cctype>
#include <iostream>

namespace {
std::string trimCopy(const std::string &s) {
  const auto begin = std::find_if_not(s.begin(), s.end(), [](unsigned char ch) {
    return std::isspace(ch);
  });
  const auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) {
    return std::isspace(ch);
  }).base();

  if (begin >= end)
    return "";
  return std::string(begin, end);
}
} // namespace

Game::Game()
    : win(sf::VideoMode::getDesktopMode(), "DUNGEON OF FATE",
          sf::Style::Fullscreen),
      sprPlayer(texPlayer), sprWall(texWall), sprFloor(texFloor),
      sprEne(texEne), sprBoss(texBoss), sprItem(texItem), sprGate(texGate),
      sprMerchant(texMerchant),
      sprWar(texWar), sprMag(texMag), sprRog(texRog),
      sprPotionShop(texPotionShop), sprSwordShop(texSwordShop),
      sprCoinShop(texCoinShop),
      sprPlBatt(texPlBatt),
      sprEneBatt(texEneBatt), sprBossBatt(texBossBatt), sprBg(texBg),
      sprAtk(texAtk), sprSkill(texSkill), sprTrophy(texTrophy),
      gs(GS::INTRO), selClass(0), selAct(0),
      cur(0), at(0), shk(0), mt(0), it(0) {

  win.setFramerateLimit(60);
  win.setView(sf::View(sf::FloatRect(0.f, 0.f, (float)W, (float)H)));
  loadFont();

  // ฟังก์ชันตัวช่วยโหลดรูปและปรับขนาด
  auto setupSprite = [](sf::Texture &tex, sf::Sprite &spr,
                        const std::string &file, float targetSize) {
    bool loaded = tex.loadFromFile(file);
    if (!loaded) {
      loaded = tex.loadFromFile("character images/" + file);
    }
    if (loaded) {
      tex.setSmooth(false);
      spr.setTexture(tex, true);
      spr.setScale(
          {targetSize / tex.getSize().x, targetSize / tex.getSize().y});
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
  setupSprite(texGate, sprGate, "gate.png", 40.0f);
  if (texGate.getSize().x == 0 || texGate.getSize().y == 0) {
    setupSprite(texGate, sprGate, "stair.png", 40.0f);
  }
  setupSprite(texMerchant, sprMerchant, "merchant.png", 48.0f);

  // โหลดรูป 3 อาชีพหน้าเลือกตัวละคร
  setupSprite(texWar, sprWar, "warrior.png", 80.0f);
  setupSprite(texMag, sprMag, "mage.png", 80.0f);
  setupSprite(texRog, sprRog, "rouge.png", 80.0f);
  setupSprite(texPotionShop, sprPotionShop, "potion.png", 28.0f);
  setupSprite(texSwordShop, sprSwordShop, "sword.png", 28.0f);
  setupSprite(texCoinShop, sprCoinShop, "coin.png", 28.0f);

  // ฉากต่อสู้
  setupSprite(texPlBatt, sprPlBatt, "player_battle.png", 120.0f);
  setupSprite(texEneBatt, sprEneBatt, "enemy.png", 120.0f);
  setupSprite(texBossBatt, sprBossBatt, "boss.png", 120.0f);

  // กราฟฟิกพื้นหลัง
  if (texBg.loadFromFile("background.png") ||
      texBg.loadFromFile("character images/background.png")) {
    texBg.setSmooth(true);
    sprBg.setTexture(texBg, true);
    sprBg.setScale(
        {(float)W / texBg.getSize().x, (float)H / texBg.getSize().y});
  } else {
    printf("WARNING: Image background.png not found!\n");
  }

  // เอฟเฟกต์โจมตี
  if (texAtk.loadFromFile("attack.png") ||
      texAtk.loadFromFile("character images/attack.png")) {
    texAtk.setSmooth(true);
    sprAtk.setTexture(texAtk, true);
    // ปรับขนาดให้เหมาะสมกับศัตรูในฉากต่อสู้
    sprAtk.setScale({120.0f / texAtk.getSize().x, 120.0f / texAtk.getSize().y});
  } else {
    printf("WARNING: Image character images/attack.png not found!\n");
  }

  // เอฟเฟกต์สกิล
  if (texSkill.loadFromFile("skill.png") ||
      texSkill.loadFromFile("character images/skill.png")) {
    texSkill.setSmooth(true);
    sprSkill.setTexture(texSkill, true);
    // ปรับขนาดให้เหมาะสม
    sprSkill.setScale(
        {120.0f / texSkill.getSize().x, 120.0f / texSkill.getSize().y});
  } else {
    printf("WARNING: Image character images/skill.png not found!\n");
  }

  setupSprite(texTrophy, sprTrophy, "trophy.png", 220.0f);
}

void Game::run() {
  sf::Clock clk;
  while (win.isOpen()) {
    float dt = clk.restart().asSeconds();
    events();
    update(dt);
    draw();
  }
}

void Game::loadFont() {
  const char *p[] = {
      "pixel.ttf", // font ใหม่ที่เพิ่มเข้ามา ถ้าอยากเปลี่ยนก็เอามาเปลี่ยนได้เลยนะ
      "/opt/homebrew/share/fonts/dejavu-fonts/DejaVuSans.ttf",
      "/System/Library/Fonts/Supplemental/Arial.ttf",
      "/System/Library/Fonts/Helvetica.ttc",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
      nullptr};
  for (int i = 0; p[i]; i++)
    if (fnt.loadFromFile(p[i]))
      return;
  std::cerr << "Font not found!\n";
}

void Game::addLog(const std::string &s) {
  log.push_back(s);
  if ((int)log.size() > 8)
    log.erase(log.begin());
}

void Game::events() {
  sf::Event event;
  while (win.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      win.close();
    } else if (event.type == sf::Event::KeyPressed) {
      handleKey(event.key.code);
    } else if (event.type == sf::Event::TextEntered) {
      char c = static_cast<char>(event.text.unicode);
      if (gs == GS::NAME && c >= 32 && c < 127 && inp.length() < 12) {
        inp += c;
      } else if (gs == GS::AGE && std::isdigit(static_cast<unsigned char>(c)) &&
                 inp.length() < 3) {
        inp += c;
      }
    }
  }
}

void Game::handleKey(sf::Keyboard::Key k) {
  using K = sf::Keyboard::Key;
  if (k == K::Escape) {
    win.close();
    return;
  }

  switch (gs) {
  case GS::INTRO:
    if (k == K::Enter || k == K::Space)
      gs = GS::NAME;
    break;
  case GS::NAME:
    if (k == K::Enter && !inp.empty()) {
      std::string enteredName = trimCopy(inp);
      if (enteredName.empty()) {
        inp.clear();
        break;
      }

      pl.name = enteredName;
      inp = "";
      pl.age = 0;
      gs = GS::AGE;
    } else if (k == K::Backspace && !inp.empty())
      inp.pop_back();
    break;
  case GS::AGE:
    if (k == K::Enter && !inp.empty()) {
      int enteredAge = std::stoi(inp);
      if (enteredAge <= 0 || enteredAge > 130) {
        inp = "";
        break;
      }
      pl.age = enteredAge;
      inp = "";
      gs = GS::CLASS;
    } else if (k == K::Backspace && !inp.empty())
      inp.pop_back();
    break;
  case GS::CLASS:
    if (k == K::Left)
      selClass = (selClass + 2) % 3;
    if (k == K::Right)
      selClass = (selClass + 1) % 3;
    if (k == K::Enter) {
      pl.job = (Job)selClass;
      initPlayer();
      genMap();
      gs = GS::MAP;

      if (pl.job == Job::WAR) {
        sprPlayer.setTexture(texWar, true);
        sprPlayer.setScale(
            {40.0f / texWar.getSize().x, 40.0f / texWar.getSize().y});
        sprPlBatt.setTexture(texWar, true);
        sprPlBatt.setScale(
            {120.0f / texWar.getSize().x, 120.0f / texWar.getSize().y});
      } else if (pl.job == Job::MAG) {
        sprPlayer.setTexture(texMag, true);
        sprPlayer.setScale(
            {40.0f / texMag.getSize().x, 40.0f / texMag.getSize().y});
        sprPlBatt.setTexture(texMag, true);
        sprPlBatt.setScale(
            {120.0f / texMag.getSize().x, 120.0f / texMag.getSize().y});
      } else if (pl.job == Job::ROG) {
        sprPlayer.setTexture(texRog, true);
        sprPlayer.setScale(
            {40.0f / texRog.getSize().x, 40.0f / texRog.getSize().y});
        sprPlBatt.setTexture(texRog, true);
        sprPlBatt.setScale(
            {120.0f / texRog.getSize().x, 120.0f / texRog.getSize().y});
      }
    }
    break;
  case GS::MAP:
    if (k == K::W || k == K::Up)
      move(0, -1);
    if (k == K::S || k == K::Down)
      move(0, 1);
    if (k == K::A || k == K::Left)
      move(-1, 0);
    if (k == K::D || k == K::Right)
      move(1, 0);
    if (k == K::N) {
      if (enemyCount() == 0)
        descend();
      else
        addLog("Still enemies left on this floor!");
    }
    if (pl.s.hp <= 0)
      gs = GS::DEAD;
    break;
  case GS::BATTLE:
    if (bs == BS::PTURN) {
      if (k == K::Left)
        selAct = (selAct + 3) % 4;
      if (k == K::Right)
        selAct = (selAct + 1) % 4;
      if (k == K::Up || k == K::Down)
        selAct = (selAct + 2) % 4;
      if (k == K::Enter)
        doPlayerAction(selAct);
    } else {
      if (k == K::Enter)
        gs = (pl.s.hp <= 0) ? GS::DEAD : GS::MAP;
    }
    break;
  case GS::EVENT:
    if (k == K::Enter || k == K::Space)
      gs = (pl.s.hp <= 0) ? GS::DEAD : GS::MAP;
    break;
  case GS::DEAD:
    if (k == K::Left)
      gameOverOffsetX -= 10.0f;
    if (k == K::Right)
      gameOverOffsetX += 10.0f;
    if (k == K::Up)
      gameOverOffsetY -= 10.0f;
    if (k == K::Down)
      gameOverOffsetY += 10.0f;
    if (k == K::Enter) {
      gs = GS::INTRO;
      it = 0;
      log.clear();
      gameOverOffsetX = 0.0f;
      gameOverOffsetY = 0.0f;
    }
    break;
  case GS::WIN:
    if (k == K::Left)
      trophyOffsetX -= 10.0f;
    if (k == K::Right)
      trophyOffsetX += 10.0f;
    if (k == K::Up)
      trophyOffsetY -= 10.0f;
    if (k == K::Down)
      trophyOffsetY += 10.0f;
    if (k == K::Enter) {
      gs = GS::INTRO;
      it = 0;
      log.clear();
      trophyOffsetX = 0.0f;
      trophyOffsetY = 0.0f;
    }
    break;
  case GS::SHOP:

    if(k == K::Up) shopSel = (shopSel + 2) % 3;
    if(k == K::Down) shopSel = (shopSel + 1) % 3;

    if(k == K::Enter){

        if(shopSel == 0){ // potion
            if(pl.gold >= 20){
                pl.gold -= 20;
                pl.inv.push_back({"Potion",40,0,0,"Restores HP"});
                addLog("Bought Potion!");
            }else{
                addLog("Not enough gold!");
            }
        }

        if(shopSel == 1){ // sword
            if(pl.gold >= 50){
                pl.gold -= 50;
                pl.s.atk += 5;
                addLog("Bought Sword! ATK +5");
            }else{
                addLog("Not enough gold!");
            }
        }

        if(shopSel == 2){
            gs = GS::MAP;
        }
    }

  break;
}
}

void Game::update(float dt) {
  cur += dt * 2.f;
  if (cur > 2.f)
    cur = 0;
  at += dt;
  if (shk > 0)
    shk -= dt;
  if (atkAnimTime > 0)
    atkAnimTime -= dt;
  if (skillAnimTime > 0)
    skillAnimTime -= dt;
}

void Game::move(int dx, int dy) {
  int nx = pl.x + dx;
  int ny = pl.y + dy;

  if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS)
    return;

  Tile &t = map[ny][nx];

  // 2. ป้องกันเดินทะลุกำแพง
  if (t.t == TT::WALL)
    return;

  pl.x = nx;
  pl.y = ny;

  if (t.t == TT::ENE || t.t == TT::BOSS) {
    bool isBoss = (t.t == TT::BOSS);
    startBattle(mkEnemy(isBoss));
    t.t = TT::FLOOR;
  } else if (t.t == TT::ITEM) {
    pl.inv.push_back(mkItem());
    t.t = TT::FLOOR;
    addLog("Found an Item!");
  }else if (t.t == TT::SHOP) {
    openShop();
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

void Game::openShop(){
    shopSel = 0;
    gs = GS::SHOP;
}

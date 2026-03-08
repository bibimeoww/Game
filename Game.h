#pragma once
#include "GameTypes.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

class Game {
public:
  Game();
  void run();

private:
  sf::RenderWindow win;
  sf::Font fnt;
  // ตัวละครหลัก
  sf::Texture texPlayer;
  sf::Sprite sprPlayer;
  // กราฟฟิกแผนที่,สิ่งของ
  sf::Texture texFloor, texWall, texStair, texEne, texBoss, texItem;
  sf::Sprite sprFloor, sprWall, sprStair, sprEne, sprBoss, sprItem;
  // กราฟฟิกหน้าตต่อสู้
  sf::Texture texPlBatt, texEneBatt, texBossBatt;
  sf::Sprite sprPlBatt, sprEneBatt, sprBossBatt;

  // กราฟฟิกพื้นหลัง
  sf::Texture texBg;
  sf::Sprite sprBg;

  // เอฟเฟกต์การโจมตี
  sf::Texture texAtk;
  sf::Sprite sprAtk;
  float atkAnimTime = 0.0f;

  // เอฟเฟกต์สกิล
  sf::Texture texSkill;
  sf::Sprite sprSkill;
  float skillAnimTime = 0.0f;

  // กราฟฟิกหน้าเลือกอาชีพ
  sf::Texture texWar, texMag, texRog;
  sf::Sprite sprWar, sprMag, sprRog;

  GS gs;
  Player pl;
  std::string inp;
  float cur, at, shk, mt, it;
  int selClass, selAct;
  std::vector<std::vector<Tile>> map;
  std::vector<Enemy> elist;
  Enemy batt{};
  BS bs = BS::PTURN;
  std::string res, evtMsg;
  std::vector<std::string> log;

  // --- Core (คนที่ 1) ---
  void events();
  void handleKey(sf::Keyboard::Key k);
  void update(float dt);
  void move(int dx, int dy);
  void loadFont();
  void addLog(const std::string &s);

  // --- Battle (คนที่ 2) ---
  void initPlayer();
  std::string jname(Job j);
  std::string skname();
  void gainExp(int v);
  int dmg(int a, int d, float m = 1.f);
  void startBattle(Enemy e);
  bool battleEnd();
  void doPlayerAction(int a);
  void enemyTurn();

  // --- UI (คนที่ 3) ---
  void draw();
  void dt_(const std::string &s, unsigned sz, sf::Color c, float x, float y);
  void dtc(const std::string &s, unsigned sz, sf::Color c, float cx, float y);
  void dr(float x, float y, float w, float h, sf::Color c, bool ol = false,
          sf::Color oc = sf::Color::White);
  void bar(float x, float y, float w, float h, float ratio, sf::Color fg,
           const std::string &lb = "");
  void dIntro();
  void dInput(const std::string &prompt, bool isAge);
  void dClass();
  void dMap(float sx, float sy);
  void dBattle(float sx, float sy);
  void dEvent();
  void dEnd(bool win);

  // --- Dungeon (คนที่ 4) ---
  void genMap();
  void revealAll();
  Enemy mkEnemy(bool boss);
  Item mkItem();
  void updateVis();
  int enemyCount();
  void descend();
  void trigEvt();
};

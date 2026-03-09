#include "Game.h"
#include <algorithm>

void Game::initPlayer() {
  pl.lv = 1;
  pl.exp = 0;
  pl.nexp = 100;
  pl.gold = 50;
  pl.score = 0;
  pl.floor = 1;
  pl.scd = 0;
  pl.inv.clear();
  pl.inv.push_back({"Health Potion", 40, 0, 0, "Restores 40 HP"});
  switch (pl.job) {
  case Job::WAR:
    pl.s = {120, 120, 18, 12, 8};
    break;
  case Job::MAG:
    pl.s = {80, 80, 28, 6, 10};
    break;
  case Job::ROG:
    pl.s = {100, 100, 20, 8, 14};
    break;
  default:
    break;
  }
}

std::string Game::jname(Job j) {
  if (j == Job::WAR)
    return "Warrior";
  if (j == Job::MAG)
    return "Mage";
  if (j == Job::ROG)
    return "Rogue";
  return "?";
}

std::string Game::skname() {
  if (pl.job == Job::WAR)
    return "Power Strike";
  if (pl.job == Job::MAG)
    return "Fireball";
  if (pl.job == Job::ROG)
    return "Backstab";
  return "Skill";
}

void Game::gainExp(int v) {
  pl.exp += v;
  pl.score += v;
  while (pl.exp >= pl.nexp) {
    pl.exp -= pl.nexp;
    pl.lv++;
    pl.nexp = 100 + pl.lv * 50;
    if (pl.job == Job::WAR) {
      pl.s.mhp += 20;
      pl.s.atk += 3;
      pl.s.def += 2;
    } else if (pl.job == Job::MAG) {
      pl.s.mhp += 10;
      pl.s.atk += 5;
      pl.s.def += 1;
    } else {
      pl.s.mhp += 15;
      pl.s.atk += 4;
      pl.s.spd += 2;
    }
    pl.s.hp = std::min(pl.s.hp + 15, pl.s.mhp);
    addLog("LEVEL UP! Lv." + ts(pl.lv));
  }
}

int Game::dmg(int a, int d, float m) {
  return std::max(1, (int)((a - d * .5f) * m) + rnd(-3, 3));
}

void Game::startBattle(Enemy e) {
  batt = e;
  bs = BS::PTURN;
  log.clear();
  addLog("Battle! " + e.name + " appeared!");
  selAct = 0;
  gs = GS::BATTLE;
}

bool Game::battleEnd() {
  if (batt.s.hp <= 0) {
    gainExp(batt.exp);
    pl.gold += batt.gold;
    pl.score += batt.gold * 2;
    res = "Victory! +" + ts(batt.exp) + " EXP +" + ts(batt.gold) + " Gold";
    addLog(res);
    if (batt.boss && pl.floor >= 9) {
      gs = GS::WIN;
      return true;
    }
    bs = BS::RESULT;
    return true;
  }
  if (pl.s.hp <= 0) {
    pl.s.hp = 0;
    res = "You were defeated...";
    addLog(res);
    bs = BS::RESULT;
    return true;
  }
  return false;
}

void Game::doPlayerAction(int a) {
  if (a == 0) {
    int d = dmg(pl.s.atk, batt.s.def);
    batt.s.hp -= d;
    addLog("You attack for " + ts(d) + " dmg!");
    atkAnimTime = 0.5f; // trigger animation
  } else if (a == 1) {
    if (pl.scd > 0) {
      addLog("Skill on cooldown (" + ts(pl.scd) + ")!");
      return;
    }
    int d = 0;
    if (pl.job == Job::WAR) {
      d = dmg(pl.s.atk, batt.s.def, 2.f);
      pl.scd = 1;
      addLog("Power Strike! " + ts(d) + " dmg!");
      skillAnimTime = 0.5f; // trigger skill animation
    } else if (pl.job == Job::MAG) {
      d = dmg(pl.s.atk, 0, 3.f);
      pl.scd = 2;
      addLog("Fireball! " + ts(d) + " magic!");
      skillAnimTime = 0.5f; // trigger skill animation
    } else {
      d = dmg(pl.s.atk, 0, 2.5f);
      pl.scd = 1;
      addLog("Backstab! " + ts(d) + " true dmg!");
      skillAnimTime = 0.5f; // trigger skill animation
    }
    batt.s.hp -= d;
  } else if (a == 2) {
    if (pl.inv.empty()) {
      addLog("No items!");
      return;
    }
    auto &item = pl.inv[0];
    if (item.hp > 0) {
      pl.s.hp = std::min(pl.s.mhp, pl.s.hp + item.hp);
      addLog("Used " + item.name + "! +" + ts(item.hp) + " HP.");
    }
    pl.inv.erase(pl.inv.begin());
  } else {
    if (rnd(1, 100) <= 50) {
      addLog("Escaped!");
      gs = GS::MAP;
      return;
    } else
      addLog("Failed to escape!");
  }

  if (pl.scd > 0 && a != 1)
    pl.scd--;

  if (!battleEnd())
    enemyTurn();
}

void Game::enemyTurn() {
  if (batt.s.hp <= 0)
    return;
  bool sp = batt.boss && rnd(1, 100) <= 30;
  int d = sp ? dmg(batt.s.atk, pl.s.def, 1.8f) : dmg(batt.s.atk, pl.s.def);
  addLog(batt.name + (sp ? " Special! " : " attacks! ") + ts(d) + " dmg!");
  pl.s.hp -= d;
  shk = 0.3f;
  battleEnd();
}


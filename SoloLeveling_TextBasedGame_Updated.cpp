// Randomization of attack from Enemy is done in final boss fight, as per instructed.
// PLayer is attack based, and does not use healing abilities, but can heal by an special ability named life steal.
#include <iostream>
#include <random>
#include <ctime>
#include <algorithm>
#include <vector>
using namespace std;

class Player{
  // public:
  private:
  string name;
  float health, Defence, shieldDefence, armourDefence;
  int Melee_damage, Ranged_damage, MaxHealth, swordDamage, bowDamage;
  bool hasSword, hasShield, hasArmour, hasBow, hasCriticalHit, hasBlocker, hasLifeSteal, hasRangedAttack = false;
  int playerLevel = 1;
  bool usedRangedAttack = false;

  public:
  Player(float health, float Defence, int Melee_damage, int Ranged_damage): health(health),  Defence(Defence), Melee_damage(Melee_damage), Ranged_damage(Ranged_damage){};

  private:
  int takeDamagefromEmemy(int damage){
    if (damage == 0){//Blocker Activated
      return health;
    }
    else if (hasArmour){
      health = std::max(0.0f,health - (1-Defence-shieldDefence-armourDefence)*damage);
      return health;
    }
    else if(hasShield){
      health = std::max(0.0f,health - (1-Defence-shieldDefence)*damage);
      return health;
    }
    else{
      health = std::max(0.0f,health - (1-Defence)*damage);
      return health;
    }
  }

  bool isAlive(){
    return health > 0;
  }

  void levelUp() {
    playerLevel += 1;
    health = MaxHealth = max(MaxHealth * 1.1f, 100.0f);
    Defence += 0.05f;
    Melee_damage += 10;
    Ranged_damage += 5;
    cout << name << " leveled up to level " << playerLevel << "!" << endl;
  }

  public:
  friend class Game;
  friend class Level;
  ~Player() {}
};

class Enemy{
  // public:
  private:
  float health, Defence;
  int Attack;

  public:
  Enemy(float health, float Defence, int Attack): health(health), Defence(Defence), Attack(Attack){};

  private:
  int takeDamageFromPlayer(int damage){
    health = std::max(0.0f,health - (1-Defence)*damage);
    return health;
  }

  int giveDamage(){
    return Attack;
  }

  bool const isAlive() const{
    return health > 0;
  }
  public:
  friend class Game;
  friend class Level;
  Enemy() = default;
  ~Enemy() {};
};

class Boss : public Enemy {
  private:
  int GroundSlashDamage, SpeedDashDamage, maxBossHealth;
  float LifeRegeneration;

  public:
  Boss(float health, float defence, int attack, int GroundSlashDamage, int SpeedDashDamage, float LifeRegeneration)
      : Enemy (health, defence, attack),GroundSlashDamage(GroundSlashDamage), SpeedDashDamage(SpeedDashDamage),LifeRegeneration(LifeRegeneration) {}

  private:
  int getGroundSlashDamage() const {
      return GroundSlashDamage;
  }
  int getSpeedDashDamage() const {
      return SpeedDashDamage;
  }
  int getLifeRegeneration() const {
      return LifeRegeneration;
  }

  public:
  friend class Game;
  friend class Level;
  Boss() = default;
  ~Boss() {};
};

class Level {
private:
  int playerLevel;
  Boss boss;
  std::vector<Enemy> enemies;

public:
  Level(int playerLevel) : playerLevel(playerLevel) {}

  void startLevel(Player& player) {

    if (player.playerLevel < 6) {
      int numEnemies = player.playerLevel;
      enemies = create_enemies(playerLevel, numEnemies);

      std::cout << "----------------------------------------------" << std::endl;
      std::cout << player.name <<", Welcome to Level "<< player.playerLevel <<"!" << std::endl;
      printPlayerStat(player);

      enemies = create_enemies(player.playerLevel, numEnemies);
      std::cout << "You have encountered " << enemies.size() << " Level" << player.playerLevel <<" Dragon/s!" << std::endl;
      for (int i = 0; i < enemies.size(); ++i) {
        std::cout << "Dragon"<< i+1 <<":" << std::endl;
        printEnemyStat(enemies[i]);
      }
    } 
    else if (player.playerLevel == 6){
      printPlayerStat(player);
      enemies.clear();
      boss = Boss(300, 0.3, 20, 30, 20, 0.2f);
      enemies.push_back(boss);
      for (int i = 0; i < enemies.size(); ++i) {
        std::cout << "The Dragon Monarch:" << std::endl;
        printEnemyStat(enemies[i]);
      }
      std::cout << "But that's not it! \nFYI: The Dragon Monarch has hidden attacks like Ground Slash(dc = 30), Speed Dash(dc = 20) and Health Regenration(20%)!" << std::endl;
    }

    playLevel(player);
  }

  private:
  std::vector<Enemy> create_enemies(int player_level, int num_enemies) {
    std::vector<Enemy> enemies;

    for (int i = 0; i < num_enemies; i++) {
      float health = 50 + (player_level * 5);
      float defence = 0.05 + (player_level * 0.01);
      int attack = 5 + (player_level * 2);
      if (player_level == 6 && i == 0) {
        health = boss.health;
        boss.maxBossHealth = boss.health;
        defence = boss.Defence;
        attack = boss.Attack;
      }

      enemies.push_back(Enemy(health, defence, attack)); 
    }
    return enemies;
  }

  void playLevel(Player& player) {
    while(player.isAlive() && !enemies.empty())
    { //player attack to enemies
      std::cout << "Choose the attack to the Dragon: 1.Dagger(Melee) 2.Soul slice(Ranged)" << std::endl;
      int choice, DamageDealt;
      std::cin >> choice;
      switch(choice)
      {
        case 1:
          dealMeleeDamage(player, enemies);
          for (int i = 0; i < enemies.size(); ++i) {
            EnemyHealth(enemies[i]);
          }
          break;
        default:
          dealRangedDamage(player, enemies);
          for (int i = 0; i < enemies.size(); ++i) {
            EnemyHealth(enemies[i]);
          }
          break;
      }

      //Delete enemies if defeated
      removeDeadEnemies(enemies);

     //Enemy attack to player
      DamageDealt = 0;
      if (!enemies.empty()) {
        handleEnemyAttackCombined(player, enemies);
        PlayerHealth(player);
      }

      if (enemies.empty() && player.playerLevel == 6) {
        std::cout << "You have defeated all the eneemies! You win Final level, "<< player.playerLevel <<"! \nGame Finished!!" << std::endl;
        std::cout << player.name <<" is now Free to live!!!" << std::endl;
        std::cout << "---------------------------------------------------------------------" << std::endl;
        player.playerLevel += 1;
      }
      else if (enemies.empty()) {
        std::cout << "All dragons defeated! You win level "<< player.playerLevel <<"!" << std::endl;
        std::cout << player.name <<" can now go to level "<< player.playerLevel + 1 <<"!" << std::endl;
        player.levelUp();
      }
    }
  }

  void dealMeleeDamage(Player& player, std::vector<Enemy>& enemies) {
    int enemyChoice;
    std::cout << "Which dragon do you want to hit with dagger?" << std::endl;
    for (int i = 0; i < enemies.size(); ++i) {
      std::cout << i + 1 << ". " << "Dragon's Health: " << enemies[i].health << std::endl;
    }
    std::cin >> enemyChoice;

    Enemy& chosenEnemy = enemies[enemyChoice - 1];
    int damageDealt = player.Melee_damage;

    if (player.hasSword) {
        damageDealt += player.swordDamage;
    }

    if (player.hasCriticalHit && player.playerLevel > 2){
      if (rand() % 10 == 0) {
        damageDealt *= 2;
        std::cout << "Critical hit!" << std::endl;
      }
    }

    chosenEnemy.takeDamageFromPlayer(damageDealt);

    if (player.hasLifeSteal && player.playerLevel > 4){
      if (rand() % 10 == 0) {
        player.health += (0.1)*damageDealt;
        if (player.health > player.MaxHealth){
          player.health = player.MaxHealth;
        }
        std::cout << "Life steal activated! Your health has been restored by "<< std::min<float>((0.1f) * damageDealt, player.MaxHealth) << "."<< std::endl;
      }
    }

    if (!chosenEnemy.isAlive()) {
        enemies.erase(enemies.begin() + (enemyChoice - 1));
    }
  }

  void dealRangedDamage(Player& player, std::vector<Enemy>& enemies) {
    int damageDealt = player.Ranged_damage;
    if (player.hasBow) {
        damageDealt += player.bowDamage;
    }
    if (player.hasCriticalHit && player.playerLevel > 2){
      if (rand() % 10 == 0) {
        damageDealt *= 2;
        std::cout << "Critical hit!" << std::endl;
      }
    }
    if (player.hasLifeSteal && player.playerLevel > 4){
      if (rand() % 10 == 0) {
        player.health += (0.1)*damageDealt;
        if(player.health > player.MaxHealth){
          player.health = player.MaxHealth;
        }
        std::cout << "Life steal activated! Your health has been restored by "<< std::min<float>((0.1f) * damageDealt, player.MaxHealth) << "." << std::endl;
      }
    }
    if (player.hasRangedAttack && player.playerLevel == 6){
      if (rand() % 10 == 0) {
        std::cout << "Special Ability Ruler's Authority Activated! You are invincible to the next attack of the Dragon Monarch!!" << std::endl;
        player.usedRangedAttack = true;
      }
    }

    for (Enemy& enemy : enemies) {
        enemy.takeDamageFromPlayer(damageDealt);
    }
  }

  void handleEnemyAttackCombined(Player& player, std::vector<Enemy>& enemies) {

    std::cout << "Dragon/s Attacked as well!!" << std::endl;
    int DamageDealt = 0;
    for (Enemy& enemy : enemies) {
      if(enemy.isAlive() && (player.playerLevel == 6)){
        int attackChoice = rand() % 3;
        switch (attackChoice) {
          case 0:
            DamageDealt += boss.giveDamage();
            DamageDealt += boss.getGroundSlashDamage();
            std::cout << "Monarch used Ground Slash!" << std::endl;
            if (player.usedRangedAttack){
              DamageDealt = 0;
              std::cout << "You won't get affected by this attack as Ruler's Authority is activated!" << std::endl;
              player.usedRangedAttack = false;
            }
            break;
          case 1:
            DamageDealt += boss.giveDamage();
            DamageDealt += boss.getSpeedDashDamage();
            std::cout << "Monarch used Speed Dash!" << std::endl;
            if (player.usedRangedAttack){
              DamageDealt = 0;
              std::cout << "You won't get affected by this attack as Ruler's Authority is activated!" << std::endl;
              player.usedRangedAttack = false;
            }
            break;
          case 2:
            enemy.health += (enemy.health) * (boss.getLifeRegeneration());  // Regenerate health
            if (enemy.health > boss.maxBossHealth){
              enemy.health = boss.maxBossHealth;
            }
            std::cout << "The Dragon regenerated 20% of its health!" << std::endl;
            std::cout << "After regenration, Dragon's current health is: " <<enemy.health << std::endl;
            break;
        }
      }
      else if (enemy.isAlive()) {
        DamageDealt += enemy.giveDamage();
      }
    }
    if (player.hasBlocker && player.playerLevel > 3){
      if (rand() % 10 == 0) {
        DamageDealt = 0;
        std::cout << "Blocker Activated! You are invincible to the attack of the enemies." << std::endl;
      }
    }

    player.takeDamagefromEmemy(DamageDealt);

    if (player.health == 0) {
        PlayerHealth(player);
        std::cout << player.name << " is dead! Game Over!" << std::endl;
    }
  }

  void removeDeadEnemies(std::vector<Enemy>& enemies) {
      int aliveCount = 0;
      for (int i = 0; i < enemies.size(); ++i) {
          if (enemies[i].isAlive()) {
              if (i != aliveCount) {
                  std::swap(enemies[i], enemies[aliveCount]);
              }
              aliveCount++;
          }
      }
      enemies.resize(aliveCount);
  }

  void printPlayerStat(Player& player1){
    std::cout << player1.name<<"'s Health: " << player1.health <<std::endl;
    std::cout << player1.name<<"'s Dagger Damage: " << player1.Melee_damage <<std::endl;
    std::cout << player1.name<<"'s Soul slice Damage: " << player1.Ranged_damage <<std::endl;
    std::cout << player1.name<<"'s Defence: " << player1.Defence <<std::endl;
  }
  void PlayerHealth(Player& player1){
    std::cout << player1.name<<"'s Current Health: " << player1.health <<std::endl;
  }
  void printEnemyStat(const Enemy& enemy) {
    std::cout << "Dragon's Health: " << enemy.health << std::endl;
    std::cout << "Dragon's Attack: " << enemy.Attack << std::endl;
    std::cout << "Dragon's Defence: " << enemy.Defence << std::endl;
  }
  void EnemyHealth(const Enemy& enemy) {
    std::cout << "Dragon's Health: " << enemy.health << std::endl;
  }

  ~Level() {}
};

class Game{
  public:
  Player *player1 = nullptr;
  std::vector<Level> levels;
  Game() {
    for (int i = 1; i <= 6; ++i) { //set number of levels in the game
      levels.emplace_back(i);
    }
  }

  void start(){
    std::cout << "----------------------------------------------" << std::endl;
    cout << "Welcome to Solo Leveling!" << endl;
    cout << "The Dragon Monarch awaits!\nDragon Monarch: Welcome to the showdown, Shadow Monarch!\nLet's see if you have the calibre to fight me! Lets see if you can stay alive with my fellow companions first!" << endl;
    cout << "What do you call yourself Monarch, in this life?: ";
  }

  void loop(){
    srand(time(0));
    Player player1(100, 0.1, 10, 5);
    player1.MaxHealth = player1.health;
    int currentLevel = player1.playerLevel;
    
    while (player1.isAlive() && currentLevel < levels.size()) {
      // levels[player1.playerLevel].startLevel(player1);
      HandleLevels(player1);
      currentLevel += 1;
    }
  }

  private:
  void HandleLevels(Player& player1) {
    for (int level = player1.playerLevel-1;level <= 6;) {
      // Level-specific actions:
      switch (player1.playerLevel) {
        case 1:
          std::cin >> player1.name;
          levels[player1.playerLevel].startLevel(player1);
          break;
        case 2:
          levels[player1.playerLevel].startLevel(player1);
          if (player1.isAlive()) {
            player1.hasSword = true;
            player1.hasCriticalHit = true;
            player1.swordDamage = 20;
            std::cout << "You have been awarded a special ability of Critical Hit(damage boost of 100%!!!), which has probability of activation = 10%. \nYou found a special Dagger with Melee Attack damage of 20!!!" << std::endl;
          }
          break;
        case 3:
          levels[player1.playerLevel].startLevel(player1);
          if (player1.isAlive()) {
            player1.hasShield = true;
            player1.shieldDefence = 0.2;
            player1.hasBlocker = true;
            std::cout << "You have been awarded a special ability of Blocker(will get damage nulliefied on enemy hit!!!), which has probability of activation = 10%. \nYou found a Shield with additional defence boost of 10%!!!" << std::endl;
          }
          break;
        case 4:
          levels[player1.playerLevel].startLevel(player1);
          if (player1.isAlive()) {
            player1.hasArmour = true;
            player1.armourDefence = 0.1;
            player1.hasLifeSteal = true;
            std::cout << "You have been awarded a special ability of Life Steal(will get HP recovery on enemy hit!!!), which has probability of activation = 10%. \nYou found an Armer with additional defence boost of 10%!!!" << std::endl;
          }
          break;
        case 5:
          levels[player1.playerLevel-1].startLevel(player1);
          if (player1.isAlive()) {
            player1.hasBow = true;
            player1.hasRangedAttack = true;
            player1.bowDamage = 10;
            std::cout << "You have been awarded a special ability of Ruler's Authority(will get immune to the next attack of the Dragon Monarch!), which has probability of activation = 10%. \nYou found an Bow with Soul Attack damage of 10!!!" << std::endl;
          }
          break;
        case 6:
          std::cout << "\n\n\n\n\n\n----------------------------------------------" << std::endl;
          std::cout << player1.name <<", Welcome to Final Dragon Monarch's lier!" << std::endl;
          std::cout << "The Dragon Monarch is waiting for you at the end of the lier! \n Defeat him if you can to get yourself free or DIE!!!" << std::endl;
          std::cout << "You have encountered The Dragon Monarch!" << std::endl;
          std::cout << "----------------------------------------------" << std::endl;
          levels[player1.playerLevel].startLevel(player1);
          break;
      }

      if (!player1.isAlive()) {
        std::cout << "Game Over!" << std::endl;
      }

    }
  }

  public:
  ~Game(){}
};

int main() {
  Game game;
  game.start();
  game.loop();
  return 0;
}

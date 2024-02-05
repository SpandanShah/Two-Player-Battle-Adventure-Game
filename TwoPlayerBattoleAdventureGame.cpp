#include <iostream>
#include <random>
#include <ctime>

class Player
{
public:
  virtual void printStats() const = 0;

   int health, baseDamage, maxHealth, minHeal, maxHeal, minAdditionalDamage, maxAdditionalDamage;

   Player(int initialHealth, int initialBaseDamage, int initialMaxHealth, int initialMinHeal, int initialMaxHeal,
           int initialMinAdditionalDamage, int initialMaxAdditionalDamage)
        : health(initialHealth),
          baseDamage(initialBaseDamage),
          maxHealth(initialMaxHealth),
          minHeal(initialMinHeal),
          maxHeal(initialMaxHeal),
          minAdditionalDamage(initialMinAdditionalDamage),
          maxAdditionalDamage(initialMaxAdditionalDamage) {}

  static int generateRandomInRange(int min, int max)
  {
    int randNum = rand() % (max - min + 1) + min;
    return randNum;
  }

  void takeDamage(int damage)
  {
    health -= damage;
    if (health < 0)
      health = 0;
  }

  int giveDamage()
  {
    return baseDamage + generateRandomInRange(minAdditionalDamage, maxAdditionalDamage);
  }

  void heal(int amount)
  {
    health += amount;
    if (health > maxHealth)
      health = maxHealth;
  }

  virtual ~Player() = default;
};

class Tank : public Player
{
public:
  Tank()
        : Player(200, 15, 200, 20, 30, 0, 15) {}

  void printStats() const override
  {
    std::cout << "Tank:" << std::endl;
    std::cout << "\tHealth: " << health << std::endl;
    std::cout << "\tBase Damage: " << baseDamage << std::endl;
  }

};

class Mage : public Player
{
public:

  Mage()
        : Player(150, 25, 150, 10, 20, 0, 20) {}

  void printStats() const override
  {
    std::cout << "Mage:" << std::endl;
    std::cout << "\tHealth: " << health << std::endl;
    std::cout << "\tBase Damage: " << baseDamage << std::endl;
  }

};

class Assassin : public Player
{
public:
  Assassin()
        : Player(100, 35, 100, 5, 15, 0, 25) {}

  void printStats() const override
  {
    std::cout << "Assasin:" << std::endl;
    std::cout << "\tHealth: " << health << std::endl;
    std::cout << "\tBase Damage: " << baseDamage << std::endl;
  }

};

class Game
{
  public:
  Player *player1 = nullptr;
  Player *player2 = nullptr;
  void start()
  {
    srand(time(0));
    std::cout << "Welcome to the Battle Game!" << std::endl;
    std::cout << "Choose your character:" << std::endl;
    std::cout << "Player 1, choose your class (1: Tank, 2: Mage, 3: Assassin): ";
    player1 = getPlayerChoice();
    std::cout << "Player 2, choose your class (1: Tank, 2: Mage, 3: Assassin): ";
    player2 = getPlayerChoice();

    std::cout << "\nInitial Player Health:" << std::endl;
    std::cout << "Player 1 health: " << player1->health << std::endl;
    std::cout << "Player 2 health: " << player2->health << std::endl;

    while (player1->health > 0 && player2->health > 0)
    {
        handlePlayerTurn(player1, player2);
        if (player2->health == 0)
        {
          std::cout << "\nPlayer 2 is dead!" << std::endl;
          std::cout << "\nPlayer 1 wins!" << std::endl;
          break;
        }

        handlePlayerTurn(player2, player1);
        if (player1->health == 0)
        {
          std::cout << "\nPlayer 1 is dead!" << std::endl;
          std::cout << "\nPlayer 2 wins!" << std::endl;
          break;
        }
    }
    delete player1;
    delete player2;
  }

  private:

  Player *getPlayerChoice(){
    int playerChoice;
    Player *player = nullptr;

    std::cin >> playerChoice;

    switch (playerChoice)
    {
      case 1:
        player = new Tank();
        break;
      case 2:
        player = new Mage();
        break;
      case 3:
        player = new Assassin();
        break;
      default:
        std::cout << "Invalid choice!" << std::endl;
        return nullptr;
    }

    return player;
  }

  void handlePlayerTurn(Player *currentPlayer, Player *otherPlayer)
  {
      std::cout << "\n" << (currentPlayer == player1 ? "Player1" : "Player2") << "'s turn:" << std::endl;
      currentPlayer->printStats();
      std::cout << "Choose action (1: Heal, 2: Attack): ";
      int playerChoice;
      std::cin >> playerChoice;
      int damageDealt;

      switch (playerChoice)
      {
        case 2:
          damageDealt = currentPlayer->giveDamage();
          otherPlayer->takeDamage(damageDealt);
          std::cout << "Dealt " << damageDealt << " damage to " << (currentPlayer == player1 ? "Player2" : "Player1") << "!" << std::endl;
          break;
        default:
          if (currentPlayer->health >= currentPlayer->maxHealth)
          {
              std::cout << "Already at full health, turn lost!" << std::endl;
          }
          else
          {
              int healAmount = currentPlayer->generateRandomInRange(currentPlayer->minHeal, currentPlayer->maxHeal);
              currentPlayer->heal(healAmount);
              std::cout << "Healed for " << healAmount << " health!" << std::endl;
          }
          break;
      }
  }
};

int main()
{
  Game game;
  game.start();
  return 0;
}
#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <vector>

using namespace std;

struct game_state 
{
    string name;
    int gold;
    vector<string> inventory;
    int honor;
    int progress;
    string armor;
    string weapon;
    int hp;
    int george;

    void save(const string& filename) const;
    void load(const string& filename);
};

class Enemy {
private:
    std::string name;
    int hp;
    int attack1_dmg;
    int attack2_dmg;

public:
    Enemy(std::string n, int h, int a1, int a2);
    void take_damage(int damage);
    void attack1(game_state& target);
    void attack2(game_state& target);
    std::string get_name();
    int get_hp();
    bool is_alive();
};

game_state new_game();
void load_game();
void exit_game();
void ensure_exists();
int armor_res(game_state& game);
void level_0(game_state game);
void level_1(game_state& game);
void level_2(game_state& game);
void level_3(game_state& game);
void level_4(game_state& game);

void end_level(game_state& game);

#endif // HELPERS_H

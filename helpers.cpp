#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>
#include "helpers.h"


#include <windows.h>
#include <direct.h>

using namespace std;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

// class for enemys
Enemy::Enemy(string n, int h, int a1, int a2) : name(n), hp(h), attack1_dmg(a1), attack2_dmg(a2) {}

void Enemy::take_damage(int damage) 
{
    hp -= damage;
    if (hp < 0) hp = 0;
    cout << name << " takes " << damage << " damage. Remaining Dragon's health: " << hp << "\n";
}

void Enemy::attack1(game_state& target) 
{
    int total;
    cout << name << " attacks " << target.name << " with claws for " << attack1_dmg << " damage.\n";
    total = attack1_dmg - armor_res(target);
    if (total < 0)
    {
        total = 0;
    }
    target.hp -= total;
}

void Enemy::attack2(game_state& target) 
{
    int total;
    cout << name << " attacks " << target.name << " with fire breath for " << attack2_dmg << " damage.\n";
    total = attack2_dmg - armor_res(target);
    if (total < 0)
    {
        total = 0;
    }
    target.hp -= total;
}

string Enemy::get_name() 
{
    return name;
}

int Enemy::get_hp() 
{
    return hp;
}

bool Enemy::is_alive() 
{
    return hp > 0;
}

void game_state::save(const string& filename) const {
    const string directory = "saves";

    ofstream file(directory + "/" + filename);
    if (!file.is_open()) {
        cerr << "Unable to open file for writing: " << directory + "/" + filename << endl;
        return;
    }

    file << name << endl;
    file << gold << endl;
    file << honor << endl;
    file << progress << endl;
    file << armor << endl;
    file << weapon << endl;
    file << hp << endl;
    
    // Save inventory
    file << inventory.size() << endl;
    for (const auto& item : inventory) {
        file << item << endl;
    }

    file << george << endl;
    file.close();
}


void game_state::load(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Unable to open file for reading: " << filename << endl;
        return;
    }

    getline(file, name);
    file >> gold;
    file >> honor;
    file >> progress;
    file.ignore(); 
    getline(file, armor);
    getline(file, weapon);
    file >> hp;
    file.ignore(); 

    // Load inventory
    size_t inventory_size;
    file >> inventory_size;
    file.ignore(); 
    inventory.clear();
    for (size_t i = 0; i < inventory_size; ++i) {
        string item;
        getline(file, item);
        inventory.push_back(item);
    }
    file >> george;
    file.close();
}


wstring string_to_widestring(const string& str) 
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// prints message with delay, char by char
void print_message(const string& message, unsigned int delay = 40000) 
{
    for (char c : message) 
    {
        cout << c << flush;
        usleep(delay);  
    }
    cout << endl;  
}

// sets values when game starts
game_state new_game() 
{
    cout << "Starting a new game...\n";
    game_state game;
    game.gold = 50;
    game.inventory = {"water-bomb"};
    game.honor = 0;
    game.progress = 0;
    game.armor = "dirty firefighter suit";
    game.weapon = "old fire extinguisher";
    game.hp = 100;
    game.george = 0;
    sleep(2);

    return game;
}

void load_level(game_state& game)
{
   switch (game.progress) 
        {
            case 0:
                level_0(game);
                break;
            case 1:
                level_1(game);
                break;
            case 2:
                level_2(game);
                break;
            case 3:
                level_3(game);
                break;
            case 4:
                level_4(game);
                break;
            default:
                cout << "Unknown level\n";
                break;
        } 
}

void load_game() 
{
    ensure_exists();

    vector<string> saveFiles;
    WIN32_FIND_DATAW findFileData; //  WIN32_FIND_DATAW for Unicode
    HANDLE hFind = FindFirstFileW(L"saves\\*", &findFileData); 

    if (hFind != INVALID_HANDLE_VALUE) 
    {
        do 
        {
            wstring file_name(findFileData.cFileName); 
            const string file_name_utf8 = string(file_name.begin(), file_name.end()); // UTF-8

            const bool is_directory = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            if (!is_directory) 
            {
                saveFiles.push_back(file_name_utf8);
            }
        } while (FindNextFileW(hFind, &findFileData) != 0); 
        FindClose(hFind);
    }

    if (saveFiles.empty()) 
    {
        cout << "No save files found.\n";
        return;
    }

    cout << "Select a save file to load:\n";
    for (size_t i = 0; i < saveFiles.size(); ++i) 
    {
        cout << i + 1 << ") " << saveFiles[i] << '\n';
    }

    int choice;
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice < 1 || choice > saveFiles.size()) 
    {
        cout << "Invalid choice.\n";
        return;
    }

    string saveFile = saveFiles[choice - 1];
    ifstream input_file ("saves/" + saveFile);
    game_state game;
    if (input_file ) 
    {
        cout << "Loading game from " << saveFile << "...\n";
        game.load("saves/" + saveFile);
        input_file .close();
    } 
    else 
    {
        cout << "Error opening save file.\n";
    }
    sleep(3);
    system("cls");
    load_level(game);
}

void exit_game() 
{
    cout << "Exiting the game...\n";
}

// shows menu in shop
void show_menu() 
{
    cout << "Welcome to the shop! Here are the items you can buy:\n";
    cout << "1. First Aid Kit - 10 gold\n";
    cout << "2. Water Bomb - 20 gold\n";
    cout << "3. Master Fire Extinguisher - 70 gold\n";
    cout << "4. Master Firefighter Suit - 70 gold\n";
    cout << "5. Exit\n";
}

// return corresponding value for string in map
int find_value(const map<string, int>& myMap, const string& key) 
{
   
    auto it = myMap.find(key);
    
    if (it != myMap.end()) 
    {
        return it->second;
    } 
    else 
    {
        
        return -1; 
    }
}

//  attack
void attack(game_state game, Enemy& target)
{
    map<string, int> weapons = {
        {"old fire extinguisher", 10},
        {"Arin's extinguisher", 20},
        {"rare fire extinguisher", 30},
        {"Master Fire Extinguisher", 50},
    };
    int dmg = find_value(weapons, game.weapon);
    target.take_damage(dmg);
}

// counts damage resistance
int armor_res(game_state& game)
{
    map<string, int> armors = {
        {"dirty firefighter suit", 2},
        {"Unordinary red firefighter suit", 5},
        {"Master Firefighter Suit", 15},
    };
    int res = find_value(armors, game.armor);
    return res;
}

// if we have bomb, delete one from vektor and deal dmg
void bomb(game_state& game, Enemy& target)
{
    auto it = find(game.inventory.begin(), game.inventory.end(), "water-bomb");
    
    if (it != game.inventory.end()) 
    {
        game.inventory.erase(it);
        target.take_damage(75);
    }
    else 
    {
        cout << "No water bomb in inventory. You lost your turn searching for one.\n";
    }
}

// first aid kit
void aid(game_state& game)
{
    auto it = find(game.inventory.begin(), game.inventory.end(), "aidkit");
    
    if (it != game.inventory.end()) 
    {
        game.inventory.erase(it);
        game.hp += 50;
    }
    else 
    {
        cout << "No aidkit in inventory. You lost your turn searching for one.\n";
    }
}

// message when players dies
void died()
{
    sleep(2);
    system("cls");
    cout << "You died";
    exit(0);
}

// menu for player turn
void player_turn(game_state& game, Enemy& target) 
{
    if (game.hp <= 0)
    {
        died();
    }
    int choice;
    cout << "It's your turn. Choose an action:\n";
    cout << "1. Attack with fire extinguisher\n";
    cout << "2. Throw water bomb\n";
    cout << "3. Use first aid kit\n";
    cout << "Enter your choice: ";
    cin >> choice;
    cout << "\n";
    
    switch (choice) {
        case 1:
            attack(game, target);
            break;
        case 2:
            bomb(game, target);
            break;
        case 3:
            aid(game);
            break;
        default:
            cout << "Invalid choice. You did nothing.\n";
            break;
    }
}

// Menu at the end of levels
void end_level(game_state& game) 
{
    int choice;
    do 
    {
        cout << "1. Continue\n";
        cout << "2. Save\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cout << "\n";

        switch (choice) 
        {
            case 1:
                switch (game.progress) 
                {
                    case 1:
                        level_1(game);
                        break;
                    case 2:
                        level_2(game);
                        break;
                    case 3:
                        level_3(game);
                        break;
                    case 4:
                        level_4(game);
                        break;
                    default:
                        cout << "Unknown level\n";
                        break;
                }
                break;
            case 2:
                game.save(game.name + ".txt");  // Ensure a unique filename for each save
                cout << "Game saved.\n";
                break;
            case 3:
                exit(0);
            default:
                cout << "Invalid choice. You did nothing.\n";
                break;
        }
    } while (choice != 3);
}

// ensures file exists
void ensure_exists() 
{
    _wmkdir(L"saves");
}

// level zero
void level_0(game_state game)
{
    system("cls");
    print_message(
        "Welcome to the bustling city of Pyroklas, a place renowned not only for its vibrant energy "
        "but also for the awe-inspiring, yet perilous, dragon sightings. Once a rare and mystical "
        "occurrence, these dragon encounters have become alarmingly frequent and increasingly destructive, "
        "setting the city ablaze and plunging its citizens into a state of perpetual fear.\n",
        100
    );

    print_message(
        "In response to this escalating crisis, the Fire Wing Brigade has sprung into action. This elite "
        "team of firefighters, known for their bravery and skill, has decided to bolster their ranks with "
        "fresh talent. Among the new recruits is you, an up-and-coming firefighter with a promising future. "
        "Your mission is clear: to protect Pyroklas from the fiery wrath of the dragons and uncover the dark "
        "motives behind their relentless attacks.\n"
    );
    sleep(3);
    system("cls");
    print_message(
        "\"Welcome, recruits, to the Fire Wing Brigade! I am Captain Arin, and I am here to prepare you for the challenges "
        "that lie ahead. Our mission is twofold: to battle fierce dragons and to extinguish the deadly fires they leave in their wake. "
        "Today marks the beginning of your journey as guardians of our city.\"\n"
    );

    print_message(
        "\"First, let's talk about your gear. Proper attire is crucial for your safety and effectiveness. "
        "You will be equipped with flame-resistant suits, essential for protecting you from the intense heat. Each of you will also "
        "carry a fire extinguisher for immediate action against smaller blazes, and water bombs - our primary weapon against both fires"
        "and dragons. Remember, courage is your most vital tool. Never forget why we do this: to protect our homes and our people.\n"
        "\"Now, take a moment to familiarize yourselves with your initial equipment.\"\n"
    );
    print_message("Received:\n dirty firefighter suit\n old fire extinguisher\n 1x water bomb\n");
    sleep(3);
    system("cls");

    print_message(
        "Suddenly, a shrill alarm pierces the air. Flames have erupted in the heart of the city! Panic spreads among the new"
        "recruits, but Captain Arin steps forward, his voice steady and reassuring.\n"
        "\"Stay calm, everyone. This is your first test. It's time to put your training into action. This is also your last chance"
        "to turn back.\"\n"
    );
    print_message(
        "You look around and notice that the rest of the recruits have fled in fear, leaving you alone. Captain Arin strides over,"
        "his expression a mix of disappointment and respect."
        "\"I expected as much,\" he says, his eyes locking onto the lone recruit. \"It takes true bravery to stand your ground in the face of fear. Tell me, what's your name?\"\n"
    );
    string player_name;
    cout << "Enter your name: ";
    cin >> player_name;   
    game.name = player_name;

    print_message(
        "Captain Arin nods approvingly as you introduce yourself. \"Well, " + game.name + ", it seems you have the heart of a true"
        "warrior. The city needs us now more than ever. Are you ready to take on this challenge and prove your worth as a member of th e"
        "Fire Wing Brigade?\"\n"
        "\"If so, follow me,\" he says with a determined glint in his eye. \"We're heading into action right now. Let's go save our city!\"\n"
    );
    sleep(3);
    system("cls");
    
    print_message(
        "As you arrive at the scene, you witness several buildings engulfed in flames, with a small red dragon perched upon the burning"
        "rubble. Captain Arin assesses the situation quickly.\n"
        "\"It's a young dragon,\" he declares, his voice steady despite the chaos around you. \"We should be able to handle this ."
        "Let's move in and take it down.\"\n"
        "Without hesitation, Captain Arin charges into battle, and you follow close behind. The heat of the flames intensifies as"
        "you draw nearer to the dragon, but your determination fuels your courage. This is your moment to prove yourself as a member "
        "of the Fire Wing Brigade.\n"
    );
    sleep(3);
    system("cls");

    Enemy young_dragon("Young Red Dragon", 150, 10, 30);
    int fire = 1;
    while (young_dragon.is_alive())
    {
        young_dragon.take_damage(30);
        cout << "Arin attack's dragon for 30 damage\n";
        if (!young_dragon.is_alive())
        {
            break;
        }
        player_turn(game, young_dragon);
        if (young_dragon.is_alive())
        {
            if (fire % 3 == 0)
            {
                young_dragon.attack2(game);
            }
            else
            {
                young_dragon.attack1(game);
            }
        }
        
        fire++;
        cout << "Your health: " << game.hp << endl;
    }
    sleep(2);
    system("cls");
    print_message("Dragon lies dead.\n");
    print_message(
        "\"The buildings are still burning,\" he says, surveying the inferno. \"We need to extinguish the flames and rescue "
        "anyone who might still be inside. Let's move quickly and work together to save as many lives as we can.\" "
        "\"" + game.name + " you and John will take the building on left. Let's meet here afterwards.\"\n"
    );
    sleep(2);
    system("cls");
    print_message(
        "\"I will take second floor\" said John. As you look around inside the burning building, you notice a faint noise coming from "
        "beyond a blazing doorway. It seems like someone might be trapped on the other side, but the intense flames make it a perilous approach. "
        "Going in may be dangerous. Is chance to save someone worth the risk?"
    );
    
    system("cls");

    int choice;
    cout << "Choose an action:\n";
    cout << "1. Go search for people and risk being hurt \n";
    cout << "2. Go back\n";
    cout << "Enter your choice: ";
    cin >> choice;
    cout << "\n";
    
    switch (choice) {
        case 1:
            game.hp -= 20;
            print_message(
                "You push through the fiery corridor, your heart pounding as you navigate the blazing wooden doors. Suddenly, a large "
                "beam crashes down on you.\n You've taken 20 damage.\n"
            );
            if (game.hp <= 0) 
            {
                died();
            }
             print_message(
                "Pain searing and nearly unbearable, but you grit your teeth and press on. In the corner, you spot an unconscious "
                "person. Summoning all your strength, you lift them and make your way back through the inferno. Barely escaping, you "
                "burst out of the collapsing building just as it comes crashing down behind you. Gasping for breath, you stagger "
                "towards the safety of your comrades. Captain Arin, John, and the rest of the squad are already there, standing near "
                "the smoldering corpse of the young dragon.\n"
            );

            print_message( 
                "Arin strides over to you, a mix of relief and pride in his eyes. \"Well done, I didn't expect fresh recruit to "
                "achieve so much in a single day\" he says, clapping a hand on your shoulder. \"You've shown incredible bravery today. " 
                "This is just the beginning of your journey with the Fire Wing Brigade. Let's get this person to safety. We'll talk about "
                "your next mission back in base"
            );      
            game.honor++;
            game.progress++;
            break;
        case 2:
            print_message( 
                "You decied to get out of the building to safety. As you wait for the rest of the crew to arrived at dragon corpse "
                "you see John leaving the building as well. Minute later building collapses. You're happy you left building in time. "
                "Arin strides over to you, a mix of relief and pride in his eyes. \"Well done, I didn't expect fresh recruit to defeat "
                "dragon in his first encounter.\" he says, clapping a hand on your shoulder. \"You've shown incredible bravery today. " 
                "This is just the beginning of your journey with the Fire Wing Brigade. We'll talk about "
                "your next mission back in base\n"
            );
            game.progress++;
            break;
        default:
            cout << "Invalid choice. You did nothing and when John came back you followed him to the outside.\n";
            print_message( 
                "As you wait for the rest of the crew to arrived at dragon corpse "
                "you see the building collapsing. You're happy you left building in time. "
                "Arin strides over to you, a mix of relief and pride in his eyes. \"Well done, I didn't expect fresh recruit to defeat "
                "dragon in his first encounter.\" he says, clapping a hand on your shoulder. \"You've shown incredible bravery today. " 
                "This is just the beginning of your journey with the Fire Wing Brigade. We'll talk about "
                "your next mission back in base.\n"
            );
            game.progress++;
            break;
    }
    end_level(game);
}

// level one
void level_1(game_state& game) 
{
    print_message(
        "Upon returning to the base, Captain Arin gathers the team and gives a quick debriefing. Afterward, he approaches you with a "
        "serious expression.\n"

        "\"" + game.name + " , you showed exceptional bravery today. Because of your performance, I have a special assignment for you,\""
        " he says. \"We need to find out why the dragons have become more aggressive recently. There's an informant on the outskirts of "
        "the city who might have some answers. Your mission is to locate this informant and gather any information they have on the "
        "dragons' unusual behavior.\"\n"
    );
    sleep(2);
    system("cls");
    print_message(
        "Captain Arin hands you a map with the informant's location marked. \"Be cautious. The outskirts can be dangerous, especially "
        "with the dragons on the move. But I have faith in your abilities. Good luck, and report back as soon as you can.\"\n"

        "With your new assignment in hand, you prepare to set out on this crucial mission, determined to uncover the mystery behind "
        "the dragons' sudden fury. As you walk out Captain Arin stops you. \"I've got present for you.\" he says as he hands you 2 "
        "water bombs, first aid kit and his own extinguisher. \"I reckon it will make better use in your hands.\" Then he says finally goodbye as you leave.\n"
    );
    print_message(
        "Recieved: \n water bombs x2 \n first aid kit \n Arin's extinguisher\n"
    );
    game.weapon = "Arin's extinguisher";
    game.inventory.emplace_back("water-bomb");
    game.inventory.emplace_back("water-bomb");
    game.inventory.emplace_back("aidkit");

    sleep(2);
    system("cls");


    print_message(
        "You leave the bustling center of Pyroklas behind, making your way towards the outskirts. The city gradually gives way to sparser, more rugged terrain. "
        "The path marked on the map leads you through dense forests and narrow, winding trails. After a few hours of trekking, you arrive at a small clearing where "
        "a lone figure stands by a campfire.\n"
    );
    sleep(2);
    system("cls");

    // Encounter with the stranger
    print_message(
        "The figure, a cloaked stranger, looks up as you approach. You see that he is injured, heavy bleeding from his right leg.\n"
        ""
    );

    print_message(
        "Stranger: Please, can you help me? I need a medicine to stop the bleeding.\n\n"
    );

    int choice;
    
    cout << "1. Use first aid kit to help him\n";
    cout << "2. Don't use aid kit on him\n";
    cout << "Enter your choice: ";
    cin >> choice;
    cout << "\n";
    auto it = game.inventory.end(); // Declare it outside the switch block

    switch (choice)
    {
        case 1:
            game.george = 1;
            it = find(game.inventory.begin(), game.inventory.end(), "aidkit"); // Initialize it here
            if (it != game.inventory.end()) 
            {
                game.inventory.erase(it);  
            } 
            print_message(
                "You give the stranger your medkit. He quickly tends to his wound, and the bleeding slows.\n"
                "Stranger: Thank you so much! My name is George. I promise that if we meet again, I will repay your kindness.\n"
            );
            game.honor++;
            break;
        case 2:
            print_message(
                "You decide not to help the stranger. He looks at you with a mix of disappointment and understanding.\n"
                "Stranger: I understand. Safe travels, stranger.\n"
            );
            game.honor--;
            break;
        default:
            break;
    }

    int choice1;
    print_message(
        "You continue on your journey and reach a fork in the road.\n"
        "Do you want to go:\n"
        "1. Right\n"
        "2. Left\n"
        "Enter your choice: "
    );
    cin >> choice1;
    cout << "\n";

    switch (choice1) {
        case 1:
            print_message(
                "You decide to go right. After a short walk, you come across a village. One of the houses is on fire.\n"
                "Villager: Help! Our house is burning! Can you assist us? My husband is inside!\n"
            );
            int choice2;
            print_message(
                "You continue on your journey and reach a fork in the road.\n"
                "Do you want to help them?\n"
                "1. Yes\n"
                "2. No\n"
                "Enter your choice: "
            );
            cin >> choice2;
            cout << "\n";
            switch (choice2)
            {
                case 1:
                    print_message("You went in and returned with man. Next you managed to extinguish the flames. \n"
                        "Villager: Thank you for saving my husband. We don't have much but take this for your trouble.\n\n"
                        "Received: 50 gold \n"
                    );
                    game.gold += 50;
                    game.honor++;
                    break;
                case 2:
                    print_message("You said that you can't stop as you have important mission to complete. As you are getting away you "
                        "hear cries and flames taking down wooden house. \n"
                    );
                    break;
                    game.honor--;
                default:
                    break;
            }
            break;
        case 2:
            print_message(
                "You decide to go left. After a short trek, you find yourself at the entrance of a cave.\n"
                "The cave looks ominous, but you feel a strange pull towards it.\n\n"
                "You found chest. Inside it you spotted red firefighter suit. It looks much better than yours. You put it on.\n "
                "Received: Unordinary red firefighter suit\n\n"
            );
            game.armor = "Unordinary red firefighter suit";
            break;
        default:
            print_message(
                "You stand at the fork, unsure of which path to take.\n"
            );
            break;
    }
    sleep(2);
    print_message(
        "You continue with your journey..."
    );
    sleep(2);
    game.progress++;
    end_level(game);
}

void level_2(game_state& game)
{
    print_message(
        "You finally reach your destination on the map and find three notes left behind by the informant.\n"
        "Notes provide discreet description of individual.\n"
        "You can see three people in the area."
        "It's time to identify the informant.\n"
    );

    // Note 1
    print_message(
        "Note 1: This person often wears a mischievous grin and has a tendency to crack jokes, with a playful glint in their piercing blue eyes.\n"
    );

    // Note 2
    print_message(
        "Note 2: The person like reading books.\n"
    );

    // Note 3
    print_message(
        "Note 3: The person usually wears black jacket.\n"
    );
    cout << "\n";
    string secret_password = "fire";

    print_message(
        "You approach the first person.\n"
        "James: Hey, what's up?\n"
        ""+game.name+": Not much, just busy with work. How about you?\n"
        "James: I'm thinking about good jokes, you wanna hear one?\n"
        ""+game.name+": Sure bring it on!\n"
        "James: Why couldn't the bicycle stand up by itself? Because it was two-tired!\n"
        ""+game.name+": Haha, that's a good one!\n"
        "James: Thanks! Anyway, I'll leave you for now, bye.\n\n"
    );

    print_message(
        "You approach the second person.'\n"
        ""+game.name+": Hey, What's up?\n"
        "Sarah: I'm Sarah. Not much, just enjoying the sunshine. How about you?\n"
        ""+game.name+": Same here, thinking of going for a walk later.\n"
        "Sarah: By the way, I just finished reading this hilarious book. Have you heard of it?\n"
        ""+game.name+": Oh really? Tell me more!\n"
        "Sarah: It's called 'The Mysterious Case of the Missing Socks'. The detective is so clueless, it's a riot!\n"
        ""+game.name+": Sounds like a fun read. Nice jacket where did you buy it?\n"
        "Sarah: Oh, the jacket I'm wearing? It's black leather only found in the South\n"
        ""+game.name+": Yeah, it looks great on you!\n"
        "Sarah: Thanks!\n"
        ""+game.name+": I'll leave you for now, bye.\n\n"
    );

    print_message(
        "You approach the third person.\n"
        "Saul: Hi, I heard you enjoy reading books.\n"
        ""+game.name+": Yes, I love reading. Do you like books too?\n"
        "Saul: Oh, absolutely! They're my passion! Recently, I came across a fascinating novel about time travel.\n"
        ""+game.name+": That sounds intriguing! What was the book?\n"
        "Saul: It's called 'Through Six Centuries'. It's an incredible tale of journeying through different historical epochs.\n"
        ""+game.name+": It sounds very interesting. Would you recommend it?\n"
        "Saul: Absolutely!\n"
        ""+game.name+": Nice jacket by the way.\n"
        "Saul: Thanks it's one of the kind, I love its' red color\n"
        ""+game.name+": See you later Mr. Goodman.\n\n"
    );

    int choice;
    bool correct_guess = false;

    while (!correct_guess) {
        print_message("Choose who you think is the informant:");
        print_message("1. James");
        print_message("2. Sarah");
        print_message("3. Saul");
        cin >> choice;
        string input_password;

        switch (choice) {
            case 1:
                print_message("You chose the wrong person. Try again.\n");
                break;
            case 2:
                while (input_password != secret_password)
                {
                    print_message(
                        "You approached Sarah and said that Capitan Arin needs informations about dragons. \n"
                        "She then starts to tell you a riddle: \n"
                        );
                    print_message("I'm not alive, but I can grow; I don't have lungs, but I need air; I don't have a mouth, but water kills me. What am I?\n");
                    print_message("Enter the special password:");
                    cin.ignore(); 
                    getline(cin, input_password);
                    if (input_password == secret_password) 
                    {
                        cout << "\n";
                        print_message(
                            "She said, \"Sorry for the caution but you know. So what I gathered says that some huge dragon called 'Lord of Flame'. \n"
                            "makes other dragons attack more often, we need to take him down to bring back peace.\""
                        );
                        correct_guess = true;
                    } 
                    else 
                    {
                        print_message("Incorrect password. Try again.\n");
                    }
                }
                break;
            case 3:
                print_message("You chose the wrong person. Try again.\n");
                break;
            default:
                print_message("Invalid choice. Please select again.\n");
                break;
        }
    }
    sleep(2);
    game.progress++;
    end_level(game);
}

void level_3(game_state& game)
{
    if (game.george == 1)
    {
        print_message( 
            "While returning from the mission, you came across George, the man you once saved with a first aid kit.\n"
            "Grateful for your help, George offers to show you around his village.\n"
            "In addition to his hospitality, he presents you with a rare fire extinguisher passed down from his grandfather, and 50 gold coins.\n"
            "Now equipped with the rare fire extinguisher and some extra gold, you feel ready to tackle any challenge that comes your way.\n"
            "\nReceived: \n50 gold \n rare fire extinguisher\n"
        );
        game.weapon = "rare fire extinguisher";
    }

    print_message(
        "Suddenly, as you arrive in the city, a dragon swoops down from the sky. It speaks to you, declaring that in the name "
        "of the Lord of Flame, it has been commanded to end your life.\n"
    );

    sleep(3);
    system("cls");

    Enemy dragon("Green Dragon", 200, 15, 25);
    int fire = 1;
    while (dragon.is_alive())
    {
        player_turn(game, dragon);
        if (dragon.is_alive())
        {
            if (fire % 3 == 0)
            {
                dragon.attack2(game);
            }
            else
            {
                dragon.attack1(game);
            }
        }
        
        fire++;
        cout << "Your health: " << game.hp << endl;
    }
    sleep(2);
    system("cls");
    print_message("Dragon lies dead.\n");
    print_message(
        "As the dragon falls you see that it carried some gold!\n"
        "Received:\n 40 gold\n"
        );
    game.gold += 40;
    sleep(2);
    system("cls");

    print_message(
        "On your way back to the base, you come across a bustling trading post. The merchant greets you warmly and shows you his "
        "wares. The shop is filled with various goods, including healing supplies, weapons, and rare items. It's a perfect opportunity "
        "to stock up for your upcoming battles. "
        "You decide to check out the items available and prepare yourself for the challenges ahead.\n"
    );

    int choice;
    bool running = true;

    while (running) 
    {
        show_menu();
        cout << "You have " << game.gold << " gold.\n";
        cout << "Enter the number of the item you wish to buy: ";
        cin >> choice;

        switch (choice) 
        {
            case 1:
                if (game.gold >= 10) 
                {
                    game.gold -= 10;
                    cout << "You bought a First Aid Kit.\n";
                    game.inventory.emplace_back("aidkit");
                } 
                else 
                {
                    cout << "Not enough gold.\n";
                }
                break;
            case 2:
                if (game.gold >= 20) 
                {
                    game.gold -= 20;
                    cout << "You bought a Water Bomb.\n";
                    game.inventory.emplace_back("water-bomb");
                } 
                else 
                {
                    cout << "Not enough gold.\n";
                }
                break;
            case 3:
                if (game.gold >= 70) 
                {
                    game.gold -= 70;
                    cout << "You bought a Master Fire Extinguisher.\n";
                    game.weapon = "Master Fire Extinguisher";
                } 
                else 
                {
                    cout << "Not enough gold.\n";
                }
                break;
            case 4:
                if (game.gold >= 70) 
                {
                    game.gold -= 70;
                    cout << "You bought a Master Firefighter Suit.\n";
                    game.armor = "Master Firefighter Suit";
                } 
                else 
                {
                    cout << "Not enough gold.\n";
                }
                break;
            case 5:
                running = false;
                cout << "Exiting the shop. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
    print_message(
        "On your way back to the base, you feel the weight of the recent events bearing down on you. The encounter with the dragon has left you with a sense of urgency and purpose "
        "The path is rough and your thoughts are consumed by the dragon's ominous words. As you approach the base, the familiar sight of the fortress brings a slight sense of relief, but you know your mission is far from over. \n"
        "Upon reaching the base, you head straight to Captain Arin's quarters to report what happened. "
        "\"Captain, we have a serious problem,\" you say, catching your breath. \"A dragon attacked us in the city. It spoke, saying it was sent by the Lord of Flames to kill me. \"\n"
        "\"The Lord of Flames,\" Captain Arin mutters, his expression darkening. \"This is worse than I thought.\"\n\n"
        "\"We need to take down this Lord of Flames,\" you insist. \"If we don't, the attacks will only get worse.\"\n\n"
        "Captain Arin nods, a look of determination in his eyes. \"You're right. We'll need to gather our best warriors and prepare for a direct assault. This ends now. \""
        "\"I'll be ready,\\\" you say, feeling a surge of resolve. \"Let's finish this.\"\n"
    );

    sleep(2);
    system("cls");

    print_message(
        "\"Before we'll move to dragons spire, you should have some rest, we'll prepre for battle.\"\n\n"
        "You take nap:\n health has been restored to full"
    );
    game.hp = 100;
    game.progress++;
    end_level(game);
}

void level_4(game_state& game)
{
    print_message(
        "The journey to the Spire of Dragons is long and arduous. The terrain grows increasingly treacherous as you and your company "
        "ascend towards the looming peak. Every step is a reminder of the weight of your mission. The air grows hotter, and "
        "the ground beneath your feet trembles with the distant roars of dragons.\n\n"
        "At last, you reach the base of the spire. The entrance looms before you, a dark maw set into the mountainside, guarded by "
        "ancient carvings of dragonkind. Captain Arin stands before the assembled warriors, his eyes fierce with determination."
        "He raises his voice, calling everyone to attention. \n\n"
        "\"Brothers and sisters, today we face our greatest challenge,\" he begins. \"The Lord of Flames has threatened our homes, "
        "our families, and our very lives. But we are the Fire Wing Brigade! We have trained for this moment, "
        "to stand against the inferno and emerge victorious!\"\n"
        "\"Remember why we fight! We fight for those who cannot defend themselves. We fight to protect our lands from the terror of "
        "the skies. We fight for each other, for our unity and our strength!\"\n"
        "\"Now, as we stand before the lair of our enemy, let our courage burn brighter than the fiercest flame. Together, we will "
        "bring down the Lord of Flames and restore peace to our world! \"\n\n"
        "The company roars in response, their spirits lifted by Captain Arin's words. With weapons ready and hearts steeled, you prepare to enter the Spire of Dragons and face your destiny."
    );
    sleep(2);
    system("cls");

    print_message(
        "As you and your company advance towards the Spire of Dragons, several dragons suddenly block your path. Their eyes burn with malice, and their scales glisten in the fiery light. "
        "Captain Arin surveys the scene quickly, then turns to you. \n\n"
        "\"Take the one on the left!\" he orders, his voice steady and commanding. \"We'll handle the others.\"\n"
        "With a nod, you ready your weapon and prepare to face the dragon. The battle is about to begin, and the fate of your mission rests on your shoulders. "
    );

    sleep(3);
    system("cls");

    Enemy dragon("Blue Dragon", 250, 20, 30);
    int fire = 1;
    while (dragon.is_alive())
    {
        player_turn(game, dragon);
        if (dragon.is_alive())
        {
            if (fire % 3 == 0)
            {
                dragon.attack2(game);
            }
            else
            {
                dragon.attack1(game);
            }
        }
        
        fire++;
        cout << "Your health: " << game.hp << endl;
    }
    sleep(2);
    system("cls");
    print_message("Dragon lies dead.\n\n");

    print_message(
        "After a fierce battle, you manage to defeat the dragon on the left. Just as you catch your breath, you hear Captain Arin's voice calling out to you. \n\n"
        "\"Well done! Now, go after the Lord of Flames!\" he shouts, his voice cutting through the chaos. \"We'll hold off the rest of these beasts!\"\n"
        "With renewed determination, you turn towards the heart of the spire, ready to face the ultimate challenge. The fate of your world rests on this final confrontation. \n"
    );
    sleep(2);
    system("cls");

    print_message(
        "You climb the stone stairs, each step echoing with the weight of destiny. The air grows hotter, and the distant roars of dragons fill your ears. At last, you stand before a pair of massive doors, their surface covered in ancient runes and carvings of dragonkind.\n"
        "These doors mark the entrance to the heart of the spire, where the Lord of Flames awaits.\n"
        "\"This is it,\" you think to yourself, steeling your resolve. \"Time to end this once and for all.\"\n"
    );
    if (game.honor > 0)
    {
        if (game.george == 1)
        {
            if (game.george == 1)
            {
                print_message(
                    "On the left side in corridor, you see familiar face - it's George!\n"
                    "After him other more faces, citizens, villagers, Jimmy, Saul, Sarah, they are all here to help. \n"
                    "\"We've heard you needed some help, ha! Let's take down this giant lizard together! \n"
                );
            }
            print_message(
                "With a deep breath, you push the doors open and step into the chamber beyond, ready to face your destiny. \n"
                "The hardest battle of your life is about to take begin. \n"
            );
            sleep(2);
            system("cls");
            Enemy lord_of_flames("Lord of Flames", 400, 30, 50);
            int fire = 1;
            while (lord_of_flames.is_alive())
            {
                if (game.george == 1)
                {
                    lord_of_flames.take_damage(30);
                    cout << "George attack's dragon for 30 damage\n";
                }
                lord_of_flames.take_damage(30);
                cout << "People of Pyroklas attack dragon for 30 damage\n";
                player_turn(game, lord_of_flames);
                if (lord_of_flames.is_alive())
                {
                    if (fire % 3 == 0)
                    {
                        lord_of_flames.attack2(game);
                    }
                    else
                    {
                        lord_of_flames.attack1(game);
                    }
                }
                
                fire++;
                cout << "Your health: " << game.hp << endl;
            }
            sleep(2);
            system("cls");
            print_message("Lord of Flames lies dead.\n\n");

            print_message(
                "The battle with the Lord of Flames was fierce and exhausting. The air was filled with the roar of fire and the clash of steel against scales. But in the end, your determination and skill proved stronger.\n"
                "As the Lord of Flames falls, the ground shakes and the flames that once filled the chamber begin to fade. The oppressive heat starts to dissipate, and a calm, cool breeze sweeps through the room.\n"
                "You stand over the fallen dragon, catching your breath and taking in the magnitude of your victory. The Lord of Flames, the terror that threatened your world, is no more.\n"
                "\"It's over,\" you think to yourself, a mixture of relief and triumph flooding your mind. \"We've done it.\"\n"
                "Captain Arin and the rest of the company rush into the chamber, their faces a mix of concern and hope. When they see the lifeless form of the Lord of Flames, their expressions change to ones of joy and pride.\n"
                "\"Well done,\" Captain Arin says, his voice filled with admiration. \"You've saved us all\"\n"
                "The company gathers around you, offering their congratulations and thanks. Together, you make your way back down the spire, the weight of your victory lifting your spirits.\n"
                "\"The world is safe once more, thanks to your bravery and the strength of the Fire Wing Brigade.\"\n"
                "\"This is just the beginning,\" you think to yourself, ready for whatever challenges the future may bring.\n"
                "Victory: The Lord of Flames has been defeated, and peace has been restored.\n"
            );  
        }
        else
        {
            print_message(
                    "On the left side in corridor, you see familiar faces - People of Pyroklas!\n"
                    "Citizens, villagers, Jimmy, Saul, Sarah, they are all here to help. \n"
                    "\"We've heard you needed some help. Fire Brigade has saved as so many times. Now its time we show our graditiude\n"
                );

            print_message(
                "With a deep breath, you push the doors open and step into the chamber beyond, ready to face your destiny. \n"
                "The hardest battle of your life is about to take begin. \n"
            );
            sleep(2);
            system("cls");
            Enemy lord_of_flames("Lord of Flames", 400, 30, 50);
            int fire = 1;
            while (lord_of_flames.is_alive())
            {
                if (game.george == 1)
                {
                    lord_of_flames.take_damage(30);
                    cout << "George attack's dragon for 30 damage\n";
                }
                lord_of_flames.take_damage(30);
                cout << "People of Pyroklas attack dragon for 30 damage\n";
                player_turn(game, lord_of_flames);
                if (lord_of_flames.is_alive())
                {
                    if (fire % 3 == 0)
                    {
                        lord_of_flames.attack2(game);
                    }
                    else
                    {
                        lord_of_flames.attack1(game);
                    }
                }
                
                fire++;
                cout << "Your health: " << game.hp << endl;
            }
            sleep(2);
            system("cls");
            print_message("Lord of Flames lies dead.\n\n");

            print_message(
                "The battle with the Lord of Flames was fierce and exhausting. The air was filled with the roar of fire and the clash of steel against scales. But in the end, your determination and skill proved stronger.\n"
                "As the Lord of Flames falls, the ground shakes and the flames that once filled the chamber begin to fade. The oppressive heat starts to dissipate, and a calm, cool breeze sweeps through the room.\n"
                "You stand over the fallen dragon, catching your breath and taking in the magnitude of your victory. The Lord of Flames, the terror that threatened your world, is no more.\n"
                "\"It's over,\" you think to yourself, a mixture of relief and triumph flooding your mind. \"We've done it.\"\n"
                "Captain Arin and the rest of the company rush into the chamber, their faces a mix of concern and hope. When they see the lifeless form of the Lord of Flames, their expressions change to ones of joy and pride.\n"
                "\"Well done,\" Captain Arin says, his voice filled with admiration. \"You've saved us all\"\n"
                "The company gathers around you, offering their congratulations and thanks. Together, you make your way back down the spire, the weight of your victory lifting your spirits.\n"
                "\"The world is safe once more, thanks to your bravery and the strength of the Fire Wing Brigade.\"\n"
                "\"This is just the beginning,\" you think to yourself, ready for whatever challenges the future may bring.\n"
                "Victory: The Lord of Flames has been defeated, and peace has been restored.\n"
            );
        }
    }
    else if (game.honor == 0)
    {
        if (game.george == 1)
        {
            print_message(
                "On the left side in corridor, you see familiar face - it's George!\n"
                "\"I've heard you needed some help, ha! Let's take down this giant lizard together! \n"
            );
        }
        print_message(
            "With a deep breath, you push the doors open and step into the chamber beyond, ready to face your destiny. \n"
            "The hardest battle of your life is about to take begin. \n"
        );
        sleep(2);
        system("cls");
        Enemy lord_of_flames("Lord of Flames", 400, 30, 50);
        int fire = 1;
        while (lord_of_flames.is_alive())
        {
            if (game.george == 1)
            {
                lord_of_flames.take_damage(30);
                cout << "George attack's dragon for 30 damage\n";
            }
            player_turn(game, lord_of_flames);
            if (lord_of_flames.is_alive())
            {
                if (fire % 3 == 0)
                {
                    lord_of_flames.attack2(game);
                }
                else
                {
                    lord_of_flames.attack1(game);
                }
            }
            
            fire++;
            cout << "Your health: " << game.hp << endl;
        }
        sleep(2);
        system("cls");
        print_message("Lord of Flames lies dead.\n\n");

        print_message(
            "The battle with the Lord of Flames was fierce and exhausting. The air was filled with the roar of fire and the clash of steel against scales. But in the end, your determination and skill proved stronger.\n"
            "As the Lord of Flames falls, the ground shakes and the flames that once filled the chamber begin to fade. The oppressive heat starts to dissipate, and a calm, cool breeze sweeps through the room.\n"
            "You stand over the fallen dragon, catching your breath and taking in the magnitude of your victory. The Lord of Flames, the terror that threatened your world, is no more.\n"
            "\"It's over,\" you think to yourself, a mixture of relief and triumph flooding your mind. \"We've done it.\"\n"
            "Captain Arin and the rest of the company rush into the chamber, their faces a mix of concern and hope. When they see the lifeless form of the Lord of Flames, their expressions change to ones of joy and pride.\n"
            "\"Well done,\" Captain Arin says, his voice filled with admiration. \"You've saved us all\"\n"
            "The company gathers around you, offering their congratulations and thanks. Together, you make your way back down the spire, the weight of your victory lifting your spirits.\n"
            "\"The world is safe once more, thanks to your bravery and the strength of the Fire Wing Brigade.\"\n"
            "\"This is just the beginning,\" you think to yourself, ready for whatever challenges the future may bring.\n"
            "Victory: The Lord of Flames has been defeated, and peace has been restored.\n"
        );   
    }
    else
    {
        print_message(
            "With a deep breath, you push the doors open and step into the chamber beyond, ready to face your destiny. \n"
            "As you enter the chamber of Lord of Flames you wonder whether you could do more on your journey, perhaps help more people?\n"
            "But these thoughs must be gone as hardest battle of your life is about to take place\n"
        );

        Enemy lord_of_flames("Lord of Flames", 400, 30, 50);
        int fire = 1;
        while (lord_of_flames.is_alive())
        {
            player_turn(game, lord_of_flames);
            if (lord_of_flames.is_alive())
            {
                if (fire % 3 == 0)
                {
                    lord_of_flames.attack2(game);
                }
                else
                {
                    lord_of_flames.attack1(game);
                }
            }
            
            fire++;
            cout << "Your health: " << game.hp << endl;
        }
        sleep(2);
        system("cls");
        print_message("Lord of Flames lies dead.\n\n");

        print_message(
            "The battle with the Lord of Flames was fierce and exhausting. The air was filled with the roar of fire and the clash of steel against scales. But in the end, your determination and skill proved stronger.\n"
            "As the Lord of Flames falls, the ground shakes and the flames that once filled the chamber begin to fade. The oppressive heat starts to dissipate, and a calm, cool breeze sweeps through the room.\n"
            "You stand over the fallen dragon, catching your breath and taking in the magnitude of your victory. The Lord of Flames, the terror that threatened your world, is no more.\n"
            "\"It's over,\" you think to yourself, a mixture of relief and triumph flooding your mind. \"We've done it.\"\n"
            "Captain Arin and the rest of the company rush into the chamber, their faces a mix of concern and hope. When they see the lifeless form of the Lord of Flames, their expressions change to ones of joy and pride.\n"
            "\"Well done,\" Captain Arin says, his voice filled with admiration. \"You've saved us all\"\n"
            "The company gathers around you, offering their congratulations and thanks. Together, you make your way back down the spire, the weight of your victory lifting your spirits.\n"
            "\"The world is safe once more, thanks to your bravery and the strength of the Fire Wing Brigade.\"\n"
            "\"This is just the beginning,\" you think to yourself, ready for whatever challenges the future may bring.\n"
            "Victory: The Lord of Flames has been defeated, and peace has been restored.\n"
        );
    }
    sleep(3);
    system("cls");
    print_message("The End\n");
    print_message("Thanks for playing");
    sleep(600);
    system("cls");
}
    

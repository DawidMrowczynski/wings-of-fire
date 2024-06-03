#include <iostream>
#include "helpers.h"

using namespace std;

int main() 
{
    game_state game;
    game_state load;
    ensure_exists();
    
    int choice = 0;
    
    while (choice != 3) 
    {
        cout << "1. New Game\n";
        cout << "2. Load Game\n";
        cout << "3. Exit Game\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cout << "\n";
        
        switch (choice) 
        {
            case 1:
                game = new_game();
                level_0(game);
                break;
            case 2:
                load_game();
                break;
            case 3:
                exit_game();
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
        
    }
    return 0;
}
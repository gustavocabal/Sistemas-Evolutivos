#include <iomanip>
#include <iostream>
#include <time.h>
#include <cmath>
#include <vector>
#include <random>

using namespace std;

#define map_lines 10
#define map_coluns 10
#define size_pop 20
#define gens 10
#define mutatation 0.01

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dist_weight(1, 100);

vector<int> population;
vector<float> grade;
vector<vector<int>> map;

class Frog_pop {
    public:
        vector<int> movement;
        int position;

        //Contructor
        Frog_pop(vector<int> mov, int pos) {
            movement = mov;
            position = pos;
        }

        void change_position() {
            discrete_distribution<> dist_choice(movement.begin(), movement.end());
            int chosen_movement = dist_choice(gen);     // should return a number between 0 and 7 taking their weights in to account

            switch (chosen_movement) {
                case 0: position += 4; break;
                case 1: position += 3; break;
                case 2: position += 2; break;
                case 3: position += 1; break;
                case 4: position -= 1; break;
                case 5: position -= 2; break;
                case 6: position -= 3; break;
                case 7: position -= 4; break;
            }
        }

};

void create_map(vector<vector<int>> &map) {
    int num = 1;
    vector<int> linha;

    for (int i = 0; i < map_lines; i++) {
        for (int j = 0; j < map_coluns; j++) {
            linha.push_back(num);
            num++;
        }
        map.push_back(linha);
        linha.clear();
    }
}

void see_map(vector<vector<int>> map) {
    int n = 4;
    for (int z = 0; z < n * 3/2 * map_coluns; z++) { cout << "-"; }
    cout << endl;
    for (int i = 0; i < map_lines; i++) {
        cout << "|";
        for (int j = 0; j < map_coluns; j++) {
            cout << setw(n) << map[i][j] << " |";
        }
        cout << endl;
        for (int k = 0; k < n * 3/2 * map_coluns; k++) { cout << "-"; }
        cout << endl;
    }
}

vector<int> create_mov() {
    vector<int> mov;
    for (int i = 0; i < 8; i++) {    
        mov.push_back(dist_weight(gen));
        cout << mov[i] << " ";
        }
    cout << endl;
    return mov;
}



int main() {
    create_map(map);
    //see_map(map);
    Frog_pop frogie1 (create_mov(),50);
    frogie1.change_position();
    cout << "New position: " << frogie1.position << endl;
    return 0;
}
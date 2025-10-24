#include <iostream>
#include <ctime>
#include <random>

//using namespace std;

int main() {
    // Tamanho do mapa
    int lin = 30;
    int col = 20;
    char mapa[lin][col];

    // Sementeando a engine de forma moderna
    std::random_device rd;                             // tenta obter entropia do SO
    std::mt19937 gen(rd());                            // engine
    std::uniform_int_distribution<int> dist(0, 19);     // intervalo INCLUSIVO [0,10]

    for(int i = 0; i < lin; i++) {
        for(int j = 0; j < col; j++) {
            int entidade = dist(gen);
            switch(entidade) {
                case 0:
                    mapa[i][j] = 'H';
                    break;
                case 1:
                    mapa[i][j] = 'S';
                    break;
                case 2:
                    mapa[i][j] = 'B';
                    break;
                case 3:
                    mapa[i][j] = 'M';
                    break;
                default:
                    mapa[i][j] = ' ';
                    break;
            }
        }
    }


    for (int i = 0; i < lin; ++i) {
        for (int j = 0; j < col; ++j) {
            std::cout << mapa[i][j];
        }
        std::cout << '\n';
    }

    return 0;
}
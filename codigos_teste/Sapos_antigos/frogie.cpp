#include <iomanip>
#include <iostream>
#include <time.h>
#include <cmath>
#include <vector>
#include <random>

using namespace std;

#define map_lines 8
#define map_coluns 8
#define size_pop 1
#define gens 10
#define leaps 5
#define mutatation 0.01

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dist_weight(1, 100);
uniform_int_distribution<int> dist(0, 5);

vector<float> grade;
vector<vector<int>> numerical_map;
vector<vector<char>> visual_map;
int inicial_position = (10*map_lines/2 + map_coluns/2);
vector<int> bombs;
vector<int> flies;
vector<int> dead;
int special_frog[2] = {int(inicial_position/map_coluns), inicial_position%map_coluns};
vector<float> score;
int MELHOR_DE_TODOS;

class Frog {
    public:
        vector<int> movement;
        int position;
        int orientacao; // Norte, sul, leste, oeste
        int moscas = 0;
        int buracos = 0;
        int bombas = 0;

        //Construtor
        Frog(vector<int> mov, int pos) {
            movement = mov;
            position = pos;
        }

        void change_position() {
            discrete_distribution<> dist_choice(movement.begin(), movement.end());
            int chosen_movement = dist_choice(gen);     // retorna um numero entre 0 e 7 levando seus pesos em consideração

            switch (chosen_movement) {
                case 0: position += (map_coluns - 1); break;
                case 1: position += map_coluns; break;
                case 2: position += (map_coluns + 1); break;
                case 3: position -= 1; break;
                case 4: position += 1; break;
                case 5: position -= (map_coluns - 1); break;
                case 6: position -= (map_coluns); break;
                case 7: position -= (map_coluns + 1); break;
            }
        }

        // operador de impressão
        friend ostream& operator<<(ostream& os, const Frog& f) {
            os << "Position: " << f.position << "\n Movements: ";
            for (int m : f.movement) os << m << " ";
            os << endl;
            return os;
        }
};

vector<Frog> population;

void create_num_map() {
    // escolha se a matriz comeca em 0 ou em outro numero
    int num = 0;
    vector<int> line;

    for (int i = 0; i < map_lines; i++) {
        for (int j = 0; j < map_coluns; j++) {
            line.push_back(num);
            num++;
        }
        numerical_map.push_back(line);
        line.clear();
    }
}

void create_active_map() {
    visual_map.assign(map_lines, vector<char>(map_coluns, ' '));
    int num = 0;

    for(int i = 0; i < map_lines; i++) {
        for(int j = 0; j < map_coluns; j++) {
            int entidade = dist(gen);
            if(num == inicial_position) {visual_map[i][j]='S';}
            else {
                switch(entidade) {
                    case 0:
                        visual_map[i][j] = 'F'; //mosca
                        flies.push_back(map_lines*i + j);
                        break;
                    case 1:
                        visual_map[i][j] = 'B'; //bomba
                        bombs.push_back(map_lines*i + j);
                        break;
                    default:
                        visual_map[i][j] = ' '; //nada
                        break;
                }
            }
            num++;
        }
    }
}

void follow_frog(Frog idol)  {
    int j = idol.position%map_coluns;
    int i = int(idol.position/map_coluns);

    // Comentar a linha abaixo para apagar (ou nao) o sapo a cada movimento
    visual_map[special_frog[0]][special_frog[1]] = ' ';

    visual_map[i][j] = 'S';
    special_frog[0] = i;
    special_frog[1] = j;
}

void see_map() {
    int n = 4;
    for (int z = 0; z < n * 3/2 * map_coluns; z++) { cout << "-"; }
    cout << endl;
    for (int i = 0; i < map_lines; i++) {
        cout << "|";
        for (int j = 0; j < map_coluns; j++) {
            cout << setw(n) << visual_map[i][j] << " |";
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
        //cout << mov[i] << " ";
        }
    //cout << endl;
    return mov;

/*
    Ordem de movimento)
        {5, 6, 7,
         3, _, 4,
         0, 1, 2}
*/


}

vector<Frog> create_pop() {
    vector<Frog> pop;
    for (int i = 0; i < size_pop; i++) {
        Frog frogie (create_mov(),inicial_position);
        pop.push_back(frogie);
    }
    return pop;
}

/*void visao(int i) {
    switch (population[i].orientacao) {
    case 0: // Cima
        int CE = population[i].position -(map_coluns - 1);
        if() {

        }
        break;
    case 1: // Direita
        
        break;
    case 2: // Baixo
        
        break;
    case 3: // Esquerda

        break;
    
    default:
        break;
    }
}*/

void moving() {
    vector<long unsigned int> death;
    bool skip;
    for (int z = 0; z < leaps; z++) {
        for (long unsigned int i = 0; i < population.size(); i++) {
            skip = false;
            for (long unsigned int w = 0; w < death.size(); w++) {if (death.at(w) == i) {skip = true;}}
            if (skip == true) {continue;}
            population.at(i).change_position();
            for (long unsigned int j = 0; j < bombs.size(); j++) {
                if (population.at(i).position == bombs.at(j)) {
                    death.push_back(i);
                }
            }
            cout << population[i];
            follow_frog(population[i]);
            see_map();
        }
    }
    /* 
    Cada sapo se move uma quantidade "leaps" de vezes
    Se a posição que o sapo parou tiver uma bomba, ele não se mexe mais
    -- o vetor "death" guarda o index dos sapos que encontraram bombas
    */
}

void see_pop(vector<Frog> pop) {
    for (int i = 0; i < size_pop; i++) {
        cout << pop[i];
    }
}

void avaliarsapos(const int saltos) {
    float a = 2; //Peso das moscas
    float b = 0.7; //Peso ciclos-buracos

    for(int i=0; i<population.size(); i++) {
        float nota = 0;
        nota = a*population[i].moscas + b*(saltos-population[i].buracos)*(1-population[i].bombas);
        score.push_back(nota);
        if(i == 0) {
            MELHOR_DE_TODOS = i;
        }
        else {
            if (score[i] > score[MELHOR_DE_TODOS]) {
                MELHOR_DE_TODOS = i;
            }
        }
    }
}

int main() {
    create_num_map();
    create_active_map();
    see_map();
    cout << "Bombs: ";
    for (long unsigned int i = 0; i < bombs.size(); i++) {cout << bombs.at(i) << " ";}
    cout << endl;
    population = create_pop();
    see_pop(population);

    int saltos = 0;      
    int geracao = 0;  
    do {
        if(saltos % 5 == 0) {
            avaliarsapos(saltos);
            cout << MELHOR_DE_TODOS << endl;
            geracao++;
        }

        moving();
        saltos++;
    } while(true);

    return 0;
}
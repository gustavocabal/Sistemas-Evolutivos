#include <iomanip>
#include <iostream>
#include <time.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

#define map_lines 8
#define map_coluns 8
#define size_pop 1
#define gens 10
#define leaps 5
#define mutation 0.01

// RNG
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dist_weight(1, 100);
uniform_int_distribution<int> dist_entity(0, 19);

struct Coord {
    int x;
    int y;
};

vector<vector<char>> visual_map;
vector<Coord> bombas;
vector<Coord> moscas;
vector<Coord> valas;
vector<float> score;
int MELHOR_DE_TODOS = 0;

Coord pos_inicial = { map_lines/2, map_coluns/2 };
Coord pos_sapo_visual = pos_inicial;

// --------------------------------------------
// CLASSE SAPO
// --------------------------------------------
class Frog {
public:
    vector<int> movement;
    Coord pos;
    int orientacao;

    int qtd_moscas = 0;
    int qtd_valas = 0;
    int qtd_bombas = 0;

    Frog(vector<int> mov, Coord p) {
        movement = mov;
        pos = p;
        orientacao = 0;
    }

    void change_position() {
        discrete_distribution<> dist_choice(movement.begin(), movement.end());
        int mv = dist_choice(gen);
        orientacao = mv;

        // Movimento (dx,dy):
        static const int dx[8] = {+1,+1,+1, 0, 0,-1,-1,-1};
        static const int dy[8] = {-1, 0,+1,-1,+1,-1, 0,+1};

        int nx = pos.x + dx[mv];
        int ny = pos.y + dy[mv];

        // Continua no mapa
        if (nx >= 0 && nx < map_lines && ny >= 0 && ny < map_coluns) {
            pos.x = nx;
            pos.y = ny;
        }
    }

    friend ostream& operator<<(ostream& os, const Frog& f) {
        os << "Pos: (" << f.pos.x << "," << f.pos.y << ")  Movs: ";
        for (auto w : f.movement) os << w << " ";
        os << endl;
        return os;
    }
};

vector<Frog> population;

// --------------------------------------------
// FUNÇÕES DO MAPA
// --------------------------------------------

void create_active_map() {
    visual_map.assign(map_lines, vector<char>(map_coluns, ' '));
    bombas.clear();
    moscas.clear();
    valas.clear();

    for (int i = 0; i < map_lines; i++) {
        for (int j = 0; j < map_coluns; j++) {

            if (i == pos_inicial.x && j == pos_inicial.y) {
                visual_map[i][j] = 'S';
                continue;
            }

            int e = dist_entity(gen);

            if (e == 0) { // 5% chance
                visual_map[i][j] = 'M';
                moscas.push_back({i,j});
            }
            else if (e > 0 && e < 5) { // 20% chance
                visual_map[i][j] = 'B';
                bombas.push_back({i,j});
            }
            else if (e == 5) { // 5% chance
                visual_map[i][j] = 'V';
                valas.push_back({i,j});
            }
            else { // 70% chance
                visual_map[i][j] = ' ';
            }
        }
    }
}

void see_map() {
    int n = 4;
    for (int z = 0; z < n * 3/2 * map_coluns; z++) cout << "-";
    cout << "\n";

    for (int i = 0; i < map_lines; i++) {
        cout << "|";
        for (int j = 0; j < map_coluns; j++) {
            cout << setw(n) << visual_map[i][j] << " |";
        }
        cout << "\n";
        for (int z = 0; z < n * 3/2 * map_coluns; z++) cout << "-";
        cout << "\n";
    }
}

bool eh_bomba(Coord p) {
    return visual_map[p.x][p.y] == 'B';
}

bool eh_mosca(Coord p) {
    return visual_map[p.x][p.y] == 'M';
}

bool eh_vala(Coord p) {
    return visual_map[p.x][p.y] == 'V';
}

void follow_frog(const Frog& f) {
    visual_map[pos_sapo_visual.x][pos_sapo_visual.y] = ' ';
    visual_map[f.pos.x][f.pos.y] = 'S';

    pos_sapo_visual = f.pos;
}


// --------------------------------------------
// POPULAÇÃO
// --------------------------------------------
vector<int> create_mov() {
    vector<int> mov(8);
    for (int i = 0; i < 8; i++) mov[i] = dist_weight(gen);
    return mov;
}

vector<Frog> create_pop() {
    vector<Frog> pop;
    for (int i = 0; i < size_pop; i++) {
        pop.emplace_back(create_mov(), pos_inicial);
    }
    return pop;
}

void see_pop(const vector<Frog>& pop) {
    for (int i = 0; i < pop.size(); i++) cout << pop[i];
}


// --------------------------------------------
// MOVIMENTAÇÃO
// --------------------------------------------
void moving() {
    vector<int> mortos;

    for (int step = 0; step < leaps; step++) {

        for (int i = 0; i < population.size(); i++) {

            if (find(mortos.begin(), mortos.end(), i) != mortos.end())
                continue;

            population[i].change_position();
            Coord p = population[i].pos;

            if (eh_bomba(p)) {
                population[i].qtd_bombas++;
                mortos.push_back(i);
            }
            if (eh_mosca(p)) {
                population[i].qtd_moscas++;
                visual_map[p.x][p.y] = ' ';
            }

            follow_frog(population[i]);
            see_map();
        }
    }
}


// --------------------------------------------
// AVALIAÇÃO
// --------------------------------------------
void avaliar_sapos(int saltos) {
    float a = 2.0;    // peso moscas
    float b = 0.7;    // peso valas

    score.clear();

    for (int i = 0; i < population.size(); i++) {
        float nota =
            a * population[i].qtd_moscas +
            b * (saltos - population[i].qtd_valas) * (1 - population[i].qtd_bombas);

        score.push_back(nota);

        if (i == 0 || nota > score[MELHOR_DE_TODOS])
            MELHOR_DE_TODOS = i;
    }
}


// --------------------------------------------
// MAIN
// --------------------------------------------
int main() {
    create_active_map();
    see_map();

    cout << "Bombas em: ";
    for (auto& b : bombas) cout << "(" << b.x << "," << b.y << ") ";
    cout << "\n";

    population = create_pop();
    see_pop(population);

    int saltos = 0;
    int geracao = 0;

    while (true) {

        if (saltos % 5 == 0) {
            avaliar_sapos(saltos);
            cout << "Melhor sapo = " << MELHOR_DE_TODOS << endl;
            geracao++;
        }

        moving();
        saltos++;
        break;
    }

    return 0;
}

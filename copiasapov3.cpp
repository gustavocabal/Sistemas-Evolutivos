#include <iomanip>
#include <iostream>
#include <time.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;

#define MAPA_LINHAS 20
#define MAPA_COLUNAS 20
#define gens 10
#define SALTOS_POR_GERACAO 5
#define TAMANHO_POP 10

// "Aleatorização" de dados
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dist_weight(1, 100);
uniform_int_distribution<int> dist_entidades(0, 19);
uniform_int_distribution<int> dist_orientacao(0, 3);

struct Coord {
    int x;
    int y;
};

Coord pos_inicial = {0,2};//{MAPA_LINHAS/2, MAPA_COLUNAS/2}; // mudar depois pra ser aletorio
Coord pos_melhor_sapo = pos_inicial;
vector<vector<char>> visual_map;
vector<Coord> bombas;
vector<Coord> moscas;
vector<Coord> valas;
vector<float> score;
int MELHOR_DE_TODOS = 0;

// Classe sapo
class Frog {
public:
    vector<int> movement;
    Coord pos;
    int orientacao;

    int qtd_moscas = 0;
    int qtd_valas = 0;
    int qtd_bombas = 0;

    // Construtor da classe 
    Frog(vector<int> mov, Coord p, int orient) {
        movement = mov;
        pos = p;
        orientacao = orient;
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
        if (nx >= 0 && nx < MAPA_LINHAS && ny >= 0 && ny < MAPA_COLUNAS) {
            pos.x = nx;
            pos.y = ny;
        }
    }

    friend ostream& operator<<(ostream& os, const Frog& f) {
        os << "Pos: (" << f.pos.x << "," << f.pos.y << ")  Movs: ";
        for (auto w : f.movement) os << w << " ";
        os << " Orientacao: " << f.orientacao;
        os << endl;
        return os;
    }
};

// Gerar população
vector<Frog> populacao;

vector<int> criar_mov() {
    vector<int> mov(6);
    for (int i = 0; i < 6; i++) {
        mov[i] = dist_weight(gen);
    }
    return mov;
}

vector<Frog> criar_pop() {
    vector<Frog> pop;
    for (int i = 0; i < TAMANHO_POP; i++) {
        pop.emplace_back(criar_mov(), pos_inicial, dist_orientacao(gen));
    }
    return pop;
}

void ver_pop(const vector<Frog>& pop) {
    for (int i = 0; i < pop.size(); i++) {
        cout << pop[i];
    }
}

// Gerar mapa

void criar_mapa() {
    visual_map.assign(MAPA_LINHAS, vector<char>(MAPA_COLUNAS, ' '));
    bombas.clear();
    moscas.clear();
    valas.clear();

    for (int i = 0; i < MAPA_LINHAS; i++) {
        for (int j = 0; j < MAPA_COLUNAS; j++) {

            if (i == pos_inicial.x && j == pos_inicial.y) {
                visual_map[i][j] = 'S';
                continue;
            }

            int e = dist_entidades(gen);

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

void ver_mapa() {
    int n = 4;
    for (int z = 0; z < n * 3/2 * MAPA_COLUNAS; z++) cout << "-";
    cout << "\n";

    for (int i = 0; i < MAPA_LINHAS; i++) {
        cout << "|";
        for (int j = 0; j < MAPA_COLUNAS; j++) {
            cout << setw(n) << visual_map[i][j] << " |";
        }
        cout << "\n";
        for (int z = 0; z < n * 3/2 * MAPA_COLUNAS; z++) cout << "-";
        cout << "\n";
    }
}

void follow_frog(const Frog& f) {
    visual_map[pos_melhor_sapo.x][pos_melhor_sapo.y] = ' ';
    visual_map[f.pos.x][f.pos.y] = 'S';

    pos_melhor_sapo = f.pos;
}

// Visão 

bool eh_bomba(Coord p) {
    return visual_map[p.x][p.y] == 'B';
}

bool eh_mosca(Coord p) {
    return visual_map[p.x][p.y] == 'M';
}

bool eh_vala(Coord p) {
    return visual_map[p.x][p.y] == 'V';
}

// logica da visao aqui

// Movimentação

int main() {

    populacao = criar_pop();
    ver_pop(populacao);
    criar_mapa();
    ver_mapa();

}
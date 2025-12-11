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
#define GENES 7
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

static const int movx[9] = {+1,+1,+1, 0, 0, 0,-1,-1,-1};
static const int movy[9] = {-1, 0,+1,-1, 0,+1,-1, 0,+1};

// frente
static const int vf_x[4] = {-1, 0, +1,  0};
static const int vf_y[4] = { 0,+1,  0, -1};

// frente-esquerda
static const int vfe_x[4] = {-1,-1,+1,+1};
static const int vfe_y[4] = {-1,+1,+1,-1};

// frente-direita
static const int vfd_x[4] = {-1,+1,+1,-1};
static const int vfd_y[4] = {+1,+1,-1,-1};


// Classe sapo
class Frog {
public:
    vector<int> movimento;
    vector<int> movimento_real;
    Coord pos;
    int orientacao;

    int qtd_moscas = 0;
    int qtd_valas = 0;
    int qtd_bombas = 0;

    Coord vista_frente;
    Coord vista_fe;
    Coord vista_fd;

    bool ve_bomba_frente = false;
    bool ve_bomba_fe = false;
    bool ve_bomba_fd = false;

    bool ve_mosca_frente = false;
    bool ve_mosca_fe = false;
    bool ve_mosca_fd = false;

    bool ve_vala_frente = false;
    bool ve_vala_fe = false;
    bool ve_vala_fd = false;

    // Construtor da classe 
    Frog(vector<int> mov, Coord p, int orient) {
        movimento = mov;
        pos = p;
        orientacao = orient;
    }

    void definir_movimento(vector<int>& mov2) {
        movimento_real = mov2;
    }

    void change_position() {
        discrete_distribution<> dist_choice(movimento_real.begin(), movimento_real.end());
        int mv = dist_choice(gen);

        // Movimento
        int dx = 0;
        int dy = 0;

        switch (mv) { //Ações do sapo
        case 0: // Pula para o espaço vazio
            
            break; 
        case 1: // Pula para a mosca
            
            break;
        case 2: // Pula para a vala
            
            break;
        case 3: // Pula para a bomba
            
            break;

        case 4: // Gira pra direita
            if (orientacao == 3) {
                orientacao = 0;
            }
            else {
                orientacao += 1;
            }      
            dx = movx[4];
            dy = movy[4];
            break;

        case 5: // Gira pra esquerda
            if (orientacao == 0) {
                orientacao = 3;
            }
            else {
                orientacao -= 1;
            }
            dx = movx[4];
            dy = movy[4];
            break;

        default: // Pula pra um espaço fora do campo de visão
            
            break;
        }

        int nx = pos.x + dx;
        int ny = pos.y + dy;

        // Continua no mapa
        if (nx >= 0 && nx < MAPA_LINHAS && ny >= 0 && ny < MAPA_COLUNAS) {
            pos.x = nx;
            pos.y = ny;
        }
        else {
            change_position(); // Pode causar um loop infinito, mas pouco provavel
        }
    }

    friend ostream& operator<<(ostream& os, const Frog& f) {
        os << "Pos: (" << f.pos.x << "," << f.pos.y << ")  Movs: ";
        for (auto w : f.movimento) os << w << " ";
        os << " Orientacao: " << f.orientacao;
        os << endl;
        return os;
    }
};

// Gerar população
vector<Frog> populacao;

vector<int> criar_mov() {
    vector<int> mov(GENES);
    for (int i = 0; i < GENES; i++) {
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

void visao() {
    for (auto &f : populacao) {

        int o = f.orientacao;

        // Coordenadas das células visíveis
        f.vista_frente = { f.pos.x + vf_x[o], f.pos.y + vf_y[o] }; //verificar se os vetores
        f.vista_fe     = { f.pos.x + vfe_x[o], f.pos.y + vfe_y[o] }; //estao com a sequencia
        f.vista_fd     = { f.pos.x + vfd_x[o], f.pos.y + vfd_y[o] }; //correta de posicoes

        // Reseta percepções
        f.ve_bomba_frente = f.ve_bomba_fe = f.ve_bomba_fd = false;
        f.ve_mosca_frente = f.ve_mosca_fe = f.ve_mosca_fd = false;
        f.ve_vala_frente  = f.ve_vala_fe  = f.ve_vala_fd  = false;

        // FRENTE
        if (f.vista_frente.x >= 0 && f.vista_frente.x < MAPA_LINHAS &&
            f.vista_frente.y >= 0 && f.vista_frente.y < MAPA_COLUNAS)
        {
            char c = visual_map[f.vista_frente.x][f.vista_frente.y];
            if (c == 'B') f.ve_bomba_frente = true;
            if (c == 'M') f.ve_mosca_frente = true;
            if (c == 'V') f.ve_vala_frente  = true;
        }

        // FRENTE ESQUERDA
        if (f.vista_fe.x >= 0 && f.vista_fe.x < MAPA_LINHAS &&
            f.vista_fe.y >= 0 && f.vista_fe.y < MAPA_COLUNAS)
        {
            char c = visual_map[f.vista_fe.x][f.vista_fe.y];
            if (c == 'B') f.ve_bomba_fe = true;
            if (c == 'M') f.ve_mosca_fe = true;
            if (c == 'V') f.ve_vala_fe  = true;
        }

        // FRENTE DIREITA
        if (f.vista_fd.x >= 0 && f.vista_fd.x < MAPA_LINHAS &&
            f.vista_fd.y >= 0 && f.vista_fd.y < MAPA_COLUNAS)
        {
            char c = visual_map[f.vista_fd.x][f.vista_fd.y];
            if (c == 'B') f.ve_bomba_fd = true;
            if (c == 'M') f.ve_mosca_fd = true;
            if (c == 'V') f.ve_vala_fd  = true;
        }
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

// Movimentação

void movimento() {
    for(auto &f : populacao) {
        int contador = 0;
        bool vetor_auxiliar[7] = {1, 0, 0, 0, 1, 1, 1}; // N, M, V, B, Rd, Re, Trás
        if(f.ve_mosca_fe || f.ve_mosca_frente || f.ve_mosca_fd) {
            vetor_auxiliar[1] = 1;
            contador++;
        }
        if(f.ve_vala_fe ||f.ve_vala_frente || f.ve_vala_fd) {
            vetor_auxiliar[2] = 1;
            contador++;
        }
        if(f.ve_bomba_fe || f.ve_bomba_frente || f.ve_bomba_fd) {
            vetor_auxiliar[3] = 1;
            contador++;
        }
        if(contador >= 3) {
            vetor_auxiliar[0] = 0;
        }

        vector<int> movimento_real(GENES);

        for(int i = 0; i < GENES; i++) {
            movimento_real[i] = vetor_auxiliar[i] * f.movimento[i];
        }

        f.definir_movimento(movimento_real);
        f.change_position();

        // Colocar os contadores pra ver se os sapos ja pisaram em algo

    }
}

// Avaliar cada sapo e achar o melhor
// Procriar eles
// Manter o melhor e substituir o resto da população pelos novos sapos
// Graficos em tempo real para vermos a evolução

int main() {

    populacao = criar_pop();
    ver_pop(populacao);
    criar_mapa();
    ver_mapa();

}
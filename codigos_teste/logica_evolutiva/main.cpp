#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <random>
#include <cmath>

using namespace std;

// --- CONFIGURAÇÕES ---
#define MAPA_LINHAS 20
#define MAPA_COLUNAS 20
#define GENES 7
#define TAMANHO_POP 100    
#define GERACOES 1000
#define TURNOS_POR_GERACAO 25 

// --- GLOBAIS DE ALEATORIEDADE E MAPA ---
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dist_weight(1, 100);
uniform_int_distribution<int> dist_entidades(0, 19);
uniform_int_distribution<int> dist_orientacao(0, 3);

struct Coord {
    int x;
    int y;
    bool operator==(const Coord& other) const { return x == other.x && y == other.y; }
};

Coord pos_inicial = {0, 2}; 
vector<vector<char>> visual_map;

// Tabelas de visão (constantes)
static const int vf_x[4] = {-1, 0, +1,  0};
static const int vf_y[4] = { 0,+1,  0, -1};
static const int vfe_x[4] = {-1,-1,+1,+1};
static const int vfe_y[4] = {-1,+1,+1,-1};
static const int vfd_x[4] = {-1,+1,+1,-1};
static const int vfd_y[4] = {+1,+1,-1,-1};

// --- CLASSE SAPO ---
class Frog {
public:
    vector<int> movimento;      
    vector<int> movimento_real; 
    Coord pos;
    int orientacao; 

    bool vivo = true;
    float fitness = -1.0f; // Começa negativo
    int moscas_comidas = 0;
    int bombas_pisadas = 0;
    int valas_pisadas = 0;

    // Visão
    Coord vista_frente, vista_fe, vista_fd;
    bool ve_bomba_frente = false, ve_bomba_fe = false, ve_bomba_fd = false;
    bool ve_mosca_frente = false, ve_mosca_fe = false, ve_mosca_fd = false;
    bool ve_vala_frente  = false, ve_vala_fe  = false, ve_vala_fd  = false;

    // Construtor
    Frog(vector<int> mov, Coord p, int orient) {
        movimento = mov;
        pos = p;
        orientacao = orient;
    }

    // Operador necessário para o sort
    bool operator<(const Frog& other) const {
        return fitness > other.fitness;
    }

    void definir_movimento(vector<int>& mov2) {
        movimento_real = mov2;
    }

    // A lógica antiga de 'jump' agora é esta função complexa
    void change_position() {
        if (!vivo) return;
        discrete_distribution<> dist_choice(movimento_real.begin(), movimento_real.end());
        int mv = dist_choice(gen);

        int dx = 0, dy = 0;
        static const int step_x[] = {-1, 0, +1, 0};
        static const int step_y[] = { 0, +1, 0, -1};

        switch (mv) {
        case 0: case 1: case 2: case 3: case 6: 
            dx = step_x[orientacao];
            dy = step_y[orientacao];
            break;
        case 4: 
            orientacao = (orientacao == 3) ? 0 : orientacao + 1;
            break; 
        case 5: 
            orientacao = (orientacao == 0) ? 3 : orientacao - 1;
            break; 
        default: break;
        }

        int nx = pos.x + dx;
        int ny = pos.y + dy;

        if (nx >= 0 && nx < MAPA_LINHAS && ny >= 0 && ny < MAPA_COLUNAS) {
            pos.x = nx;
            pos.y = ny;
            char terreno = visual_map[nx][ny];
            
            if (terreno == 'B') { vivo = false; bombas_pisadas++; }
            else if (terreno == 'M') { moscas_comidas++; }
            else if (terreno == 'V') { valas_pisadas++; }
        }
    }
};

// --- VARIÁVEL GLOBAL DO MELHOR SAPO ---
Frog global_best(vector<int>(GENES, 0), pos_inicial, 0);

// --- FUNÇÕES AUXILIARES ---

void criar_mapa() {
    visual_map.assign(MAPA_LINHAS, vector<char>(MAPA_COLUNAS, ' '));
    for (int i = 0; i < MAPA_LINHAS; i++) {
        for (int j = 0; j < MAPA_COLUNAS; j++) {
            if (i == pos_inicial.x && j == pos_inicial.y) { visual_map[i][j] = 'S'; continue; }
            int e = dist_entidades(gen);
            if (e == 0)            visual_map[i][j] = 'M';
            else if (e > 0 && e<5) visual_map[i][j] = 'B';
            else if (e == 5)       visual_map[i][j] = 'V';
            else                   visual_map[i][j] = ' ';
        }
    }
}

void atualizar_visao(vector<Frog>& pop) {
    for (auto &f : pop) {
        if (!f.vivo) continue;
        int o = f.orientacao;
        f.vista_frente = { f.pos.x + vf_x[o], f.pos.y + vf_y[o] };
        f.vista_fe     = { f.pos.x + vfe_x[o], f.pos.y + vfe_y[o] };
        f.vista_fd     = { f.pos.x + vfd_x[o], f.pos.y + vfd_y[o] };
        
        f.ve_bomba_frente = f.ve_bomba_fe = f.ve_bomba_fd = false;
        f.ve_mosca_frente = f.ve_mosca_fe = f.ve_mosca_fd = false;
        f.ve_vala_frente  = f.ve_vala_fe  = f.ve_vala_fd  = false;

        auto check = [&](Coord c, bool& b, bool& m, bool& v) {
            if (c.x >= 0 && c.x < MAPA_LINHAS && c.y >= 0 && c.y < MAPA_COLUNAS) {
                char ch = visual_map[c.x][c.y];
                if (ch == 'B') b = true; if (ch == 'M') m = true; if (ch == 'V') v = true;
            }
        };
        check(f.vista_frente, f.ve_bomba_frente, f.ve_mosca_frente, f.ve_vala_frente);
        check(f.vista_fe, f.ve_bomba_fe, f.ve_mosca_fe, f.ve_vala_fe);
        check(f.vista_fd, f.ve_bomba_fd, f.ve_mosca_fd, f.ve_vala_fd);
    }
}

void executar_movimento(vector<Frog>& pop) {
    for(auto &f : pop) {
        if(!f.vivo) continue;
        int contador = 0;
        bool vetor_auxiliar[7] = {1, 0, 0, 0, 1, 1, 1}; 

        if(f.ve_mosca_fe || f.ve_mosca_frente || f.ve_mosca_fd) { vetor_auxiliar[1] = 1; contador++; }
        if(f.ve_vala_fe ||f.ve_vala_frente || f.ve_vala_fd)     { vetor_auxiliar[2] = 1; contador++; }
        if(f.ve_bomba_fe || f.ve_bomba_frente || f.ve_bomba_fd) { vetor_auxiliar[3] = 1; contador++; }
        
        if(contador >= 3) vetor_auxiliar[0] = 0; 

        vector<int> mov_calc(GENES);
        for(int i = 0; i < GENES; i++) mov_calc[i] = vetor_auxiliar[i] * f.movimento[i];
        f.definir_movimento(mov_calc);
        f.change_position();
    }
}

void simular_geracao(vector<Frog>& pop) {
    for(auto& f : pop) {
        f.pos = pos_inicial;
        f.vivo = true;
        f.moscas_comidas = 0;
        f.bombas_pisadas = 0;
        f.valas_pisadas = 0;
        f.orientacao = dist_orientacao(gen);
    }

    for(int i=0; i < TURNOS_POR_GERACAO; i++) {
        atualizar_visao(pop);
        executar_movimento(pop);
    }

    for(auto& f : pop) {
        float fit = (f.moscas_comidas * 100.0f);
        if (f.vivo) fit += 20.0f; else fit -= 50.0f;
        fit -= (f.valas_pisadas * 10.0f); 
        f.fitness = (fit < 0) ? 0 : fit;
    }
}

Frog crossover(const Frog& pai, const Frog& mae, int mutation_rate) {
    vector<int> dna_filho;
    uniform_int_distribution<int> moeda(0, 1);
    for(int i = 0; i < GENES; i++) {
        if (moeda(gen) == 0) dna_filho.push_back(pai.movimento[i]);
        else                 dna_filho.push_back(mae.movimento[i]);
    }
    uniform_int_distribution<int> d100(0, 100);
    if (d100(gen) < mutation_rate) { 
        uniform_int_distribution<int> d_gene(0, GENES - 1);
        dna_filho[d_gene(gen)] = dist_weight(gen);
    }
    return Frog(dna_filho, pos_inicial, 0);
}

Frog torneio(const vector<Frog>& pop) {
    uniform_int_distribution<int> dist_idx(0, pop.size() - 1);
    int best = dist_idx(gen);
    for(int i=0; i<2; i++) { 
        int adv = dist_idx(gen);
        if(pop[adv].fitness > pop[best].fitness) best = adv;
    }
    return pop[best];
}

int main() {
    criar_mapa();
    cout << "Mapa criado!" << endl;

    vector<Frog> population;
    for (int i = 0; i < TAMANHO_POP; i++) {
        vector<int> dna(GENES);
        for(int j=0; j<GENES; j++) dna[j] = dist_weight(gen);
        population.push_back(Frog(dna, pos_inicial, dist_orientacao(gen)));
    }
    
    // Inicializa o global best com um dummy
    global_best = population[0];
    global_best.fitness = -1.0f;

    int taxa_mutacao = 5;

    for (int g = 0; g < GERACOES; g++) {
        simular_geracao(population);
        sort(population.begin(), population.end());

        // --- LÓGICA DO MELHOR GLOBAL ---
        if (population[0].fitness > global_best.fitness) {
            global_best = population[0];
        }

        if (g % 20 == 0 || g == GERACOES - 1) {
            cout << "Gen " << setw(3) << g 
                 << " | Atual Fit: " << setw(4) << population[0].fitness
                 << " | GLOBAL BEST: " << setw(4) << global_best.fitness 
                 << " | Mutacao: " << taxa_mutacao << "%" << endl;
        }

        // Mutação dinâmica
        if (global_best.fitness > population[0].fitness * 1.5) taxa_mutacao = 20;
        else taxa_mutacao = 5;

        vector<Frog> next_gen;
        
        // --- SUPER ELITISMO ---
        next_gen.push_back(global_best); 

        while(next_gen.size() < TAMANHO_POP) {
            Frog p1 = torneio(population);
            Frog p2 = torneio(population);
            next_gen.push_back(crossover(p1, p2, taxa_mutacao));
        }
        population = next_gen;
    }

    cout << "\n--- RESULTADO FINAL ---" << endl;
    cout << "Melhor Sapo da Historia comeu " << global_best.moscas_comidas << " moscas." << endl;
    cout << "Fitness Final: " << global_best.fitness << endl;
    cout << "DNA Campeao: ";
    for(int w : global_best.movimento) cout << w << " ";
    cout << endl;

    return 0;
}
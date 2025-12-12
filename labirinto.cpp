#include <iomanip>
#include <iostream>
#include <time.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>

using namespace std;


#define MAPA_LINHAS 30
#define MAPA_COLUNAS 30
#define GENES 7
#define SALTOS_POR_GERACAO 60 
#define TAMANHO_POP 1000
#define CHANCE_MUTACAO 20
#define MUTACAO_INICIAL 4

struct Coord {
    int x;
    int y;
};

Coord START_POS = {26, 11}; 

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dist_weight(1, 100);
uniform_int_distribution<int> dist_orientacao(0, 3);

vector<vector<char>> visual_map;
vector<vector<char>> visual_map_base; 
vector<float> score;
int MELHOR_DE_TODOS = 0;
float nota_melhor = 0.0f;
vector<int> genes_recordista;

float melhor_fitness_antigo = -1.0f;
int contador_estagnacao = 0;
int taxa_de_mutacao = MUTACAO_INICIAL; 

// Movimentos (N, L, S, O e diagonais)
static const int movx[9] = {+1,+1,+1, 0, 0, 0,-1,-1,-1};
static const int movy[9] = {-1, 0,+1,-1, 0,+1,-1, 0,+1};

// Visão: frente
static const int vf_x[4] = {-1, 0, +1,  0};
static const int vf_y[4] = { 0,+1,  0, -1};
// Visão: frente-esquerda
static const int vfe_x[4] = {-1,-1,+1,+1};
static const int vfe_y[4] = {-1,+1,+1,-1};
// Visão: frente-direita
static const int vfd_x[4] = {-1,+1,+1,-1};
static const int vfd_y[4] = {+1,+1,-1,-1};



// CLASSE SAPO

class Frog {
public:
    vector<int> movimento;
    vector<int> movimento_real;
    
    vector<Coord> moscas_comidas; 

    Coord pos;
    int orientacao;
    bool vivo = true;

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

        int dx = 0; int dy = 0;

        switch (mv) {
            case 0: // Pula vazio
            {
                vector<Coord> opcoes;
                if (vista_fe.x >= 0 && vista_fe.x < MAPA_LINHAS && 
                    vista_fe.y >= 0 && vista_fe.y < MAPA_COLUNAS && 
                    visual_map[vista_fe.x][vista_fe.y] == ' ') opcoes.push_back(vista_fe);

                if (vista_frente.x >= 0 && vista_frente.x < MAPA_LINHAS && 
                    vista_frente.y >= 0 && vista_frente.y < MAPA_COLUNAS && 
                    visual_map[vista_frente.x][vista_frente.y] == ' ') opcoes.push_back(vista_frente);

                if (vista_fd.x >= 0 && vista_fd.x < MAPA_LINHAS && 
                    vista_fd.y >= 0 && vista_fd.y < MAPA_COLUNAS && 
                    visual_map[vista_fd.x][vista_fd.y] == ' ') opcoes.push_back(vista_fd);

                if (!opcoes.empty()) {
                    uniform_int_distribution<> pick(0, opcoes.size()-1);
                    Coord destino = opcoes[pick(gen)];
                    dx = destino.x - pos.x; dy = destino.y - pos.y;
                }
                break;
            }
            case 1: // Pula mosca
            {
                vector<Coord> opcoes;
                if (ve_mosca_fe) opcoes.push_back(vista_fe);
                if (ve_mosca_frente) opcoes.push_back(vista_frente);
                if (ve_mosca_fd) opcoes.push_back(vista_fd);

                if (!opcoes.empty()) {
                    uniform_int_distribution<> pick(0, opcoes.size()-1);
                    Coord destino = opcoes[pick(gen)];
                    dx = destino.x - pos.x; dy = destino.y - pos.y;
                }
                break;
            }
            case 2: // Pula vala
            {
                vector<Coord> opcoes;
                if (ve_vala_fe) opcoes.push_back(vista_fe);
                if (ve_vala_frente) opcoes.push_back(vista_frente);
                if (ve_vala_fd) opcoes.push_back(vista_fd);

                if (!opcoes.empty()) {
                    uniform_int_distribution<> pick(0, opcoes.size()-1);
                    Coord destino = opcoes[pick(gen)];
                    dx = destino.x - pos.x; dy = destino.y - pos.y;
                }
                break;
            }
            case 3: // Pula bomba
            {
                vector<Coord> opcoes;
                if (ve_bomba_fe) opcoes.push_back(vista_fe);
                if (ve_bomba_frente) opcoes.push_back(vista_frente);
                if (ve_bomba_fd) opcoes.push_back(vista_fd);

                if (!opcoes.empty()) {
                    uniform_int_distribution<> pick(0, opcoes.size()-1);
                    Coord destino = opcoes[pick(gen)];
                    dx = destino.x - pos.x; dy = destino.y - pos.y;
                }
                break;
            }
            case 4: // Gira Dir
                orientacao = (orientacao == 3) ? 0 : orientacao + 1;
                dx = movx[4]; dy = movy[4];
                break;
            case 5: // Gira Esq
                orientacao = (orientacao == 0) ? 3 : orientacao - 1;
                dx = movx[4]; dy = movy[4];
                break;
            default: // Pula Cego
                if (orientacao == 0) { uniform_int_distribution<> pk(0, 5); int i=pk(gen); dx=movx[i]; dy=movy[i]; }
                if (orientacao == 1) { vector<int> p={0,1,3,4,6,7}; uniform_int_distribution<> pk(0,p.size()-1); int i=p[pk(gen)]; dx=movx[i]; dy=movy[i]; }
                if (orientacao == 2) { uniform_int_distribution<> pk(3, 8); int i=pk(gen); dx=movx[i]; dy=movy[i]; }
                if (orientacao == 3) { vector<int> p={1,2,4,5,7,8}; uniform_int_distribution<> pk(0,p.size()-1); int i=p[pk(gen)]; dx=movx[i]; dy=movy[i]; }
                break;
        }

        int nx = pos.x + dx;
        int ny = pos.y + dy;

        if (nx >= 0 && nx < MAPA_LINHAS && ny >= 0 && ny < MAPA_COLUNAS) {
            pos.x = nx;
            pos.y = ny;
        } else { 
            orientacao = (orientacao == 3) ? 0 : orientacao + 1;
        }
    }

    friend ostream& operator<<(ostream& os, const Frog& f) {
        os << "Pos: (" << f.pos.x << "," << f.pos.y << ") | Moscas Comidas: " << f.qtd_moscas;
        return os;
    }
};

//              CRIACAO DE MAPA MANUAL (LABIRINTO)

void carregar_mapa_manual() {
    visual_map.assign(MAPA_LINHAS, vector<char>(MAPA_COLUNAS, ' '));
    
    // B = Parede/Bomba (Mata)
    // M = Mosca (Objetivo)
    //   = Caminho
    // START_POS definido como {18, 1}
    
    vector<string> layout = {
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", // 00
    "BMMM                      MMMB", // 01
    "BMMM   BBBBBBBBBBBBBBBBB   V B", // 02
    "BMMM   B                 B   B", // 03
    "BBBB   B    V            B   B", // 04
    "B      B    BBBBBBBBBBBBBB   B", // 05
    "B BBBBBBB                MMM B", // 06
    "B B      BBBBBBBBBBBBBBBBBB B", // 07
    "B B      B                  B", // 08
    "B B   V  B   V    BBBBBBBBB B", // 09
    "B BBBBBBB       B          VB", // 10
    "B          MMM  B   BBBBBBBB", // 11
    "B BBBBBBBBBBBB  B   B       B", // 12
    "B B         V   B   B MMMM  B", // 13
    "B B  BBBBBBBBBBBB   B BBBB  B", // 14
    "B B  B             V B      B", // 15
    "B B  B BBBBBBBBBBBBBB BBBBB B", // 16
    "B B  B B           MMM    B B", // 17
    "B B  B B BBBBBBBBBBBBBBBB B B", // 18
    "B B  B B B                 B B", // 19
    "B B  B B B   V   BBBBBBBBBBB B", // 20
    "B B  B B BBBBBBBB B          B", // 21
    "B B  B B        B B BBBBBBBBBB", // 22
    "B B  B BBBBBBBB B B B        B", // 23
    "B B  B        B B B B MMMM   B", // 24
    "B B  BBBBBBB  B B B B BBBBBBBB", // 25
    "B B         S B B B B        B", // 26  <-- Start aqui
    "B BBBBBBBBBBBB B B BBBBBBBBB B", // 27
    "BMMMMMMMMMMMMMMMMMMMMMMMMMMMMB", // 28  <-- Goal com moedinhas
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"  // 29
    };


    for (int i = 0; i < MAPA_LINHAS; i++) {
        for (int j = 0; j < MAPA_COLUNAS; j++) {
            if (i < layout.size() && j < layout[i].size()) {
                visual_map[i][j] = layout[i][j];
            } else {
                visual_map[i][j] = ' ';
            }
        }
    }
    visual_map_base = visual_map;
}

vector<Frog> populacao;

vector<int> criar_mov() {
    vector<int> mov(GENES);
    for (int i = 0; i < GENES; i++) mov[i] = dist_weight(gen);
    return mov;
}

vector<Frog> criar_pop() {
    vector<Frog> pop;
    for (int i = 0; i < TAMANHO_POP; i++) {
        // TODOS NASCEM NO START_POS
        pop.emplace_back(criar_mov(), START_POS, dist_orientacao(gen));
    }
    return pop;
}

void ver_mapa() {
    int n = 4;
    for (int z = 0; z < n * 3/2 * MAPA_COLUNAS; z++) cout << "-";
    cout << "\n";
    for (int i = 0; i < MAPA_LINHAS; i++) {
        cout << "|";
        for (int j = 0; j < MAPA_COLUNAS; j++) cout << setw(n) << visual_map[i][j] << " |";
        cout << "\n";
        for (int z = 0; z < n * 3/2 * MAPA_COLUNAS; z++) cout << "-";
        cout << "\n";
    }
}

void desenhar_melhor_sapo() {
    visual_map = visual_map_base; 
    Frog& f = populacao[MELHOR_DE_TODOS];
    if(f.pos.x >= 0 && f.pos.x < MAPA_LINHAS && f.pos.y >= 0 && f.pos.y < MAPA_COLUNAS) {
        visual_map[f.pos.x][f.pos.y] = 'S';
    }
}

// Visão Simplificada
void visao() {
    for (auto &f : populacao) {
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
                if (ch == 'B') b = true;
                if (ch == 'M') m = true;
                if (ch == 'V') v = true;
            }
        };

        check(f.vista_frente, f.ve_bomba_frente, f.ve_mosca_frente, f.ve_vala_frente);
        check(f.vista_fe, f.ve_bomba_fe, f.ve_mosca_fe, f.ve_vala_fe);
        check(f.vista_fd, f.ve_bomba_fd, f.ve_mosca_fd, f.ve_vala_fd);
    }
}

void movimento() {
    for(int i = 0; i < populacao.size(); i++) {
        auto &f = populacao[i];
        if (!f.vivo) continue;

        bool vetor_auxiliar[7] = {0, 0, 0, 0, 1, 1, 1}; 
        int qtd_moscas  = f.ve_mosca_fe + f.ve_mosca_frente + f.ve_mosca_fd;
        int qtd_valas   = f.ve_vala_fe  + f.ve_vala_frente  + f.ve_vala_fd;
        int qtd_bombas  = f.ve_bomba_fe + f.ve_bomba_frente + f.ve_bomba_fd;

        if(qtd_moscas > 0) vetor_auxiliar[1] = 1;
        if(qtd_valas  > 0) vetor_auxiliar[2] = 1;
        if(qtd_bombas > 0) vetor_auxiliar[3] = 1;
        if(qtd_moscas + qtd_valas + qtd_bombas < 3) vetor_auxiliar[0] = 1;

        vector<int> movimento_real(GENES);
        for(int k = 0; k < GENES; k++) movimento_real[k] = vetor_auxiliar[k] * f.movimento[k];

        f.definir_movimento(movimento_real);
        f.change_position();

        if (f.pos.x >= 0 && f.pos.x < MAPA_LINHAS && f.pos.y >= 0 && f.pos.y < MAPA_COLUNAS) {
            char terreno = visual_map_base[f.pos.x][f.pos.y]; 

            if(terreno == 'M') {
                // LÓGICA ANTI-FARMING
                bool ja_comeu = false;
                for(auto& m : f.moscas_comidas) {
                    if(m.x == f.pos.x && m.y == f.pos.y) {
                        ja_comeu = true;
                        break;
                    }
                }
                if(!ja_comeu) {
                    f.qtd_moscas++;
                    f.moscas_comidas.push_back(f.pos); // Memoriza essa mosca
                }
            }
            else if(terreno == 'B') {
                f.qtd_bombas++;
                f.vivo = false;
            }
            else if(terreno == 'V') {
                f.qtd_valas++;
            }
        }
    }
}

void avaliar_sapos() {
    float a = 5.0;    
    float b = 0.5;    

    score.clear();
    for (int i = 0; i < populacao.size(); i++) {
        float nota = (a * populacao[i].qtd_moscas * 10) + 
                     (b * (SALTOS_POR_GERACAO - populacao[i].qtd_valas));
        
        if(!populacao[i].vivo) nota /= 2; // Punição por morrer

        score.push_back(nota);

        if (i == MELHOR_DE_TODOS || nota > score[MELHOR_DE_TODOS]) {
            MELHOR_DE_TODOS = i;
        }
    }
}

Frog cruzamento_elitista(const Frog& pai, const Frog& mae) {
    vector<int> genes_filho(GENES);
    for (int i = 0; i < GENES; i++) genes_filho[i] = (pai.movimento[i] + mae.movimento[i]) / 2;
    
    return Frog(genes_filho, START_POS, dist_orientacao(gen));
}

void mutacao(Frog& f, int magnitude_percent) {
    uniform_int_distribution<> prob_ocorrer(1, 100);
    uniform_int_distribution<> delta_dist(-magnitude_percent, magnitude_percent);
    for (int i = 0; i < GENES; i++) {
        if (prob_ocorrer(gen) <= CHANCE_MUTACAO) { 
            int delta = delta_dist(gen);
            f.movimento[i] += delta;
            if (f.movimento[i] < 1) f.movimento[i] = 1;
            if (f.movimento[i] > 100) f.movimento[i] = 100;
        }
    }
}

void criar_nova_geracao(int taxa_de_mutacao) {
    vector<Frog> nova_populacao;

    // Elitismo
    Frog melhor_sapo = populacao[MELHOR_DE_TODOS];
    melhor_sapo.pos = START_POS; // Reset posição
    melhor_sapo.vivo = true;
    melhor_sapo.qtd_moscas = 0;
    melhor_sapo.moscas_comidas.clear(); // Reset memória
    melhor_sapo.qtd_bombas = 0;
    melhor_sapo.qtd_valas = 0;
    nova_populacao.push_back(melhor_sapo);

    for (int i = 0; i < TAMANHO_POP; i++) {
        if (i == MELHOR_DE_TODOS) continue;
        if (nova_populacao.size() >= TAMANHO_POP) break;
        Frog filho = cruzamento_elitista(melhor_sapo, populacao[i]);
        mutacao(filho, taxa_de_mutacao); 
        nova_populacao.push_back(filho);
    }
    populacao = nova_populacao;
    MELHOR_DE_TODOS = 0; 
}

int main() {
    carregar_mapa_manual(); // Cria labirinto
    populacao = criar_pop(); // Cria sapos no START_POS

    float recorde_historico = 0.0f;
    int geracao = 0;
    score.assign(TAMANHO_POP, 0.0f); 

    while(true) {
        for(int s = 0; s < SALTOS_POR_GERACAO; s++) {
            system("clear"); // Linux
            //system("cls"); // Windows 

            visao();
            movimento();
            desenhar_melhor_sapo();
            ver_mapa();

            cout << "Geracao: " << geracao << " | Salto: " << s + 1 
                 << " | Melhor ID: " << MELHOR_DE_TODOS << endl;
            
            cout << "Forca Mutacao: +/-" << taxa_de_mutacao 
                 << " (Estagnacao: " << contador_estagnacao << ")" << endl;

            cout << "STATUS DO CAMPEAO:\n";
            cout << populacao[MELHOR_DE_TODOS] 
                 << " | Nota: " << nota_melhor <<
                 "\n | Recorde: " << recorde_historico << endl;
            
            if (!genes_recordista.empty()) {
                cout << " | Lenda Viva (Genes): [ ";
                for(int g : genes_recordista) cout << g << " ";
                cout << "]" << endl;
            }

            this_thread::sleep_for(chrono::milliseconds(200)); // Rapido
        }

        avaliar_sapos();
        nota_melhor = score[MELHOR_DE_TODOS];
        float melhor_fitness_agora = score[MELHOR_DE_TODOS];

        if (melhor_fitness_agora > melhor_fitness_antigo) {
            melhor_fitness_antigo = melhor_fitness_agora;
            contador_estagnacao = 0;
            taxa_de_mutacao = MUTACAO_INICIAL; 
        } 
        else {
            contador_estagnacao++;
            if (contador_estagnacao > 5) taxa_de_mutacao = MUTACAO_INICIAL + contador_estagnacao/5;
            if (taxa_de_mutacao > 30) taxa_de_mutacao = 30; // Aumentei limite

            if (contador_estagnacao == 30 && !genes_recordista.empty()) { // Reduzi pra 30 pra testar mais rapido
                
                int pior_id = 0;
                float pior_nota = score[0];
                for(int i = 1; i < TAMANHO_POP; i++) {
                    if (score[i] < pior_nota) { pior_nota = score[i]; pior_id = i; }
                }

                populacao[pior_id].movimento = genes_recordista;
                populacao[pior_id].pos = START_POS; 
                populacao[pior_id].orientacao = dist_orientacao(gen);
                populacao[pior_id].vivo = true;
                populacao[pior_id].moscas_comidas.clear(); // CRUCIAL
                populacao[pior_id].qtd_moscas = 0;
                populacao[pior_id].qtd_bombas = 0;
            }
            if (contador_estagnacao > 100) taxa_de_mutacao = 100;
        }
        
        if (melhor_fitness_agora > recorde_historico) {
            recorde_historico = melhor_fitness_agora;
            genes_recordista = populacao[MELHOR_DE_TODOS].movimento;
        }
        
        melhor_fitness_antigo = melhor_fitness_agora;
        criar_nova_geracao(taxa_de_mutacao);
        geracao++;
    }
}
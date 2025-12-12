#include "json.hpp"
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
#include <fstream>

using namespace std;
using json = nlohmann::json;
json elements = json::array();

#define MAPA_LINHAS 20
#define MAPA_COLUNAS 20
#define GENES 7
#define SALTOS_POR_GERACAO 50
#define TAMANHO_POP 1000
#define CHANCE_MUTACAO 20
#define MUTACAO_INICIAL 4

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

Coord pos_inicial = {MAPA_LINHAS/2, MAPA_COLUNAS/2}; 
Coord pos_melhor_sapo = pos_inicial;
vector<vector<char>> visual_map;
vector<vector<char>> visual_map_base; 
vector<Coord> bombas;
vector<Coord> moscas;
vector<Coord> valas;
vector<float> score;
int MELHOR_DE_TODOS = 0;
float nota_melhor = 0.0f;
vector<int> genes_recordista;

float melhor_fitness_antigo = -1.0f;
int contador_estagnacao = 0;
int taxa_de_mutacao = MUTACAO_INICIAL; 

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

        switch (mv) {

            case 0: // Pula para o espaço vazio
            {
                vector<Coord> opcoes;
                
                // Checa Esquerda
                if (vista_fe.x >= 0 && vista_fe.x < MAPA_LINHAS && 
                    vista_fe.y >= 0 && vista_fe.y < MAPA_COLUNAS && 
                    visual_map[vista_fe.x][vista_fe.y] == ' ')
                {
                    opcoes.push_back(vista_fe);
                }

                // Checa Frente
                if (vista_frente.x >= 0 && vista_frente.x < MAPA_LINHAS && 
                    vista_frente.y >= 0 && vista_frente.y < MAPA_COLUNAS && 
                    visual_map[vista_frente.x][vista_frente.y] == ' ')
                {
                    opcoes.push_back(vista_frente);
                }

                // Checa Direita
                if (vista_fd.x >= 0 && vista_fd.x < MAPA_LINHAS && 
                    vista_fd.y >= 0 && vista_fd.y < MAPA_COLUNAS && 
                    visual_map[vista_fd.x][vista_fd.y] == ' ')
                {
                    opcoes.push_back(vista_fd);
                }

                if (!opcoes.empty()) {
                    uniform_int_distribution<> pick(0, opcoes.size()-1);
                    Coord destino = opcoes[pick(gen)];
                    dx = destino.x - pos.x;
                    dy = destino.y - pos.y;
                }
                break;
            }

            case 1: // Pula para a mosca
            {
                vector<Coord> opcoes;

                if (ve_mosca_fe)
                    opcoes.push_back(vista_fe);

                if (ve_mosca_frente)
                    opcoes.push_back(vista_frente);

                if (ve_mosca_fd)
                    opcoes.push_back(vista_fd);

                if (!opcoes.empty()) {
                    uniform_int_distribution<> pick(0, opcoes.size()-1);
                    Coord destino = opcoes[pick(gen)];
                    dx = destino.x - pos.x;
                    dy = destino.y - pos.y;
                }
                break;
            }

            case 2: // Pula para a vala
            {
                vector<Coord> opcoes;

                if (ve_vala_fe)
                    opcoes.push_back(vista_fe);

                if (ve_vala_frente)
                    opcoes.push_back(vista_frente);

                if (ve_vala_fd)
                    opcoes.push_back(vista_fd);

                if (!opcoes.empty()) {
                    uniform_int_distribution<> pick(0, opcoes.size()-1);
                    Coord destino = opcoes[pick(gen)];
                    dx = destino.x - pos.x;
                    dy = destino.y - pos.y;
                }
                break;
            }

            case 3: // Pula para a bomba
            {
                vector<Coord> opcoes;

                if (ve_bomba_fe)
                    opcoes.push_back(vista_fe);

                if (ve_bomba_frente)
                    opcoes.push_back(vista_frente);

                if (ve_bomba_fd)
                    opcoes.push_back(vista_fd);

                if (!opcoes.empty()) {
                    uniform_int_distribution<> pick(0, opcoes.size()-1);
                    Coord destino = opcoes[pick(gen)];
                    dx = destino.x - pos.x;
                    dy = destino.y - pos.y;
                }
                break;
            }

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
            if (orientacao == 0) {
                uniform_int_distribution<> pick(0, 5);
                int idx = pick(gen);
                dx = movx[idx];
                dy = movy[idx];
            }
            if (orientacao == 1) {
                std::vector<int> permitidos = {0, 1, 3, 4, 6, 7};  
                std::uniform_int_distribution<> pick(0, permitidos.size() - 1);
                int idx = permitidos[pick(gen)];
                dx = movx[idx];
                dy = movy[idx];
            }
            if (orientacao == 2) {
                uniform_int_distribution<> pick(3, 8);
                int idx = pick(gen);
                dx = movx[idx];
                dy = movy[idx];
            }
            if (orientacao == 3) {
                std::vector<int> permitidos = {1, 2, 4, 5, 7, 8}; 
                std::uniform_int_distribution<> pick(0, permitidos.size() - 1);
                int idx = permitidos[pick(gen)];
                dx = movx[idx];
                dy = movy[idx];
            }
            break;
        }

        int nx = pos.x + dx;
        int ny = pos.y + dy;

        // Continua no mapa
        if (nx >= 0 && nx < MAPA_LINHAS && ny >= 0 && ny < MAPA_COLUNAS) {
            pos.x = nx;
            pos.y = ny;
        }
        else { // Gira o sapo pra direita se ele quiser se mover pra uma borda
            if (orientacao == 3) {
                orientacao = 0;
            }
            else {
                orientacao += 1;
            }      
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

Coord aleatorizar_posicao() {
    uniform_int_distribution<> d_lin(0, MAPA_LINHAS - 1);
    uniform_int_distribution<> d_col(0, MAPA_COLUNAS - 1);
    return {d_lin(gen),d_col(gen)};
}

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
        pop.emplace_back(criar_mov(), aleatorizar_posicao(), dist_orientacao(gen));
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
                visual_map[i][j] = ' ';
                continue; 
            }

            int e = dist_entidades(gen);

            if (e == 0 || e < 4) { // 20% chance
                visual_map[i][j] = 'M';
                moscas.push_back({i,j});
            }
            else if (e > 4 && e < 9) { // 20% chance
                visual_map[i][j] = 'B';
                bombas.push_back({i,j});
            }
            else if (e == 18 || e == 19) { // 10% chance
                visual_map[i][j] = 'V';
                valas.push_back({i,j});
            }
            else { // 60% chance
                visual_map[i][j] = ' ';
            }
        }
    }
    visual_map_base = visual_map;
}

Coord posicao_segura() {
    int tentativas = 0;
    Coord pos;

    // Tenta sortear uma posição até achar um Espaço Vazio (' ')
    do {
        pos = aleatorizar_posicao();
        tentativas++;
        if (tentativas > 500) break; // Evita travar se o mapa estiver cheio
    } while (visual_map[pos.x][pos.y] != ' '); 

    return pos;
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

void desenhar_melhor_sapo() {
    // Restaura o mapa original (recupera moscas que o sapo pisou)
    visual_map = visual_map_base; 

    //  Pega o melhor sapo da geração anterior (ou o 0 se for a primeira)
    Frog& f = populacao[MELHOR_DE_TODOS];

    // Desenha ele se estiver dentro do mapa
    if(f.pos.x >= 0 && f.pos.x < MAPA_LINHAS && f.pos.y >= 0 && f.pos.y < MAPA_COLUNAS) {
        visual_map[f.pos.x][f.pos.y] = 'S';
    }
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

// Movimentação

void movimento() {
    for(int i = 0; i < populacao.size(); i++) {
        auto &f = populacao[i];

        if (!f.vivo) continue;

        bool vetor_auxiliar[7] = {0, 0, 0, 0, 1, 1, 1}; // N, M, V, B, Rd, Re, Trás

        int qtd_moscas  = f.ve_mosca_fe + f.ve_mosca_frente + f.ve_mosca_fd;
        int qtd_valas   = f.ve_vala_fe  + f.ve_vala_frente  + f.ve_vala_fd;
        int qtd_bombas  = f.ve_bomba_fe + f.ve_bomba_frente + f.ve_bomba_fd;

        // Ativações baseadas na visão
        if(qtd_moscas > 0) vetor_auxiliar[1] = 1;
        if(qtd_valas  > 0) vetor_auxiliar[2] = 1;
        if(qtd_bombas > 0) vetor_auxiliar[3] = 1;

        if(qtd_moscas + qtd_valas + qtd_bombas < 3) {
            vetor_auxiliar[0] = 1;
        }

        vector<int> movimento_real(GENES);

        for(int i = 0; i < GENES; i++) {
            movimento_real[i] = vetor_auxiliar[i] * f.movimento[i];
        }

        f.definir_movimento(movimento_real);
        f.change_position();

        // Verficação de mapa de novo porque não aguento mais segmentation fault :( 
        if (f.pos.x >= 0 && f.pos.x < MAPA_LINHAS && f.pos.y >= 0 && f.pos.y < MAPA_COLUNAS) {
            
            // Olha no mapa base para saber o que tem no chão (M, B, V)
            char terreno = visual_map_base[f.pos.x][f.pos.y]; 

            if(terreno == 'M') {
                bool ja_comeu = false;

                for(auto& m : f.moscas_comidas) {
                    if(m.x == f.pos.x && m.y == f.pos.y) {
                        ja_comeu = true;
                        break;
                    }
                }

                if(!ja_comeu) {
                    f.qtd_moscas++;
                    f.moscas_comidas.push_back(f.pos); // Salva na memória
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

// Avaliação

void avaliar_sapos() {
    float a = 2.0;    // peso moscas
    float b = 0.7;    // peso valas

    score.clear();

    for (int i = 0; i < populacao.size(); i++) {
        float nota =
            a * (populacao[i].qtd_moscas) +
            b * (SALTOS_POR_GERACAO - populacao[i].qtd_valas) * (1 - populacao[i].qtd_bombas);

        score.push_back(nota);

        // Mantém salvo e encontra o novo melhor de todos
        if (i == MELHOR_DE_TODOS || nota > score[MELHOR_DE_TODOS]) {
            MELHOR_DE_TODOS = i;
            pos_melhor_sapo = populacao[i].pos;
        }
    }
}

// Procriação

// Elitismo 
Frog cruzamento_elitista(const Frog& pai, const Frog& mae) {
    vector<int> genes_filho(GENES);
    for (int i = 0; i < GENES; i++) {
        genes_filho[i] = (pai.movimento[i] + mae.movimento[i]) / 2;
    }
    return Frog(genes_filho, posicao_segura(), dist_orientacao(gen));
}

// Mutação 
void mutacao(Frog& f, int magnitude_percent) {
    uniform_int_distribution<> prob_ocorrer(1, 100);
    
    uniform_int_distribution<> delta_dist(-magnitude_percent, magnitude_percent);

    for (int i = 0; i < GENES; i++) {
        if (prob_ocorrer(gen) <= CHANCE_MUTACAO) { 
            int delta = delta_dist(gen);
            
            f.movimento[i] += delta;

            // Garante que todo nosso trabalho não será destruido (odeio o segmentation fault)
            if (f.movimento[i] < 1) f.movimento[i] = 1;
            if (f.movimento[i] > 100) f.movimento[i] = 100;
        }
    }
}

void criar_nova_geracao(int taxa_de_mutacao) {
    vector<Frog> nova_populacao;

    // Elitismo
    Frog melhor_sapo = populacao[MELHOR_DE_TODOS];
    melhor_sapo.pos = posicao_segura();
    melhor_sapo.vivo = true;
    melhor_sapo.qtd_moscas = 0;
    melhor_sapo.qtd_bombas = 0;
    melhor_sapo.qtd_valas = 0;
    nova_populacao.push_back(melhor_sapo);

    // Cruzamento
    for (int i = 0; i < TAMANHO_POP; i++) {
        if (i == MELHOR_DE_TODOS) continue;
        if (nova_populacao.size() >= TAMANHO_POP) break;

        Frog filho = cruzamento_elitista(melhor_sapo, populacao[i]);
        
        mutacao(filho, taxa_de_mutacao); 

        nova_populacao.push_back(filho);
    }
    populacao = nova_populacao;
    MELHOR_DE_TODOS = 0; // Necessário já que trocamos a posição do melhor no novo vetor
}

void SaveTable();

int main() {
    pos_inicial = aleatorizar_posicao();
    pos_melhor_sapo = pos_inicial;

    populacao = criar_pop();
    //ver_pop(populacao);
    criar_mapa();

    for (int i = 0; i < TAMANHO_POP; i++) {
        populacao[i].pos = posicao_segura();
    }

    int geracao = 0;
    float recorde_historico = 0.0f;
    score.assign(TAMANHO_POP, 0.0f); // Mais uma medida pra evitar o segmentation fault :(

    ofstream arquivo_csv("dados_evolucao.csv");
    arquivo_csv << "Geracao,MelhorFitness,MediaFitness,RecordeHistorico,GenesR,GenesA" << endl;

    while(true) {
        for(int s = 0; s < SALTOS_POR_GERACAO; s++) {
            // Limpa o terminal
            system("clear"); // Trocar pra cls se for windows 

            // Logica de visao e movimento
            visao();
            movimento();

            // Printa apenas o melhor sapo
            desenhar_melhor_sapo();

            // Print
            //ver_mapa();
            SaveTable();

            cout << "Geracao: " << geracao << " | Salto: " << s + 1 
                 << " | Melhor ID: " << MELHOR_DE_TODOS << endl;
            
            cout << "Forca Mutacao: +/-" << taxa_de_mutacao 
                 << " (Estagnacao: " << contador_estagnacao << ")" << endl;

            cout << "STATUS DO CAMPEAO:\n";
            cout << populacao[MELHOR_DE_TODOS] 
                 << " | Nota do melhor da geracao: " << nota_melhor <<
                 "\n | Recorde historico: " << recorde_historico << endl;
            if (!genes_recordista.empty()) {
                cout << " | Genes do recordista: [ ";
                for(int g : genes_recordista) cout << g << " ";
                cout << "]" << endl;
            }

            // Breve pausa 
            this_thread::sleep_for(chrono::milliseconds(200));
        }
        // Fim da geração
        avaliar_sapos();
        nota_melhor = score[MELHOR_DE_TODOS];

        float soma_fitness = 0;
        for(float s : score) soma_fitness += s;
        float media_fitness = soma_fitness / TAMANHO_POP;

        string genes_r_str = "[";
        if (!genes_recordista.empty()) {
            for(size_t i = 0; i < genes_recordista.size(); i++) {
                genes_r_str += to_string(genes_recordista[i]);
                if(i < genes_recordista.size() - 1) genes_r_str += "-";
            }
        }
        genes_r_str += "]";

        string genes_a_str = "[";
        vector<int>& mov_atual = populacao[MELHOR_DE_TODOS].movimento; 
        
        for(size_t i = 0; i < mov_atual.size(); i++) {
            genes_a_str += to_string(mov_atual[i]);
            if(i < mov_atual.size() - 1) genes_a_str += "-";
        }
        genes_a_str += "]";

        if (arquivo_csv.is_open()) {
            arquivo_csv << geracao << "," 
                        << nota_melhor << "," 
                        << media_fitness << "," 
                        << recorde_historico << ","
                        << genes_r_str << ","   
                        << genes_a_str << endl; 
                        
            arquivo_csv.flush(); 
        }

        float melhor_fitness_agora = score[MELHOR_DE_TODOS];

        // Indicador de tendencia de melhora
        if (melhor_fitness_agora > melhor_fitness_antigo) {
            melhor_fitness_antigo = melhor_fitness_agora;
            contador_estagnacao = 0;
            taxa_de_mutacao = MUTACAO_INICIAL; 
        } 

        // Indicador de estagnação
        else {
            contador_estagnacao++;
            
            // Acumula +1 de mutacao a cada 5 gerações estagnadas
            if (contador_estagnacao > 5) {
                taxa_de_mutacao = MUTACAO_INICIAL + contador_estagnacao/5;
            }

            // Impede taxas de mutação absurdas
            if (taxa_de_mutacao > 20) taxa_de_mutacao = 20;

            if (contador_estagnacao == 50 && !genes_recordista.empty()) {
                
                // Encontrar o índice do PIOR sapo da geração atual
                int pior_id = 0;
                float pior_nota = score[0];
                
                for(int i = 1; i < TAMANHO_POP; i++) {
                    if (score[i] < pior_nota) {
                        pior_nota = score[i];
                        pior_id = i;
                    }
                }

                // Substitui DNA do pior pelo DNA do recordista
                populacao[pior_id].movimento = genes_recordista;
                populacao[pior_id].pos = posicao_segura(); 
                populacao[pior_id].orientacao = dist_orientacao(gen);
                populacao[pior_id].vivo = true;
                populacao[pior_id].moscas_comidas.clear(); 
                populacao[pior_id].qtd_moscas = 0; 
                populacao[pior_id].qtd_bombas = 0;
                populacao[pior_id].qtd_valas = 0;
            }

            // Caso a estagnação seja muito alta, testamos uma mudança brusca
            if (contador_estagnacao > 100) taxa_de_mutacao = 100;
        }
        
        if (melhor_fitness_agora > recorde_historico) {
            recorde_historico = melhor_fitness_agora;
            genes_recordista = populacao[MELHOR_DE_TODOS].movimento;
        }
        /* Salva o melhor do momento para indicar uma tendencia de melhora,
        já que as vezes é difícil para os novos sapos superarem o recorde de um 
        sapo muito sortudo */
        melhor_fitness_antigo = melhor_fitness_agora;

        criar_nova_geracao(taxa_de_mutacao);
        
        geracao++;
    }
}

void SaveTable() {

    // limpa elementos antes de preencher
    // caso contrário JSON acumula dados
     elements.clear();

    for (size_t i = 0; i < MAPA_LINHAS; i++) {
        for (size_t j = 0; j < MAPA_COLUNAS; j++) {

            if (visual_map[i][j] == ' ')
                continue;

            json obj;  // Criado apenas 1 vez por célula

            switch (visual_map[i][j]) {
                case 'M':
                    obj["type"] = "Fly";
                    obj["direction"] = "None";
                    break;
                case 'B':
                    obj["type"] = "mine";
                    obj["direction"] = "None";
                    break;
                case 'V':
                    obj["type"] = "Hole";
                    obj["direction"] = "None";
                    break;
                case 'S':
                    obj["type"] = "Sapo";
                    obj["direction"] = populacao[MELHOR_DE_TODOS].orientacao;
                    break;
                default:
                    continue;
            }

            obj["row"] = (int)i;
            obj["col"] = (int)j;
            obj["passo"] = 0;

            elements.push_back(obj);
        }
    }
}
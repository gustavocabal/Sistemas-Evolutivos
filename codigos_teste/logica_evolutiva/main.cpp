#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <random>
#include "Sapo.h" 

using namespace std;

const int SIZE_POP = 100;
const int GENS = 1000;       
const int LEAPS = 20;       

random_device rd;
mt19937 gen(rd()); 
uniform_int_distribution<> dist_weight(1, 100);

vector<int> bombs;
vector<int> flies;
int inicial_position = (10 * MAP_LINES / 2 + MAP_COLS / 2); 

void setup_map() {
    bombs.clear();
    flies.clear();
    uniform_int_distribution<int> dist_entity(0, 100);

    for (int i = 0; i < MAP_LINES * MAP_COLS; i++) {
        if (i == inicial_position) continue;
        int chance = dist_entity(gen);
        if (chance < 10) flies.push_back(i);      
        else if (chance < 15) bombs.push_back(i); 
    }
}

void simulate_generation(vector<Frog>& pop) {
    for (auto& frog : pop) {
        vector<int> local_flies = flies; 
        frog.moscas_comidas = 0;
        frog.vivo = true;
        frog.position = inicial_position; 
        int turnos_vividos = 0;

        for (int k = 0; k < LEAPS; k++) {
            if (!frog.vivo) break;

            frog.jump();
            turnos_vividos++;

            // Verifica Bomba
            for (int b : bombs) {
                if (frog.position == b) {
                    frog.vivo = false;
                    break;
                }
            }
            
            // Verifica Mosca
            if (frog.vivo) {
                for (size_t f = 0; f < local_flies.size(); f++) {
                    if (frog.position == local_flies[f]) {
                        frog.moscas_comidas++;
                        local_flies[f] = local_flies.back();
                        local_flies.pop_back();
                        break;
                    }
                }
            }
        }
        // Fitness
        float bonus_vida = frog.vivo ?  50.0f: 0.0f;
        frog.fitness = (1000.0f * frog.moscas_comidas) + bonus_vida + turnos_vividos;
    }
}

// --- Crossover (com taxa de mutação variável) ---
Frog crossover(const Frog& pai, const Frog& mae, int mutation_rate) {
    vector<int> dna_filho;
    uniform_int_distribution<int> moeda(0, 1);
    
    for(int i = 0; i < 8; i++) {
        if (moeda(gen) == 0) dna_filho.push_back(pai.movement[i]);
        else                 dna_filho.push_back(mae.movement[i]);
    }
    
    // Mutação usando a taxa passada como argumento
    uniform_int_distribution<int> d100(0, 100);
    uniform_int_distribution<int> d_gene(0, 7); 

    if (d100(gen) < mutation_rate) { 
        int idx = d_gene(gen);
        dna_filho[idx] = dist_weight(gen); 
    }

    return Frog(dna_filho, inicial_position);
}

// --- Torneio ---
Frog tournament_selection(const vector<Frog>& pop) {
    uniform_int_distribution<int> dist_idx(0, pop.size() - 1);
    
    // Sorteia índice inicial
    int best_idx = dist_idx(gen);
    
    // Faz 2 desafios (Torneio de tamanho 3)
    for (int i = 0; i < 2; i++) {
        int contender_idx = dist_idx(gen);
        if (pop[contender_idx].fitness > pop[best_idx].fitness) {
            best_idx = contender_idx;
        }
    }
    return pop[best_idx]; // Retorna cópia do vencedor
}

int main() {
    setup_map();

    // 1. População Inicial
    vector<Frog> population;
    for (int i = 0; i < SIZE_POP; i++) {
        vector<int> random_mov;
        for(int j=0; j<8; j++) random_mov.push_back(dist_weight(gen));
        population.push_back(Frog(random_mov, inicial_position));
    }

    cout << "Mapa criado! Moscas: " << flies.size() << " | Bombas: " << bombs.size() << endl;

    float last_best_fitness = -1.0f;
    int stagnation_count = 0;
    int current_mutation = 5; 

    // 2. Loop Evolutivo
    for (int g = 0; g < GENS; g++) {
        simulate_generation(population);
        sort(population.begin(), population.end());

        // Controle de Estagnação (Mutação Adaptativa)
        if (population[0].fitness > last_best_fitness) {
            last_best_fitness = population[0].fitness;
            stagnation_count = 0;
            current_mutation = 5; // Reseta mutação para 5%
        } else {
            stagnation_count++;
        }

        // Se travou por 10 gerações, aumenta mutação
        if (stagnation_count > 10) {
            current_mutation = 50; 
        }

        if (g % 5 == 0 || g == GENS - 1) {
            cout << "Gen " << setw(3) << g 
                 << " | Best Fit: " << setw(4) << population[0].fitness
                 << " | Moscas: " << population[0].moscas_comidas
                 << " | Mutacao: " << setw(2) << current_mutation << "%" 
                 << " | Vivo: " << (population[0].vivo ? "S" : "N") << endl;
        }

        vector<Frog> next_gen;
        // Elitismo: Salva o Top 1
        next_gen.push_back(population[0]); 

        // Restante por Torneio + Crossover
        while(next_gen.size() < SIZE_POP) {
            Frog pai = tournament_selection(population);
            Frog mae = tournament_selection(population);
            next_gen.push_back(crossover(pai, mae, current_mutation));
        }
        population = next_gen;
    
    }

    cout << "\n--- RESULTADO FINAL ---" << endl;
    cout << "Melhor Sapo comeu " << population[0].moscas_comidas << " moscas." << endl;
    cout << "DNA do Campeao: ";
    for(int w : population[0].movement) cout << w << " ";
    cout << endl;

    return 0;
}
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <random>
#include "Sapo.h" 

using namespace std;

// --- CONFIGURAÇÕES DO AG ---
const int SIZE_POP = 100;
const int GENS = 50;
const int LEAPS = 20;
const int MUTATION_RATE = 5;

// --- GLOBAIS ---
random_device rd;
mt19937 gen(rd()); 
uniform_int_distribution<> dist_weight(1, 100);

vector<int> bombs;
vector<int> flies;
int inicial_position = (10 * MAP_LINES / 2 + MAP_COLS / 2);

// --- FUNÇÕES AUXILIARES ---

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

        for (int k = 0; k < LEAPS; k++) {
            if (!frog.vivo) break;

            frog.jump();

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
        float bonus_vida = frog.vivo ? (LEAPS * 1.0f) : 0.0f;
        frog.fitness = (50.0f * frog.moscas_comidas) + bonus_vida;
    }
}

Frog crossover(const Frog& pai, const Frog& mae) {
    vector<int> dna_filho = pai.movement;
    int cut = 4; 
    for(int i = cut; i < 8; i++) {
        dna_filho[i] = mae.movement[i];
    }
    
    // Mutação
    uniform_int_distribution<int> d100(0, 100);
    if (d100(gen) < MUTATION_RATE) {
        int idx = rand() % 8;
        dna_filho[idx] = dist_weight(gen);
    }

    return Frog(dna_filho, inicial_position);
}

// --- MAIN ---

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

    // 2. Loop Evolutivo
    for (int g = 0; g < GENS; g++) {
        simulate_generation(population);
        sort(population.begin(), population.end());

        if (g % 5 == 0 || g == GENS - 1) {
            cout << "Gen " << setw(2) << g 
                 << " | Melhor Fitness: " << setw(4) << population[0].fitness
                 << " | Moscas: " << population[0].moscas_comidas
                 << " | Vivo: " << (population[0].vivo ? "SIM" : "NAO") << endl;
        }

        vector<Frog> next_gen;
        next_gen.push_back(Frog(population[0].movement, inicial_position)); // Top 1
        next_gen.push_back(Frog(population[1].movement, inicial_position)); // Top 2

        while(next_gen.size() < SIZE_POP) {
            int r1 = rand() % (SIZE_POP / 2);
            int r2 = rand() % (SIZE_POP / 2);
            next_gen.push_back(crossover(population[r1], population[r2]));
        }
        population = next_gen;
    }

    cout << "\nDNA do Campeao: ";
    for(int w : population[0].movement) cout << w << " ";
    cout << endl;

    return 0;
}
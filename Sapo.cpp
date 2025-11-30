#include "Sapo.h"
#include <iostream>

using namespace std;

// Implementação do Construtor
Frog::Frog(vector<int> mov, int pos) {
    movement = mov;
    position = pos;
    fitness = 0;
    moscas_comidas = 0;
    vivo = true;
}

// Implementação do Pulo
void Frog::jump() {
    if (!vivo) return;

    // Usa os pesos do genoma para escolher a direção
    discrete_distribution<> dist_choice(movement.begin(), movement.end());
    int chosen_movement = dist_choice(gen); // Usa o 'gen' global

    int old_pos = position;

    // Lógica de Movimento
    switch (chosen_movement) {
        case 0: position += (MAP_COLS - 1); break;
        case 1: position += MAP_COLS; break;
        case 2: position += (MAP_COLS + 1); break;
        case 3: position -= 1; break;
        case 4: position += 1; break;
        case 5: position -= (MAP_COLS - 1); break;
        case 6: position -= (MAP_COLS); break;
        case 7: position -= (MAP_COLS + 1); break;
    }

    // Proteção de Borda (Se sair do mapa, volta para onde estava)
    if (position < 0 || position >= MAP_LINES * MAP_COLS) {
        position = old_pos;
    }
}

// Implementação da Comparação (para o Ranking)
bool Frog::operator<(const Frog& other) const {
    return fitness > other.fitness; // Do maior para o menor
}
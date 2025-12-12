#include "Sapo.h"
#include <iostream>

using namespace std;

Frog::Frog(vector<int> mov, int pos) {
    movement = mov;
    position = pos;
    fitness = 0;
    moscas_comidas = 0;
    vivo = true;
}

void Frog::jump() {
    if (!vivo) return;

    discrete_distribution<> dist_choice(movement.begin(), movement.end());
    int chosen_movement = dist_choice(gen);

    int old_pos = position;

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

    if (position < 0 || position >= MAP_LINES * MAP_COLS) {
        position = old_pos;
    }
}

bool Frog::operator<(const Frog& other) const {
    return fitness > other.fitness;
}
#ifndef SAPO_H
#define SAPO_H

#include <vector>
#include <random>

const int MAP_LINES = 8;
const int MAP_COLS = 8;

extern std::mt19937 gen;

class Frog {
public:
    std::vector<int> movement; // Genoma (pesos)
    int position;
    
    // Status para avaliação
    float fitness;
    int moscas_comidas;
    bool vivo;

    // Construtor
    Frog(std::vector<int> mov, int pos);

    // Métodos (Ações)
    void jump(); 
    
    // Operador para ordenar (necessário para o sort)
    bool operator<(const Frog& other) const;
};

#endif
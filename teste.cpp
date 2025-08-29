#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>

using namespace std;

const int pessoas = 100;
const int genes = 1;
const int geracoes = 10;

int melhor_individuo = 0;

vector<vector<float>> gerar_individuos();
void print_populacao(vector<vector<float>>&);
void avaliacao(vector<vector<float>>&);
void crossover(vector<vector<float>>&);

int main() {
    srand(time(0));
    // int pessoas, genes;
    //cout << "Quantos indivíduos haverá inicialmente na população?" << endl;
    //cin >> pessoas;
    
    vector<vector<float>> populacao = gerar_individuos();
    print_populacao(populacao);
    avaliacao(populacao);
    cout << "\n\n\n CROSSOVER \n\n" << endl;
    for(int i = 0; i < geracoes; i++) {
        crossover(populacao);
    }
    print_populacao(populacao);
    avaliacao(populacao);

    return 0;
}

vector<vector<float>> gerar_individuos() {
    srand(time(0));
    vector<vector<float>> populacao;

    for(int i = 0; i < pessoas; i++) {
        vector<float> pessoa;
        for(int j = 0; j < genes; j++) {
            pessoa.push_back(static_cast<float>(rand()) / RAND_MAX * 20);
        }
        populacao.push_back(pessoa);
    }

    return move(populacao);
}

void print_populacao(vector<vector<float>> &populacao) {
    for(int i = 0; i < pessoas; i++) {
        cout << "Pessoa " << i + 1 << ": ";
        for(int j = 0; j < genes; j++) {
            if(j == genes - 1) cout << populacao[i][j];
            else cout << populacao[i][j] << ", ";
        }
        cout << endl;
    }
}

void avaliacao(vector<vector<float>> &populacao) {
    vector<float> notas;
    const int j = 0; 
    // FUNÇÃO
    for(int i = 0; i < pessoas; i++) {
        float x = populacao[i][j];
        if(x < 10) {
            notas.push_back(x);
        }
        else {
            notas.push_back(20 - x);
        }
    }

    // PRINT DAS NOTAS  
    for(int i = 0; i < pessoas; i++) {
        cout << "Nota pessoa " << i + 1 << ": " << notas[i] << endl;
    }

    for(int i = 0; i < pessoas; i++) {
        if(notas[melhor_individuo] < notas[i]) {
            melhor_individuo = i;
        }
    }

    cout << "Melhor: " << melhor_individuo + 1 << 
    " || Nota: " << notas[melhor_individuo] << endl;
}

void crossover(vector<vector<float>> &populacao) {
    for(int i = 0; i < pessoas; i++) {
        if(i == melhor_individuo) continue; 
        populacao[i][0] = (populacao[i][0] + populacao[melhor_individuo][0]) / 2.0;

        // MUTAÇÃO
        float mut = ((float)(rand() % 100) / 100.0f) * 0.8f - 0.4f;
        populacao[i][0] = populacao[i][0] + mut;
    }
}
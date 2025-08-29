#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>

using namespace std;

vector<vector<float>> gerar_individuos(const int, const int);
void print_populacao(vector<vector<float>>&, const int, const int);
void avaliacao(vector<vector<float>>&, const int);

int main() {
    int pessoas, genes;
    cout << "Quantos indivíduos haverá inicialmente na população?" << endl;
    cin >> pessoas;
    genes = 1;
    vector<vector<float>> populacao = gerar_individuos(pessoas, genes);
    print_populacao(populacao, pessoas, genes);
    avaliacao(populacao, pessoas);

    return 0;
}

vector<vector<float>> gerar_individuos(const int pessoas, const int genes) {
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

void print_populacao(vector<vector<float>> &populacao,
                    const int pessoas, const int genes) 
{
    for(int i = 0; i < pessoas; i++) {
        cout << "Pessoa " << i + 1 << ": ";
        for(int j = 0; j < genes; j++) {
            if(j == genes - 1) cout << populacao[i][j];
            else cout << populacao[i][j] << ", ";
        }
        cout << endl;
    }
}

void avaliacao(vector<vector<float>> &populacao, const int pessoas) {
    vector<float> notas;
    const int j = 0; 
    for(int i = 0; i < pessoas; i++) {
        float x = populacao[i][j];
        if(x < 10) {
            notas.push_back(x);
        }
        else {
            notas.push_back(20 - x);
        }
    }

    for(int i = 0; i < pessoas; i++) {
        cout << "Nota pessoa " << i + 1 << ": " << notas[i] << endl;
    }

    int melhor_individuo = 0;
    for(int i; i < pessoas; i++) {
        if(notas[melhor_individuo] < notas[i]) {
            melhor_individuo = i;
        }
    }

    cout << "Melhor: " << melhor_individuo + 1 << 
    " || Nota: " << notas[melhor_individuo] << endl;
}
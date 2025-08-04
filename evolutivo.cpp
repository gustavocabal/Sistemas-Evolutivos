#include <iostream>
#include <vector>
#include <ctime>
#define MUTACAO 0.02

using namespace std;

vector<vector<int>> gerar_individuos(const int pessoas, const int genes) {
    srand(time(0));
    vector<vector<int>> populacao;

    for(int i = 0; i < pessoas; i++) {
        vector<int> pessoa;
        for(int j = 0; j < genes; j++) {
            pessoa.push_back(rand() % 100);
        }
        populacao.push_back(pessoa);
    }

    return move(populacao);
}

vector<int> copulacao(vector<int> pai, vector<int> mae) {
    int n = pai.size();
    vector<int> filho;
    for (int i = 0; i < n; i++) {
        filho.push_back(((pai[i] + mae[i]) / 2.0) * (1 + MUTACAO));
    }
    return move(filho);
}

int main() {
    int pessoas, genes;
    cout << "Quantos indivíduos haverá inicialmente na população?" << endl;
    cin >> pessoas;
    cout << "Quantos genes em cada pessoa?" << endl;
    cin >> genes;

    vector<vector<int>> populacao = gerar_individuos(pessoas, genes);

    // Teste para cada individuo l
    for(int i = 0; i < pessoas; i++) {
        cout << "Pessoa " << i + 1 << ": ";
        for(int j = 0; j < genes; j++) {
            if(j == genes - 1) cout << populacao[i][j];
            else cout << populacao[i][j] << ", ";
        }
        cout << endl;
    }
    
    vector<int> filho = copulacao(populacao[rand() % pessoas], populacao[rand() % pessoas]);

    // Teste filho
    for(int i = 0; i < genes; i++) {
        if(i == genes - 1) cout << filho[i];
        else cout << filho[i] << ", ";
    }

    return 0;
}
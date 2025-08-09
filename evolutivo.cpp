#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#define MUTACAO 0.02

using namespace std;

vector<vector<float>> gerar_individuos(const int pessoas, const int genes) {
    srand(time(0));
    vector<vector<float>> populacao;

    for(int i = 0; i < pessoas; i++) {
        vector<float> pessoa;
        for(int j = 0; j < genes; j++) {
            pessoa.push_back(static_cast<float>(rand()) / RAND_MAX * 100);
        }
        populacao.push_back(pessoa);
    }

    return move(populacao);
}

vector<float> copulacao(vector<float> pai, vector<float> mae) {
    int n = pai.size();
    vector<float> filho;
    for (int i = 0; i < n; i++) {
        filho.push_back(((pai[i] + mae[i]) / 2.0) * (1 + MUTACAO));
    }
    return move(filho);
}

float funcao(float x, float y, float z) {
    //Constantes
    float a = 1;
    float b = 0;
    float c = 0;

    //Expoentes
    float exp1 = 2;
    float exp2 = 3;
    float exp3 = 5;

    return a*pow(x, exp1) + b*pow(y, exp2) + c*pow(z, exp3);
}

void testar_funcao(vector<vector<float>>& populacao, int pessoas, int genes) {
    float temp = 0;
    float maior_valor = temp; //Iguais pra nao dar BO no if
    int melhor;

    for(int i = 0; i < pessoas; i++) {
        temp = funcao(populacao[i][0], populacao[i][1], populacao[i][2]);
        if(i == 0 || temp > maior_valor) {
            maior_valor = temp;
            melhor = i;
        }
    }

    cout << "O maior valor é: " << maior_valor << endl;
    cout << "O melhor individuo é: " << melhor + 1 << "\nSeus genes são: ";
    for(int i = 0; i < genes; i++) {
        if(i == genes - 1) cout << populacao[melhor][i] << endl;
        else cout << populacao[melhor][i] << ", ";
    }
}

int main() {
    int pessoas, genes;
    cout << "Quantos indivíduos haverá inicialmente na população?" << endl;
    cin >> pessoas;
    //cout << "Quantos genes em cada pessoa?" << endl;
    //cin >> genes;
    
    genes = 3;
    vector<vector<float>> populacao = gerar_individuos(pessoas, genes);

    // Print de cada individuo
    for(int i = 0; i < pessoas; i++) {
        cout << "Pessoa " << i + 1 << ": ";
        for(int j = 0; j < genes; j++) {
            if(j == genes - 1) cout << populacao[i][j];
            else cout << populacao[i][j] << ", ";
        }
        cout << endl;
    }
    
    testar_funcao(populacao, pessoas, genes);

    // Acoplando um novo individuo em populacao
    /*populacao.push_back(copulacao(populacao[rand() % pessoas], populacao[rand() % pessoas]));
    for(int j = 0; j < genes; j++) {
        if(j == genes - 1) cout << populacao[pessoas][j];
        else cout << populacao[pessoas][j] << ", ";
    }*/

    // Teste filho
    /*vector<int> filho = copulacao(populacao[rand() % pessoas], populacao[rand() % pessoas]);
    for(int i = 0; i < genes; i++) {
        if(i == genes - 1) cout << filho[i];
        else cout << filho[i] << ", ";
    }*/

    return 0;
}
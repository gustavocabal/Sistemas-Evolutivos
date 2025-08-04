#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
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

int funcao(int x, int y, int z) {
    //Constantes
    int a = 1;
    int b = 0;
    int c = 0;

    //Expoentes
    int exp1 = 2;
    int exp2 = 3;
    int exp3 = 5;

    int funcao = a*pow(x, exp1) + b*pow(y, exp2) + c*pow(z, exp3); 
    return funcao;
}

void testar_funcao(vector<vector<int>>& populacao, int pessoas, int genes) {
    int temp = 0;
    int maior_valor = temp; //Iguais pra nao dar BO no if
    int melhor;

    for(int i = 0; i < pessoas; i++) {
        temp = funcao(populacao[i][0], populacao[i][1], populacao[i][2]);
        if(i == 0 || temp > maior_valor) {
            maior_valor = temp;
            melhor = i;
        }
    }

    cout << "O maior valor é: " << maior_valor << endl;
    cout << "O melhor individuo é: " << melhor + 1 << "\nSeus genes são: " << endl;
    for(int i = 0; i < genes; i++) {
        if(i == genes - 1) cout << populacao[melhor][i];
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
    vector<vector<int>> populacao = gerar_individuos(pessoas, genes);

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
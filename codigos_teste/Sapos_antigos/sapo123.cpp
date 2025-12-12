#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

using namespace std;

vector<vector<float>> gerar_individuos(int, int);
float funcao(float, float, float);
void testar_funcao(vector<vector<float>>&, int, int);

int main() {
    int sapos, genes;
    cout << "Quantos sapos haverá inicialmente na população?" << endl;
    cin >> sapos;
    genes = 8;
    vector<vector<float>> populacao = gerar_individuos(sapos, genes);

    // Print de cada individuo
    for(int i = 0; i < sapos; i++) {
        cout << "Sapo " << i + 1 << ": ";
        for(int j = 0; j < genes; j++) {
            if(j == genes - 1) cout << populacao[i][j];
            else cout << populacao[i][j] << ", ";
        }
        cout << endl;
    }
    
    testar_funcao(populacao, sapos, genes);

    return 0;
}

vector<vector<float>> gerar_individuos(const int sapos, const int genes) {
    srand(time(0));
    vector<vector<float>> populacao;

    for(int i = 0; i < sapos; i++) {
        vector<float> sapo;
        for(int j = 0; j < genes; j++) {
            // static_cast converte o número do rand para float
            sapo.push_back(static_cast<float>(rand()) / RAND_MAX * 100);
        }
        populacao.push_back(sapo);
    }

    return move(populacao);
}

float funcao(float x, float y, float z, float z1, float z2, float z3, float z4, float z5) {
    //Constantes
    float a = 1;float b = 10;float c = 20;float c1 = 23;
    float c2 = 42;float c3 = -57;float c4 = 3;float c5 = -20;

    //Expoentes
    float exp1 = 2;float exp2 = -3;float exp3 = 1;float exp4 = 2;
    float exp5 = 7;float exp6 = -5;float exp7 = 3;float exp8 = 10;

    float func = a*pow(x, exp1) + b*pow(y, exp2) + c*pow(z, exp3) + c1*pow(z, exp4)
             + c2*pow(z, exp5) + c3*pow(z, exp6) + c4*pow(z, exp7) + c5*pow(z, exp8);

    return func;
}

void testar_funcao(vector<vector<float>>& populacao, int sapos, int genes) {
    float temp = 0;
    float maior_valor = temp; //Iguais pra nao dar BO no if
    int melhor;

    for(int i = 0; i < sapos; i++) {
        temp = funcao(populacao[i][0], populacao[i][1], populacao[i][2], populacao[i][3],
             populacao[i][4], populacao[i][5], populacao[i][6], populacao[i][7]);
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
// Teste.cpp

#include "json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>

using json = nlohmann::json;

int NumSteps = 10;
int numRows = 8;
int numCols = 8;
int matriz[8][8];

json elements = json::array();

struct Movement {
    int row;
    int col;
    int passo;
    std::string direction;
};

// ----------------------------------------------------------
void GenerateRandomTable() {

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            if ((rand() % 21) / 3 == 0)
                matriz[i][j] = rand() % 4;  
            else
                matriz[i][j] = 0;
        }
    }
}

// ----------------------------------------------------------
void SaveTable() {

    // limpa elementos antes de preencher
    // caso contrário JSON acumula dados
     elements.clear();

    for (size_t i = 0; i < numRows; i++) {
        for (size_t j = 0; j < numCols; j++) {

            if (matriz[i][j] == 0)
                continue;

            json obj;  // Criado apenas 1 vez por célula

            switch (matriz[i][j]) {
                case 1:
                    obj["type"] = "Fly";
                    obj["direction"] = "None";
                    break;
                case 2:
                    obj["type"] = "mine";
                    obj["direction"] = "None";
                    break;
                case 3:
                    obj["type"] = "Hole";
                    obj["direction"] = "None";
                    break;
                case 4:
                    obj["type"] = "Sapo";
                    obj["direction"] = populacao[MELHOR_DE_TODOS].orientacao;
                    break;
                default:
                    continue;
            }

            obj["row"] = (int)i;
            obj["col"] = (int)j;
            obj["passo"] = 0;

            elements.push_back(obj);
        }
    }
}

// ----------------------------------------------------------
void GenerateRandomMovements() {

    int x = numRows / 2;
    int y = numCols / 2;
    int passo = 0;

    int dx[4] = { -1, 0, 1, 0 };
    int dy[4] = { 0, 1, 0, -1 };
    const char* dirNames[4] = { "Front", "Right", "Back", "Left" };

    for (int i = 0; i < NumSteps; i++) {

        std::vector<int> valid;

        for (int dir = 0; dir < 4; dir++) {
            int nx = x + dx[dir];
            int ny = y + dy[dir];

            if (nx >= 0 && nx < numRows && ny >= 0 && ny < numCols)
                valid.push_back(dir);
        }

        int chosen = valid[rand() % valid.size()];

        json obj;
        obj["type"] = "Sapo";
        obj["row"] = x;
        obj["col"] = y;
        obj["passo"] = passo;
        obj["direction"] = dirNames[chosen];

        elements.push_back(obj);

        x += dx[chosen];
        y += dy[chosen];
        passo++;
    }
}

// ----------------------------------------------------------
void SaveMovements(const std::vector<Movement>& movs) {

    for (const Movement& m : movs) {
        json obj;
        obj["type"] = "Sapo";
        obj["row"] = m.row;
        obj["col"] = m.col;
        obj["passo"] = m.passo;
        obj["direction"] = m.direction;

        elements.push_back(obj);
    }
}

// ----------------------------------------------------------
int main() {
    srand(time(NULL));

    GenerateRandomTable();
    SaveTable();

    GenerateRandomMovements();

    std::vector<Movement> listaPronta = {
        {4, 4, 0, "Right"},
        {4, 5, 1, "Right"},
        {4, 6, 2, "Right"}
    };

    SaveMovements(listaPronta);

    json finalJson;
    finalJson["elements"] = elements;

    std::ofstream arquivo("Info.json");
    arquivo << finalJson.dump(4);
    arquivo.close();

    return 0;
}

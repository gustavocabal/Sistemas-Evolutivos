// Configurações
const numRows = 20;
const numCols = 20;
const gameBoard = document.getElementById("gameBoard");

// Estados locais
let board = [];

// Imagens (Seus caminhos)
const images = {
    normal: "Images/Sist_Evo_vict.png",
    mine: "Images/Bomb.png",
    Fly: "Images/Sist_Evo_mosca.png",
    Hole: "Images/Hole.png",
    SapoF: "Images/Sapo/Sist_Ev_sapo.png",
    SapoB: "Images/Sapo/Sist_Ev_sapo_Back.png",
    SapoL: "Images/Sapo/Sapo_Side_L.png",
    SapoR: "Images/Sapo/Sapo_Side_R.png",
    Boom: "Images/Sapo/Sist_Ev_sapo_Boom.png",
    Fall: "Images/Sapo/Sist_Ev_sapo_Hole.png",
    Yummy: "Images/Sapo/Sist_Ev_sapo_Yummy.png"
};

// 1. Inicializa a Matriz Lógica
function initializeBoard() {
    board = [];
    for (let i = 0; i < numRows; i++) {
        board[i] = [];
        for (let j = 0; j < numCols; j++) {
            board[i][j] = {
                type: "normal",
                image: images.normal
            };
        }
    }
}

// 2. Coloca os elementos baseados no JSON recebido
function placeElements(dataElements) {
    // Reset do tabuleiro lógico
    for (let i = 0; i < numRows; i++) {
        for (let j = 0; j < numCols; j++) {
            board[i][j].type = "normal";
            board[i][j].image = images.normal;
        }
    }

    // Processa a lista que veio do JSON
    dataElements.forEach(el => {
        const r = el.row;
        const c = el.col;

        if (r >= 0 && r < numRows && c >= 0 && c < numCols) {
            
            // Se for Mosca, Bomba ou Buraco
            if (el.type === "Fly") {
                board[r][c].type = "Fly";
                board[r][c].image = images.Fly;
            } 
            else if (el.type === "mine") {
                board[r][c].type = "mine";
                board[r][c].image = images.mine;
            }
            else if (el.type === "Hole") {
                board[r][c].type = "Hole";
                board[r][c].image = images.Hole;
            }
            // Se for o Sapo
            else if (el.type === "Sapo") {
                // Verifica colisão visual
                if (board[r][c].type === "mine") {
                    board[r][c].image = images.Boom; // Explodiu
                } else if (board[r][c].type === "Hole") {
                    board[r][c].image = images.Fall; // Caiu
                } else if (board[r][c].type === "Fly") {
                    board[r][c].image = images.Yummy; // Comeu
                } else {
                    // Sapo normal andando
                    board[r][c].type = "Sapo";
                    switch (el.direction) {
                        case "Front": board[r][c].image = images.SapoF; break;
                        case "Back":  board[r][c].image = images.SapoB; break;
                        case "Left":  board[r][c].image = images.SapoL; break;
                        case "Right": board[r][c].image = images.SapoR; break;
                        default:      board[r][c].image = images.SapoF;
                    }
                }
            }
        }
    });
}

// 3. Desenha na tela (DOM)
function renderBoard() {
    gameBoard.innerHTML = ""; // Limpa o HTML anterior

    for (let i = 0; i < numRows; i++) {
        const rowDiv = document.createElement("div");
        rowDiv.style.display = "flex"; // Garante alinhamento

        for (let j = 0; j < numCols; j++) {
            const cell = document.createElement("div");
            cell.className = "cell_norm"; // Sua classe CSS

            const img = document.createElement("img");
            img.src = board[i][j].image;
            img.className = "cell-image"; // Sua classe CSS
            
            cell.appendChild(img);
            rowDiv.appendChild(cell);
        }
        gameBoard.appendChild(rowDiv);
    }
}

// 4. Função Principal (Loop de Leitura)
function updateGame() {
    // Adiciona timestamp para evitar cache do navegador
    fetch("Info.json?t=" + Date.now())
        .then(response => {
            if (!response.ok) throw new Error("Erro leitura");
            return response.json();
        })
        .then(data => {
            // O JSON agora contém apenas o estado ATUAL do mundo
            // Não precisamos filtrar por passo, apenas desenhar o que veio.
            placeElements(data.elements);
            renderBoard();
        })
        .catch(err => {
            // Ignora erros de leitura momentâneos (enquanto C++ escreve)
            // console.warn(err); 
        });
}

// Inicializa
initializeBoard();
renderBoard();

// Atualiza a cada 100ms (10 FPS)
// O JS apenas "espelha" o que o C++ gravou no arquivo
setInterval(updateGame, 10);
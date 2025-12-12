// Script.js
const numRows = 8;
const numCols = numRows;
let passo = 0;
let passoTot = 11;
const gameBoard = document.getElementById("gameBoard");
let board = [];
let flies = [];
let holes = [];
let mines = [];
let Sapo = [];

const images = {
    normal: "Images/Sist_Evo_vict.png",
    mine: "Images/Bomb.png",
    Fly: "Images/Sist_Evo_mosca.png",
    Hole: "Images/Hole.png",
    StepD: "Images/Sapo/Sist_Ev_sapo_Back.png",
    SapoF: "Images/Sapo/Sist_Ev_sapo.png",
    SapoB: "Images/Sapo/Sist_Ev_sapo_Back.png",
    SapoL: "Images/Sapo/Sapo_Side_L.png",
    SapoR: "Images/Sapo/Sapo_Side_R.png",
    Boom: "Images/Sapo/Sist_Ev_sapo_Boom.png",
    Fall: "Images/Sapo/Sist_Ev_sapo_Hole.png",
    Yummy: "Images/Sapo/Sist_Ev_sapo_Yummy.png"
};


/* --- Funções --- */

function initializeBoard() {
    board = [];
    for (let i = 0; i < numRows; i++) {
        board[i] = [];
        for (let j = 0; j < numCols; j++) {
            board[i][j] = {
                isElement: false,
                type: "normal",
                StepD: false,
                image: images.normal
            };
        }
    }
}


function placeElements() {

    // Reset total
    for (let i = 0; i < numRows; i++) {
        for (let j = 0; j < numCols; j++) {
            board[i][j].isElement = false;
            board[i][j].type = "normal";
            board[i][j].image = images.normal;
        }
    }

    // ---- Flies ----
    flies.forEach(pos => {
        if (isValidPos(pos.row, pos.col)) {
            board[pos.row][pos.col].isElement = true;
            board[pos.row][pos.col].type = "Fly";
            board[pos.row][pos.col].image = images.Fly;
        }
    });

    // ---- Holes ----
    holes.forEach(pos => {
        if (isValidPos(pos.row, pos.col)) {
            board[pos.row][pos.col].isElement = true;
            board[pos.row][pos.col].type = "Hole";
            board[pos.row][pos.col].image = images.Hole;
        }
    });

    // ---- Mines ----
    mines.forEach(pos => {
        if (isValidPos(pos.row, pos.col)) {
            board[pos.row][pos.col].isElement = true;
            board[pos.row][pos.col].type = "mine";
            board[pos.row][pos.col].image = images.mine;
        }
    });

    // ---- Sapo ----
    Sapo.forEach(pos => {
        if (pos.passo === passo && isValidPos(pos.row, pos.col)) {
            board[pos.row][pos.col].isElement = true;

            if (board[pos.row][pos.col].type !== "Sapo" && board[pos.row][pos.col].type !== "normal") {
                // Sapo entrou em uma mina → explode
                revealCell(pos.row, pos.col);
                return; // não desenha sapo por cima
            }

            board[pos.row][pos.col].type = "Sapo";
            switch (pos.direction) {
                case "Front": board[pos.row][pos.col].image = images.SapoF; break;
                case "Back": board[pos.row][pos.col].image = images.SapoB; break;
                case "Left": board[pos.row][pos.col].image = images.SapoL; break;
                case "Right": board[pos.row][pos.col].image = images.SapoR; break;
            }
        }
    });
}


function isValidPos(r, c) {
    return r >= 0 && r < numRows && c >= 0 && c < numCols;
}

function revealCell(row, col) {
    if (!isValidPos(row, col)) return;

    board[row][col].StepD = true;

    switch (board[row][col].type) {
        case "mine":
             board[row][col].image = images.Boom;
            break;
        case "Fly":
            board[row][col].image = images.Yummy;
            break;
        case "Hole":
            board[row][col].image = images.Fall;
            break;  
        default:
        break;
    }

    renderBoard();
}

function renderBoard() {
    gameBoard.innerHTML = "";

    for (let i = 0; i < numRows; i++) {
        for (let j = 0; j < numCols; j++) {

            const cell = document.createElement("div");
            cell.className = "cell_norm";

            const img = document.createElement("img");
            img.src = board[i][j].image;
            img.className = "cell-image";
            cell.append(img);
            gameBoard.appendChild(cell);
        }
        gameBoard.appendChild(document.createElement("br"));
    }
}

/* --- NOVO: Loop de animação do sapo com delay --- */
async function animarSapo() {
    for (let i = 0; i < passoTot; i++) {
        passo = i;          // atualiza o número do passo
        placeElements();    // reposiciona o sapo conforme o passo
        renderBoard();      // renderiza o novo frame
        await new Promise(r => setTimeout(r, 1500)); // delay 1.5 s
    }
}

/* --- Fluxo --- */

initializeBoard();

// Carrega o arquivo JSON e só depois coloca elementos, calcula counts e renderiza
fetch("Info.json")
    .then(response => {
        if (!response.ok) throw new Error("HTTP error " + response.status);
        return response.json();
    })
    .then(data => {

        flies = [];
        holes = [];
        mines = [];
        Sapo = [];

        data.elements.forEach(el => {
            const pos = { row: el.row, col: el.col, passo: el.passo, direction: el.direction };
            if (!isValidPos(pos.row, pos.col)) return;

            if (el.type === "Fly") flies.push(pos);
            else if (el.type === "Hole") holes.push(pos);
            else if (el.type === "mine") mines.push(pos);
            else if (el.type === "Sapo") Sapo.push(pos);
        });

        placeElements();
        renderBoard();

        animarSapo(); // INÍCIO DA ANIMAÇÃO AUTOMÁTICA
    })
    .catch(err => {
        console.error("Erro ao carregar Info.json:", err);
        renderBoard();
    });

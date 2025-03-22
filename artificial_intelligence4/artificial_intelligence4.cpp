#include <iostream>
#include <vector>
#include <algorithm>

const char PLAYER_SYMBOL = 'X', COMPUTER_SYMBOL = 'O';
const int DIMENSION = 3;

struct Board;

struct Board {
    std::vector<std::vector<char>> board;

    Board(std::vector<std::vector<char>> new_board) {
        board = new_board;
    }

    std::vector<Board> next_moves(char turn) {
        std::vector<Board> moves;
        std::vector<std::pair<int, int>> unoccupied_positions;

        for (int i = 0; i < board.size(); i++) {
            for (int j = 0; j < board.size(); j++) {
                if (board[i][j] == ' ') {
                    unoccupied_positions.push_back(std::make_pair(i, j));
                }
            }
        }

        for (int i = 0; i < unoccupied_positions.size(); i++) {
            Board b(*this);

            int x = unoccupied_positions[i].first;
            int y = unoccupied_positions[i].second;

            b.board[x][y] = turn;
            moves.push_back(b);
        }

        return moves;
    }

    char row_winner() {

        if (board[0][0] != ' ' && board[0][0] == board[0][1] && board[0][1] == board[0][2]) {
            return board[0][0];
        }
        else if (board[1][0] != ' ' && board[1][0] == board[1][1] && board[1][1] == board[1][2]) {
            return board[1][0];
        }
        else if (board[2][0] != ' ' && board[2][0] == board[2][1] && board[2][1] == board[2][2]) {
            return board[2][0];
        }

        return ' ';
    }

    char col_winner() {

        if (board[0][0] != ' ' && board[0][0] == board[1][0] && board[1][0] == board[2][0]) {
            return board[0][0];
        }
        else if (board[0][1] != ' ' && board[0][1] == board[1][1] && board[1][1] == board[2][1]) {
            return board[0][1];
        }
        else if (board[0][2] != ' ' && board[0][2] == board[1][2] && board[1][2] == board[2][2]) {
            return board[0][2];
        }

        return ' ';
    }

    char diagonal_winner() {
        if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
            return board[0][0];
        }
        else if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
            return board[0][2];
        }

        return ' ';
    }

    int result_of_game(int depth) {
        char symbol = diagonal_winner();
        if (symbol == PLAYER_SYMBOL)return -10 - depth;
        else if (symbol == COMPUTER_SYMBOL)return 10 + depth;

        symbol = row_winner();
        if (symbol == PLAYER_SYMBOL)return -10 - depth;
        else if (symbol == COMPUTER_SYMBOL)return 10 + depth;

        symbol = col_winner();
        if (symbol == PLAYER_SYMBOL)return -10 - depth;
        else if (symbol == COMPUTER_SYMBOL)return 10 + depth;
        
        
        for (int i = 0; i < DIMENSION; i++) {
            for (int j = 0; j < DIMENSION; j++) {
                if (board[i][j] == ' ') {
                    return -2; // intermediate state
                }
            }
        }
        
        return 0; // draw
    }

    void print() {
        for (int i = 0; i < DIMENSION; i++) {
            for (int j = 0; j < DIMENSION; j++) {
                std::cout << board[i][j];

                if (j <= DIMENSION - 2) {
                    std::cout << '|';
                }
            }
            std::cout << '\n';
        }
    }


    Board(const Board& other) {
        board = other.board;    }
};

/*
int minimax(Board board, bool maximizing_player) {
    
    if (board.is_terminal_state()) {
        return board.result_of_game();
    }

    if (maximizing_player) {
        int value = INT_MIN;

        std::vector<Board> next_moves = board.next_moves(COMPUTER_SYMBOL);

        for (Board move : next_moves) {
            value = std::max(value, minimax(move, false));
        }

        return value;
    }
    else {
        int value = INT_MAX;

        std::vector<Board> next_moves = board.next_moves(PLAYER_SYMBOL);

        for (Board move : next_moves) {
            value = std::min(value, minimax(move, true));
        }

        return value;
    }
}
*/

int alpha_beta(Board board,int alpha, int beta, bool maximizing_player, int depth) {
    
    int result = board.result_of_game(depth);

    // only -2 if in intermediate state
    if (result != -2 || depth == 0) {
        return result;
    }

    if (maximizing_player) {
        int value = INT_MIN;

        std::vector<Board> next_moves = board.next_moves(COMPUTER_SYMBOL);

        for (Board move : next_moves) {
            value = std::max(value, alpha_beta(move, alpha,beta,false,depth-1));

            if (value >= beta) {
                return value;
            }
            alpha = std::max(alpha, value);
        }

        return value;
    }
    else {
        int value = INT_MAX;

        std::vector<Board> next_moves = board.next_moves(PLAYER_SYMBOL);

        for (Board move : next_moves) {
            value = std::min(value, alpha_beta(move, alpha, beta, true,depth - 1));

            if (value <= alpha) {
                return value;
            }
            beta = std::min(beta, value);
        }

        return value;
    }
}

bool is_valid_move(int x, int y, const Board& board) {
    if (x < 0 || x > 2) {
        return false;
    }
    if (y < 0 || y > 2) {
        return false;
    }

    return board.board[x][y] == ' ';
}

void tic_tac_toe(Board current_board) {

    char first;
    const int MAX_DEPTH = 6;

    std::cout << "who starts first? you (P) or computer (C)\n";
    std::cin >> first;

    if (first == 'P') {

        current_board.print();

        int board_value = current_board.result_of_game(MAX_DEPTH);

        while (board_value == -2) {

            int move_x, move_y;

            std::cout << "make a move!\n";
            std::cin >> move_x >> move_y;

            while (!is_valid_move(move_x, move_y, current_board)) {
                std::cout << "invalid move, try again\n";
                std::cin >> move_x >> move_y;
            }

            current_board.board[move_x][move_y] = PLAYER_SYMBOL;

            current_board.print();
            std::cout << '\n';

            Board best_move(current_board);
            int best_score = INT_MIN;

            std::vector<Board> next_moves = current_board.next_moves(COMPUTER_SYMBOL);

            for (Board move : next_moves) {
                int score = alpha_beta(move, INT_MIN, INT_MAX, false, MAX_DEPTH);

                if (score > best_score) {
                    best_score = score;
                    best_move = move;
                }
            }

            current_board = best_move;
            current_board.print();
            std::cout << '\n';

            board_value = current_board.result_of_game(MAX_DEPTH);
        }
    }
    else if (first == 'C') {

        int board_value = current_board.result_of_game(MAX_DEPTH);

        while (board_value == -2) {

            Board best_move(current_board);
            int best_score = INT_MIN;

            std::vector<Board> next_moves = current_board.next_moves(COMPUTER_SYMBOL);

            for (Board move : next_moves) {
                int score = alpha_beta(move, INT_MIN, INT_MAX, false, MAX_DEPTH);

                if (score > best_score) {
                    best_score = score;
                    best_move = move;
                }
            }

            current_board = best_move;
            current_board.print();
            std::cout << '\n';

            if (current_board.result_of_game(MAX_DEPTH) != -2) {
                break;
            }

            int move_x, move_y;

            std::cout << "make a move!\n";
            std::cin >> move_x >> move_y;

            while (!is_valid_move(move_x, move_y, current_board)) {
                std::cout << "invalid move, try again\n";
                std::cin >> move_x >> move_y;
            }

            current_board.board[move_x][move_y] = PLAYER_SYMBOL;

            current_board.print();
            std::cout << '\n';

            board_value = current_board.result_of_game(MAX_DEPTH);
        }
    }

    int result = current_board.result_of_game(MAX_DEPTH);

    if (result > 0) {
        std::cout << "you lost!\n";
    }
    else if (result < 0) {
        std::cout << "you won!\n";
    }
    else if (result == 0) {
        std::cout << "it's a draw!\n";
    }
}

int main() {
    std::vector<std::vector<char>> board;

    for (int i = 0; i < 3; i++) {
        board.push_back(std::vector<char>(3, ' '));
    }

    Board ttt(board);

    tic_tac_toe(ttt);

    return 0;
}
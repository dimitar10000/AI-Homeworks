#include <iostream>
#include <string>
#include <sstream>
#include <stack>
#include <vector>
#include <chrono>
#include <cmath>

std::vector<std::vector<int>> goal_board;

std::chrono::high_resolution_clock my_clock;

int get_goal_row(int element) {
	int n = goal_board.size();

	return (element - 1) / n;
}

int get_goal_col(int element) {
	int n = goal_board.size();

	return (element - 1) % n;
}

struct Board {
	std::vector<std::vector<int>> board;
	int manhattan_distance;
	std::string last_move;
	Board* parent;

	Board(std::vector<std::vector<int>> _board) {
		board = _board;
		manhattan_distance = get_manhattan_distance();
		parent = nullptr;
	}

	std::vector<Board> neighbors() {
		std::vector<Board> neighbors;
		int n = board.size();

		std::pair<int, int> coordinates = get_zero_coords();

		int x = coordinates.first;
		int y = coordinates.second;

		if (x > 0) {
			Board b(*this);
			swap_nodes(b,x, y, x - 1, y);
			b.last_move = "down";

			neighbors.push_back(b);
		}
		if (x < n - 1) {
			Board b(*this);
			swap_nodes(b,x, y, x + 1, y);
			b.last_move = "up";

			neighbors.push_back(b);
		}
		if (y > 0) {
			Board b(*this);
			swap_nodes(b,x, y, x, y-1);
			b.last_move = "right";

			neighbors.push_back(b);
		}
		if (y < n - 1) {
			Board b(*this);
			swap_nodes(b, x, y, x, y + 1);
			b.last_move = "left";

			neighbors.push_back(b);
		}

		return neighbors;
	}

	void print() {
		for (std::vector<int> row : board) {
			for (int element : row) {
				std::cout << element << ' ';
			}
			std::cout << std::endl;
		}
	}

	int cost(Board neighbor_board) {
		return std::abs(manhattan_distance - neighbor_board.manhattan_distance);
	}

	bool operator==(const Board other) {
		for (int i = 0; i < board.size(); i++) {
			for (int j = 0; j < board[i].size(); j++) {
				if (board[i][j] != other.board[i][j]) {
					return false;
				}
			}
		}

		return true;
	}

	std::pair<int,int> get_zero_coords() {
		for (int i = 0; i < board.size(); i++) {
			for (int j = 0; j < board[i].size(); j++) {
				if (board[i][j] == 0) {
					return std::make_pair(i, j);
				}
			}
		}
	}

	void swap_nodes(Board& new_board,int x1,int y1,int x2,int y2) {

		std::swap(new_board.board[x1][y1],new_board.board[x2][y2]);
		manhattan_distance = get_manhattan_distance();
	}

	int get_manhattan_distance() {
		int s = 0;

		for (int i = 0; i < board.size(); i++) {
			for (int j = 0; j < board[i].size(); j++) {
				if (board[i][j] != 0 && board[i][j] != goal_board[i][j]) {

					int goal_x = get_goal_row(board[i][j]);
					int goal_y = get_goal_col(board[i][j]);

					s += std::abs(i - goal_x) + std::abs(j - goal_y);
				}
			}
		}

		return s;
	}
};


template<typename T>
bool member_of(T element, std::stack<T> container) {
	while (!container.empty()) {
		T top = container.top();
		if (element == top) {
			return true;
		}

		container.pop();
	}

	return false;
}

std::vector<std::vector<int>> get_goal_board(int n,int pos_zero) {
	std::vector<std::vector<int>> board;

	for (int i = 0; i < n; i++) {
		board.push_back(std::vector<int>());
	}
	
	int num = 1;

	if (pos_zero == -1) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (i == n - 1 && j == n-1) {
					board[i].push_back(0);
				}
				else {
					board[i].push_back(num);
					num++;
				}
			}
		}
	}
	else {
		int x = pos_zero / n;
		int y = pos_zero % n;
		
		if (y == 0) {
			x--;
			y = n - 1;
		}
		else {
			y = y - 1;
		}
		

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (i == x && j == y) {
					board[i].push_back(0);
				}
				else {
					board[i].push_back(num);
					num++;
				}
			}
		}
	}

	return board;
}

bool solvable(Board board) {
	int inversions = 0;
	int n = board.board.size();
	int row_of_zero = board.get_zero_coords().first;

	std::vector<int> rows;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (board.board[i][j] != 0) {
				rows.push_back(board.board[i][j]);
			}
		}
	}

	for (int i = 1; i < rows.size(); i++) {
		for (int k = 0; k < i; k++) {
			if (rows[k] > rows[i]) {
				inversions++;
			}
		}
	}

	if (n % 2 == 1) {
		return inversions % 2 == 0;
	}
	return (inversions + row_of_zero) % 2 == 1;
}

int search(Board current_board, int bound, int& min,int current_cost,std::stack<Board>& solutions) {

	//std::cout << current_board.manhattan_distance << '\n';

	int f = current_board.manhattan_distance + current_cost;

	if (f > bound) {
		return f;
	}

	if (current_board.manhattan_distance == 0) {
		std::cout << "found solution\n";
		solutions.push(current_board);
		return 0;
	}

	std::vector<Board> new_boards = current_board.neighbors();

	for (Board board : new_boards) {
		if (current_board.parent == nullptr || board == *current_board.parent) {
			continue;
		}
		else {
			int tmp = search(board, bound, min,current_cost + 1, solutions);

			if (tmp == 0) {
				solutions.push(board);
				return tmp; // found goal
			}

			if (tmp < min) {
				min = tmp;
				//std::cout << "min = " << min << '\n';
			}
		}
	}

	return -1;
}


void ida_star(Board board) {

	if (!solvable(board)) {
		std::cout << "this board isn't solvable, sorry...\n";
		return;
	}

	int bound = board.manhattan_distance;
	std::stack<Board> solution_boards;

	auto t1 = my_clock.now();
	board.parent = nullptr;

	while (true) {
		std::cout << "BOUND = " << bound << '\n';

		int min = INT_MAX;

		int result = search(board,bound,min,0,solution_boards);

		if (result == 0) {
			auto t2 = my_clock.now();

			std::cout << solution_boards.size() - 1 << '\n';

			while (!solution_boards.empty()) {

				std::cout << solution_boards.top().last_move << '\n';
				solution_boards.pop();
			}

			auto diff = t2 - t1;
			auto ms_time = std::chrono::duration<double, std::milli>(diff).count();

			std::cout << "time taken to find path: " << (ms_time / 1000) << " secs\n";
			return;
		}

		bound = min;
	}
	
}

int main() {
	int blocks,n,pos_zero;
	std::cin >> blocks;
	std::cin >> pos_zero;

	n = std::sqrt(blocks + 1);

	std::vector<std::vector<int>> vec;
	std::cin.ignore();

	for (int i = 0; i < n; i++) {
		vec.push_back(std::vector<int>());
		std::string line, str;
		std::getline(std::cin, line);

		std::stringstream ss(line);

		int j = 0;
		while (ss >> str) {
			int num = std::stoi(str);
			vec[i].push_back(num);
			j++;
		}
	}

	goal_board = get_goal_board(n, pos_zero);
	
	/*
	for (int i = 0; i < goal_board.size(); i++) {
		for (int j = 0; j < goal_board[i].size(); j++) {
			std::cout << goal_board[i][j] << ' ';
		}
		std::cout << '\n';
	}
	*/

	Board board(vec);
	ida_star(board);

	return 0;
}
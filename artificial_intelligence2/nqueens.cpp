#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

std::chrono::high_resolution_clock my_clock;

// constant time
int get_conflicts_at(int num_in_row, int num_left_diag,
	int num_right_diag) {

	int conflicts = 0;
	
	
	conflicts += (num_in_row*(num_in_row-1))/2;
	conflicts += (num_left_diag*(num_left_diag-1))/2;
	conflicts += (num_right_diag*(num_right_diag-1))/2;
	

	/*
	conflicts += (num_in_row > 1) ? (num_in_row - 1) : 0;
	conflicts += (num_left_diag > 1) ? (num_left_diag - 1) : 0;
	conflicts += (num_right_diag > 1) ? (num_right_diag - 1) : 0;
	*/

	return conflicts;
}

// linear time
bool has_conflicts(std::vector<int> queens_in_row,std::vector<int> left_diagonals, std::vector<int> right_diagonals) {
	for (int i = 0; i < queens_in_row.size(); i++) {
		if (queens_in_row[i] > 1) return true;
	}
	
	for (int i = 0; i < left_diagonals.size();i++) {
		if (left_diagonals[i] > 1 || right_diagonals[i] > 1)return true;
	}

	return false;
}

void print_board(std::vector<int> board) {
	std::vector<std::vector<char>> printable;
	
	for (int i = 0; i < board.size(); i++) {
		printable.push_back(std::vector<char>(board.size(),'-'));
	}

	for (int i = 0; i < board.size(); i++) {
		int queen_row = board[i];
		printable[queen_row][i] = '*';
	}

	for (std::vector<char> row : printable) {
		for (char c : row) {
			std::cout << c << ' ';
		}
		std::cout << '\n';
	}
}

// linear time
int get_max_conflicts_col(const std::vector<int>& board, const std::vector<int>& queens_in_row,
	const std::vector<int>& left_diagonals, const std::vector<int>& right_diagonals) {
	
	int n = board.size();

	int max_conflicts = INT_MIN;
	std::vector<int> max_conflicts_cols;

	for (int i = 0; i < n; i++) {
		int queen_row = board[i];

		int left_diagonals_index = i - queen_row + n - 1;
		int right_diagonals_index = queen_row + i;
		
		int conflicts = get_conflicts_at(queens_in_row[queen_row], left_diagonals[left_diagonals_index], right_diagonals[right_diagonals_index]);

		//std::cout << conflicts << ' ';

		if (conflicts > max_conflicts) {
			max_conflicts = conflicts;
			max_conflicts_cols.clear();
			max_conflicts_cols.push_back(i);
		}
		else if (conflicts == max_conflicts) {
			max_conflicts_cols.push_back(i);
		}
	}
	//std::cout << '\n';

	// signal we have found the solution
	if (max_conflicts == 0) {
		return -1;
	}

	int random_index = rand() % max_conflicts_cols.size();

	return max_conflicts_cols[random_index];
}

void initialize_board(std::vector<int>& queen_positions, std::vector<int>& queens_in_row,
	std::vector<int>& left_diagonals,std::vector<int>& right_diagonals, int n) {
	
	for (int i = 0; i < n; i++) {
		queen_positions[i] = i;
	}

	std::random_device rd;
	std::mt19937 generator(rd());

	std::shuffle(queen_positions.begin(), queen_positions.end(), generator);

	for (int i = 0; i < n; i++) {
		int row = queen_positions[i];
		queens_in_row[row]++;

		int left_diag_index = i - row + n - 1;
		int right_diag_index = row + i;

		left_diagonals[left_diag_index]++;
		right_diagonals[right_diag_index]++;
	}
}

// linear time
int get_min_conflicts_row(const std::vector<int>& queen_positions,
	const std::vector<int>& queens_in_row, const std::vector<int>& left_diagonals,
	const std::vector<int>& right_diagonals, const int col_max_conflicts) {
	
	int n = queen_positions.size();
	std::vector<int> min_conflicts_rows;

	int original_row = queen_positions[col_max_conflicts];
	
	int min_conflicts = INT_MAX;

	for (int row = 0; row < n; row++) {

		int conflicts_at_row = 0;

		int left_diagonals_index = col_max_conflicts - row + n - 1;
		int right_diagonals_index = row + col_max_conflicts;

		if (row != original_row) {
			conflicts_at_row = get_conflicts_at(queens_in_row[row] + 1, left_diagonals[left_diagonals_index] + 1, right_diagonals[right_diagonals_index] + 1);
		}
		else {
			conflicts_at_row = get_conflicts_at(queens_in_row[row], left_diagonals[left_diagonals_index], right_diagonals[right_diagonals_index]);
		}
			
		if (conflicts_at_row < min_conflicts) {
			min_conflicts = conflicts_at_row;
			min_conflicts_rows.clear();
			min_conflicts_rows.push_back(row);
		}
		else if (conflicts_at_row == min_conflicts) {
			min_conflicts_rows.push_back(row);
		}
	}
	
	int random_index = rand() % min_conflicts_rows.size();

	return min_conflicts_rows[random_index];
}

std::vector<int> solution;

void n_queens(int n) {
	std::vector<int> queen_positions(n); 
	std::vector<int> queens_in_row(n);
	std::vector<int> left_diagonals(2 * n - 1);
	std::vector<int> right_diagonals(2 * n - 1);

	initialize_board(queen_positions, queens_in_row, left_diagonals, right_diagonals,n);

	bool found_solution = false;
	int iterations = 0;

	while (++iterations <= n) {

		int max_conflicts_col = get_max_conflicts_col(queen_positions, queens_in_row, left_diagonals, right_diagonals);

		// is only -1 if max conflicts were 0 at the column
		if (max_conflicts_col == -1) {
			found_solution = true;
			break;
		}
		
		int row_min_conflicts = get_min_conflicts_row(queen_positions, queens_in_row, left_diagonals, right_diagonals, max_conflicts_col);

		int previous_row = queen_positions[max_conflicts_col];

		queen_positions[max_conflicts_col] = row_min_conflicts;

		queens_in_row[previous_row]--;
		queens_in_row[row_min_conflicts]++;

		int left_diagonals_index = max_conflicts_col - previous_row + n - 1;
		int right_diagonals_index = max_conflicts_col + previous_row;

		left_diagonals[left_diagonals_index]--;
		right_diagonals[right_diagonals_index]--;

		left_diagonals_index = max_conflicts_col - row_min_conflicts + n - 1;
		right_diagonals_index = max_conflicts_col + row_min_conflicts;

		left_diagonals[left_diagonals_index]++;
		right_diagonals[right_diagonals_index]++;

		/*
		for (int i = 0; i < n; i++) {
			std::cout << "col " << i << ": " << queen_positions[i] << '\n';
		}
		std::cout << '\n';
		*/

		//print_board(queen_positions);
		
	}

	// we have exited because of too much iterations
	if (!found_solution) {
		std::cout << "restarting\n";
		n_queens(n);
	}
	else {
		std::cout << iterations << '\n';
		solution = queen_positions;
	}
}

int main() {
	int n;
	std::cin >> n;

	auto t1 = my_clock.now();
	n_queens(n);
	auto t2 = my_clock.now();

	auto diff = t2 - t1;
	auto ms_time = std::chrono::duration<double, std::milli>(diff).count();

	std::cout << "found solution in " << (ms_time / 1000) << "s\n";

	//print_board(solution);

	return 0;
}
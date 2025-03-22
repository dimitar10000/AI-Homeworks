#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <unordered_map>
#include <iomanip>

struct Table {
	std::vector<std::vector<std::string>> content;
	std::vector<std::string> col_names;

	Table() {

	}

	Table(std::vector<std::vector<std::string>> info) {
		content = info;
	}

	int size() {
		return content.size();
	}

	int features() {
		return content[0].size()-1;
	}

	void print() {
		for (int i = 0; i < col_names.size(); i++) {
			std::cout << std::setw(15) << col_names[i];
		}
		std::cout << '\n';

		for (int i = 0; i < content.size(); i++) {
			if (content[i][0] == "recurrence-events") {
				std::cout << std::setw(20) << content[i][0];
			}
			else {
				std::cout << std::setw(14) << content[i][0];
			}

			for (int j = 1; j < content[i].size(); j++) {
				std::cout << std::setw(14) << content[i][j];
			}
			std::cout << '\n';
		}
	}

	Table operator+(const Table& other) {
		Table new_table = *this;

		for (int i = 0; i < other.content.size(); i++) {
			new_table.content.push_back(other.content[i]);
		}

		return new_table;
	}

	int index_col_by_name(std::string name) {
		for (int i = 0; i < col_names.size(); i++) {
			if (col_names[i] == name) {
				return i;
			}
		}
	}

	void filter_by_value(int col_index,std::string value) {
		std::vector<std::vector<std::string>> new_table;

		for (int i = 0; i < size(); i++) {
			if (content[i][col_index] == value) {
				content[i].erase(content[i].begin() + col_index);
				new_table.push_back(content[i]);
			}
		}

		col_names.erase(col_names.begin() + col_index);

		content = new_table;
	}

	std::vector<std::string> get_values_of(std::string feature) {
		int index;
		
		for (int i = 0; i < col_names.size(); i++) {
			if (feature == col_names[i]) {
				index = i;
				break;
			}
		}

		std::vector<std::string> values;

		for (int i = 0; i < size(); i++) {
			if (std::find(values.begin(), values.end(), content[i][index]) == values.end()) {
				values.push_back(content[i][index]);
			}
		}

		return values;
	}

	double get_target_entropy() {
		int n = size();

		int class1 = 0;
		int class2 = 0;

		int class_index = index_col_by_name("class");

		for (int i = 0; i < n; i++) {
			if (content[i][class_index] == "no-recurrence-events") {
				class1++;
			}
			else {
				class2++;
			}
		}

		if (class1 == 0 || class2 == 0) {
			return 0;
		}

		double prob1 = (double)class1 / n;
		double prob2 = (double)class2 / n;

		return -prob1 * (log(prob1) / log(2)) - prob2 * (log(prob2) / log(2));
	}

	double get_double_entropy(int num1,int num2) {
		if (num1==0 || num2 == 0) {
			return 0;
		}
		
		double prob1 = (double)num1 / (double)(num1 + num2);
		double prob2 = (double)num2 / (double)(num1 + num2);

		return -prob1 * (log(prob1)/log(2)) - prob2 * (log(prob2) / log(2));
	}

	// takes class column and another one by index
	void split_on_attribute(int index) {
		std::vector<std::vector<std::string>> new_table;

		int n = size();

		int class_index = index_col_by_name("class");

		for (int i = 0; i < n; i++) {
			new_table.push_back(std::vector<std::string>());

			new_table[i].push_back(content[i][class_index]);
			new_table[i].push_back(content[i][index]);
		}

		content = new_table;
	}

	// used on split table
	double get_mixed_entropy() {
		std::unordered_map<std::string,std::pair<int,int>> dict;

		int n = size();

		for (int i = 0; i < n; i++) {
			if (dict.count(content[i][1]) == 0) {
				dict.insert({ content[i][1], std::make_pair(0,0) });
			}
			
				auto& element = dict[content[i][1]];

				if (content[i][0] == "no-recurrence-events") {
					element.first++;
				}
				else {
					element.second++;
				}
		}

		double sum = 0;

		for (auto kv : dict) {
			auto key = kv.first;
			auto value = dict.at(key);

			double prob_value = (double)(value.first + value.second) / (double)n;
			double entropy = get_double_entropy(value.first, value.second);

			sum += prob_value * entropy;
		}

		return sum;
	}

	std::string get_dominant_class() {
		int index = index_col_by_name("class");

		int counter1 = 0;
		int counter2 = 0;

		for (int i = 0; i < content.size(); i++) {
			if (content[i][index] == "no-recurrence-events") {
				counter1++;
			}
			else {
				counter2++;
			}
		}

		return (counter1 > counter2) ? "no-recurrence-events" : "recurrence-events";
	}
};

struct Tree {
	std::string root;
	std::vector<Tree*> children;

	Tree(std::string value = "") {
		root = value;
	}

	Tree(std::string value, std::vector<Tree*> desc) {
		if (value.empty()) {
			value = "";
		}
		else {
			root = value;
		}
		children = desc;
	}

	int size() {
		if (root == "") {
			return 0;
		}

		if (children.empty()) {
			return 1;
		}

		int s = 0;

		for (int i = 0; i < children.size(); i++) {
			int nodes = children[i]->size();
			s += nodes;
		}

		return 1 + s;
	}

	int height() {
		if (root == "") {
			return 0;
		}

		if (children.empty()) {
			return 1;
		}

		int max = INT_MIN;

		for (int i = 0; i < children.size(); i++) {
			int child_height = children[i]->height();
			
			if (child_height > max) {
				max = child_height;
			}
		}

		return 1 + max;
	}

	void print() {
		std::cout << "root " << root << '\n';
		if (!children.empty()) {

			std::cout << "children ";

			for (int i = 0; i < children.size(); i++) {
				std::cout << children[i]->root << ' ';
			}
			std::cout << '\n';
		}

		for (int i = 0; i < children.size(); i++) {
			children[i]->print();
		}
	}
};

std::vector<std::vector<std::string>> table;
Table breast_data(table);

Tree* build_decision_tree(Table current_table,std::string split_by) {
	/*
	std::cout << "current table\n";
	current_table.print();
	std::cout << '\n';
	*/

	double target_entropy = current_table.get_target_entropy();

	//std::cout << target_entropy << '\n';

	// leaf node
	if (target_entropy == 0) {

		int index = current_table.index_col_by_name("class");

		std::string result = current_table.content[0][index];
		Tree* new_node = new Tree(result);

		return new_node;
	}

	std::vector<double> info_gains;

	for (int i = 1; i <= current_table.features(); i++) {
		Table copy = current_table;
		copy.split_on_attribute(i);

		double mixed_entropy = copy.get_mixed_entropy();
		info_gains.push_back(target_entropy - mixed_entropy);
	}

	double max_entropy = info_gains[0];
	int max_index = 1;

	for (int i = 0; i < info_gains.size(); i++) {
		if (info_gains[i] > max_entropy) {
			max_entropy = info_gains[i];
			max_index = i+1;
		}
	}

	// save best attribute
	std::string best_attribute = current_table.col_names[max_index];

	Tree* decision_tree = new Tree(best_attribute);
	std::vector<Tree*> children;

	std::vector<std::string> values = current_table.get_values_of(best_attribute);

	for (int i = 0; i < values.size(); i++) {
		Tree* tree_ptr = new Tree(values[i]);
		children.push_back(tree_ptr);
	}

	decision_tree->children = children;

	for (int i = 0; i < decision_tree->children.size(); i++) {
		Table copy = current_table;

		copy.filter_by_value(max_index, decision_tree->children[i]->root);

		double new_target_entropy = copy.get_target_entropy();

		if (copy.size() == 0 || new_target_entropy == 1 || new_target_entropy == target_entropy) {
			std::string result = current_table.get_dominant_class();
			Tree* single_node = new Tree(result);

			decision_tree->children[i]->children.push_back(single_node);
		}
		else {
			decision_tree->children[i]->children.push_back(build_decision_tree(copy, decision_tree->children[i]->root));
		}
	}

	return decision_tree;
}

double validate(Tree* tree, Table test_set) {
	int correct = 0;
	int n = test_set.size();

	Tree attribute_node;
	std::string tree_value;

	for (int i = 0; i < n; i++) {
		attribute_node = *tree;	
		Table current_table = breast_data;

		bool found_leaf = false;
		bool deadline = false;

		while (!found_leaf) {
			int j = 0;

			while (j < attribute_node.children.size()) {
				int col_attribute = test_set.index_col_by_name(attribute_node.root);
				std::string table_value = test_set.content[i][col_attribute];

				tree_value = attribute_node.children[j]->root;

				//std::cout << "tree " << tree_value << '\n';
				//std::cout << "table " << table_value << '\n';

				if (tree_value == table_value) {
					int col_index = current_table.index_col_by_name(attribute_node.root);
					current_table.filter_by_value(col_index,table_value);

					// get next attribute to sort by
					attribute_node = *(attribute_node.children[j]->children[0]);

					if (attribute_node.root == "no-recurrence-events" ||
						attribute_node.root == "recurrence-events") {
						found_leaf = true;

						//std::cout << "found leaf\n";
						break;
					}

					//std::cout << "switching to node " << attribute_node.root << '\n';

					j = 0;
				}
				else {
					j++;
				}
			}

			if (!found_leaf) {
				deadline = true;
				break;
			}
		}

		int class_col = test_set.index_col_by_name("class");

		if (deadline) {
			std::string dominant_class = current_table.get_dominant_class();

			if (test_set.content[i][class_col] == dominant_class) {
				correct++;
			}
		}
		else if (test_set.content[i][class_col] == attribute_node.root) {
			correct++;
		}
	}

	std::cout << "correct " << correct << " all " << n << '\n';
	std::cout << "accuracy " << (double)correct / n << "\n\n";

	return (double)correct / n;
}

void parse_input(Table& table, std::ifstream& reader) {
	table.col_names.push_back("class");
	table.col_names.push_back("age");
	table.col_names.push_back("menopause");
	table.col_names.push_back("tumor-size");
	table.col_names.push_back("inv-nodes");
	table.col_names.push_back("node-caps");
	table.col_names.push_back("deg-malig");
	table.col_names.push_back("breast");
	table.col_names.push_back("breast-quad");
	table.col_names.push_back("irradiat");

	std::string row;

	int i = 0;

	while (std::getline(reader, row)) {
		if (row.find('?') != std::string::npos) {
			continue;
		}
		
		std::string delim = ",";

		table.content.push_back(std::vector<std::string>());

		auto start = 0;
		auto end = row.find(delim);

		while (end != std::string::npos) {
			table.content[i].push_back(row.substr(start, end - start));
			start = end + delim.length();
			end = row.find(delim, start);
		}

		table.content[i].push_back(row.substr(start, end - start));

		i++;
	}
	
}

void partition(int k,std::vector<Table>& partitions) {
	
	for (int i = 0; i < 10; i++) {
		Table part;
		part.col_names = breast_data.col_names;

		partitions.push_back(part);
	}

	int num_in_set = k / 9;
	int remainder = breast_data.size() - 9 * num_in_set;

	Table breast_copy = breast_data;

	std::random_device rd;
	std::mt19937 generator(rd());

	std::shuffle(breast_copy.content.begin(), breast_copy.content.end(), generator);

	int counter = 0;

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < num_in_set; j++) {
			partitions[i].content.push_back(breast_copy.content[counter]);
			counter++;
		}
	}

	for (int i = 0; i < remainder; i++) {
		partitions[9].content.push_back(breast_copy.content[counter]);
		counter++;
	}
}

Table merge_tables(std::vector<Table>& partitions,int index) {
	
	Table whole_table;
	whole_table.col_names = breast_data.col_names;

	for (int i = 0; i < partitions.size(); i++) {
		if (i != index) {
			whole_table = whole_table + partitions[i];
		}
	}

	return whole_table;
}

void cross_validate(std::vector<Table>& partitions) {
	Table training_set;
	Table test_set;

	double sum = 0;

	for (int i = 0; i < 10; i++) {
		test_set = partitions[i];
		training_set = merge_tables(partitions, i);

		Tree* dec_tree = build_decision_tree(training_set,"class");

		/*
		dec_tree->print();
		std::cout << '\n';
		*/

		sum += validate(dec_tree, test_set);
	}

	std::cout << "average accuracy " << (sum / 10.0) << '\n';
}

int main() {
	std::ifstream reader("C:\\Users\\offline\\Desktop\\breast-cancer.txt");
	parse_input(breast_data, reader);

	// original size for training set: 243

	std::vector<Table> partitions;
	partition(200, partitions);

	cross_validate(partitions);
}
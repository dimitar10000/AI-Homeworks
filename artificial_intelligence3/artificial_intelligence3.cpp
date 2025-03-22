#include <iostream>
#include <random>

std::vector<std::pair<int, int>> weights_values;
int capacity;

const int POPULATION_SIZE = 10000;
const int DESCENDANTS = 4000;
const double REPRODUCTION_RATE = 0.3; // not used
const double MUTATION_RATE = 0.1;
const double CROSSOVER_RATE = 1;

struct Individual {
	std::vector<int> genes;
	int fitness;

	Individual() {
		fitness = 0;
	}

	Individual(std::vector<int> new_genes, int new_fitness = 0) {
		genes = new_genes;
		fitness = new_fitness;
	}

	void print() const {
		for (int i = 0; i < genes.size(); i++) {
			std::cout << genes[i] << ' ';
		}
		std::cout << fitness << '\n';
	}
};

void print_population(const std::vector<Individual>& population) {
	for (int i = 0; i < population.size(); i++) {
		population[i].print();
	}
}

std::vector<Individual> get_population(int n,int m) {
	std::vector<Individual> population;
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, 1);

	for (int i = 0; i < n; i++) {
		std::vector<int> genes_array;
		for (int j = 0; j < m; j++) {
			genes_array.push_back(distr(gen));
		}

		population.push_back(Individual(genes_array));
	}

	return population;
}

void update_fitness(std::vector<Individual>& population) {

	int length_chromosome = population[0].genes.size();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, length_chromosome-1);

	for (int i = 0; i < population.size(); i++) {
		int value = 0, weight = 0;

		for (int j = 0; j < population[i].genes.size(); j++) {
			value += population[i].genes[j] * weights_values[j].second;
			weight += population[i].genes[j] * weights_values[j].first;
		}

		while (weight > capacity) {
			int random_index = distr(gen);

			if (population[i].genes[random_index] == 1) {
				population[i].genes[random_index] = 0;
				value = value - weights_values[random_index].second;
				weight = weight - weights_values[random_index].first;
			}
		}
		population[i].fitness = value;
	}
}


std::vector<Individual> selection(std::vector<Individual> population, const int num_to_select) {

	std::vector<Individual> parents;

	std::sort(population.begin(), population.end(), [](const Individual& i1, const Individual& i2) {
		return i1.fitness > i2.fitness;
		});

	for (int i = 0; i < num_to_select; i++) {
		parents.push_back(population[i]);
	}

	return parents;
}

void crossover(std::vector<Individual>& parents) {
	
	int n = parents[0].genes.size();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distr(0, 1);
	std::uniform_int_distribution<> distr2(0, n-1);

	for (int i = 0; i < parents.size(); i = i + 2) {
		if (distr(gen) < CROSSOVER_RATE) {

			int random_index = distr2(gen);

			std::swap_ranges(parents[i].genes.begin()+random_index, parents[i].genes.end(), parents[i + 1].genes.begin() + random_index);
		}
	}

	update_fitness(parents);
}

void mutation(std::vector<Individual>& population) {
	
	int n = population[0].genes.size();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distr(0, 1);

	std::uniform_int_distribution<> distr2(0, n-1);

	/*
	for (int i = 0; i < population.size(); i++) {
		if (distr(gen) < MUTATION_RATE) {

			int index = distr2(gen);

			if (population[i].genes[index] == 1) {
				for (int k = index; k >= 0; k--) {
					if (population[k].genes[index] == 0) {
						population[k].genes[index] = 1;
						break;
					}
				}
			}
		}
	}
	*/

	
	for (int i = 0; i < population.size(); i++) {
		if (distr(gen) < MUTATION_RATE) {

			int index1 = distr2(gen);
			int index2 = distr2(gen);

			std::swap(population[i].genes[index1], population[i].genes[index2]);
		}
	}

	update_fitness(population);
}

std::vector<Individual> next_generation(std::vector<Individual> population, const int num_descendants) {

	int original_size = population.size();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distr(0, 1);

	// choose the fittest parents
	std::vector<Individual> parents = selection(population,num_descendants);
	std::vector<Individual> descendants = parents;

	crossover(descendants);

	for (int i = 0; i < num_descendants; i++) {
		population.push_back(descendants[i]);
	}

	mutation(population);

	std::sort(population.begin(), population.end(), [](const Individual& i1, const Individual& i2) {
		return i1.fitness > i2.fitness;
		});

	population.resize(original_size);

	return population;
}

double average_fitness(std::vector<Individual> population) {
	double sum = 0;

	for (int i = 0; i < population.size(); i++) {
		sum += population[i].fitness;
	}

	return sum / population.size();
}

Individual knapsack(std::vector<Individual> population) {
	double prev_fitness = average_fitness(population);

	const int ITERATIONS = 1000;
	const int COUNTER_MAX = 50;
	const int PRINT_EVERY = 5;

	int counter = 0;

	std::sort(population.begin(), population.end(), [](const Individual& i1, const Individual& i2) {
		return i1.fitness > i2.fitness;
		});

	std::cout << population[0].fitness << '\n';

	for (int i = 0; i < ITERATIONS; i++) {
		population = next_generation(population,DESCENDANTS);
		double fitness = average_fitness(population);

		if (i % PRINT_EVERY == 0) {
			std::cout << population[0].fitness << '\n';
		}

		if (fitness == prev_fitness) {
			counter++;
		}
		else {
			counter = 0;
		}

		if (counter == COUNTER_MAX) {
			std::cout << "terminating early after " << (i + 1) << " iterations\n";
			break;
		}

		prev_fitness = fitness;
	}

	std::sort(population.begin(), population.end(), [](const Individual& i1, const Individual& i2) {
		return i1.fitness > i2.fitness;
		});

	return population[0];
}

int main() {
	int m, n;

	std::cin >> m >> n;

	for (int i = 0; i < n; i++) {
		int weight, value;

		std::cin >> weight >> value;

		weights_values.push_back(std::make_pair(weight, value));
	}

	// sorting values descending
	std::sort(weights_values.begin(), weights_values.end(), [](const std::pair<int,int>& p1, const std::pair<int, int>& p2) {
		return p1.second > p2.second;
		});

	capacity = m;

	std::vector<Individual> my_population = get_population(POPULATION_SIZE, n);
	update_fitness(my_population);
	//print_population(my_population);

	Individual solution = knapsack(my_population);

	solution.print();
	std::cout << '\n';
}
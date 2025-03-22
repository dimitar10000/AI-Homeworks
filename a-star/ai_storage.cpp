#include <iostream>
#include <vector>
#include <unordered_map>

struct Node {
	std::string name;
	int straight_distance;

	Node(std::string vertex,int distance) {
		name = vertex;
		straight_distance = distance;
	}
};

struct Edge {
	Node* start;
	Node* end;
	int weight;

	Edge(std::string n1, int d1, std::string n2,int d2,int w) {
		start = new Node(n1, d1);
		end = new Node(n2, d2);
		weight = w;
	}
};


class Graph {
	std::vector<Edge*> edges;
	std::vector<Node*> vertices;

	void add_vertex(std::string vertex, int distance) {
		// check if vertex is already in list
		for (Node* node : vertices) {
			if (node->name == vertex) return;
		}

		Node* n = new Node(vertex, distance);
		vertices.push_back(n);
	}

public:
	Graph(std::string vertex, int distance) {
		Node* n = new Node(vertex, distance);
		add_vertex(vertex,distance);
	};

	void add_edges(std::vector<Edge*> _edges) {

		for (Edge* item : _edges) {
			add_vertex(item->start->name,item->start->straight_distance);
			add_vertex(item->end->name, item->end->straight_distance);

			Edge* reverse = new Edge(item->end->name, item->end->straight_distance, item->start->name, item->start->straight_distance, item->weight);

			edges.push_back(item);
			edges.push_back(reverse);
		}
	}

	std::vector<Node*> get_neighbors(std::string vertex_name) {
		std::vector<Node*> neighbors;

		for (Edge* e : edges) {
			if (e->start->name == vertex_name) {
				neighbors.push_back(e->end);
			}
		}

		return neighbors;
	}


	void print() {
		for (Node* n : vertices) {
			std::cout << '(' << n->name << ", " << n->straight_distance << ')' << std::endl;
			std::cout << "neighbors: ";

			std::vector<Node*> neighbors = get_neighbors(n->name);

			for (Node* neighbor : neighbors) {
				Edge* edge = get_edge(n->name, neighbor->name);
				int d = edge->weight;

				std::cout << '(' << neighbor->name << ", " << neighbor->straight_distance << ", " <<
				   d << ") ";
			}
			std::cout << std::endl;
		}
	}

	Node* get_vertex(std::string name) {
		for (Node* n : vertices) {
			if (n->name == name) {
				return n;
			}
		}
	}

	Edge* get_edge(std::string n1, std::string n2) {
		for (Edge* edge : edges) {
			if (edge->start->name == n1 && edge->end->name == n2) {
				return edge;
			}
		}
	}

};

template <typename T, typename V>
struct PriorityQueue {
	std::vector<std::pair<T, V>> queue;

	PriorityQueue() {}

	void push(T first, V second) {

		for (int i = 0; i < queue.size(); i++) {
			if (second <= queue.at(i).second) {
				queue.insert(queue.begin() + i, std::make_pair(first, second));
				return;
			}
		}

		queue.push_back(std::make_pair(first, second));
	}
};


std::vector<std::string> a_star(Graph g,std::string start, PriorityQueue<std::string,int> pq) {

	pq.push(start,0);
	std::unordered_map<std::string, std::string> came_from;
	std::unordered_map<std::string, int> cost_so_far;

	came_from[start] = start;
	cost_so_far[start] = 0;

	std::vector<std::string> visited;

	while (!pq.queue.empty()) {
		std::string current_node = pq.queue.front().first;
		visited.push_back(current_node);
		pq.queue.erase(pq.queue.begin());

		if (current_node == "G")break;

		std::vector<Node*> neighbors = g.get_neighbors(current_node);

		for (Node* neighbor : neighbors) {
			if (std::find(visited.begin(), visited.end(), neighbor->name) != visited.end())
				continue;

			Edge* edge = g.get_edge(current_node, neighbor->name);
			int new_cost = cost_so_far[current_node] + edge->weight;

			if (cost_so_far.find(neighbor->name) == cost_so_far.end() || new_cost < cost_so_far[neighbor->name]) {
				cost_so_far[neighbor->name] = new_cost;
			}

			int priority = new_cost + neighbor->straight_distance;
			pq.push(neighbor->name, priority);
			came_from[neighbor->name] = current_node;
		}
	}

	std::vector<std::string> path;
	path.push_back("G");

	while (path.front() != "S") {
		std::string prev = came_from[path.front()];
		path.insert(path.begin(), prev);
	}

	return path;
}


int main() {
    
    return 0;
}
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <limits>
#include <queue>
#include <algorithm>
#include <cmath>
#include <stdio.h> 
#include <time.h> 
#include <unordered_set>

using namespace std;

// Узел графа
struct Node {
    string lon, lat;  // Долгота и широта хранятся как строки (O(1) по памяти на каждую строку)
    vector<pair<Node*, double>> neighbors;  // Соседи узла и вес ребра до них (O(E) по памяти для всех соседей)
};

struct Graph {
    vector<Node*> nodes;    // Вектор всех узлов графа (O(V) по памяти)
    unordered_map<string, Node*> nodeMap; // Хэш-тфблица для быстрого поиска узлов по строковому ключу (О(V) по памяти)

    Graph(const string& filename) {
        loadGraphFromFile(filename);    // O(V + E) по времени, где V - количество узлов, E - количество рёбер
    }

    void loadGraphFromFile(const string& filename) {
        ifstream file(filename);    // Открытие файла (O(1))
        if (!file.is_open()) {
            cerr << "Ошибка открытия файла: " << filename << endl; // O(1)
            return;
        }

        string line;
        while (getline(file, line)) {   // O(V + E) 
            stringstream ss(line);
            string token;

            // Парсинг родительского узла
            getline(ss, token, ':');
            string parentKey = token;  // Ключ - исходная строка координат (O(1))
            Node* parentNode;

            if (nodeMap.find(parentKey) == nodeMap.end()) { // O(1) на поиск в хеш-таблице
                stringstream parentCoordSS(token);
                getline(parentCoordSS, token, ',');
                string parentLon = token;   // O(1)
                getline(parentCoordSS, token, ',');
                string parentLat = token;   // O(1)

                parentNode = new Node{parentLon, parentLat};    // O(1)
                nodes.push_back(parentNode);    // O(1)
                nodeMap[parentKey] = parentNode;    // O(1)
            } else {
                parentNode = nodeMap[parentKey];    // O(1)
            }

            // Парсинг дочерних узлов и ребер
            while (getline(ss, token, ';')) {
                stringstream childSS(token);

                getline(childSS, token, ',');
                string childLon = token;    // O(1)
                getline(childSS, token, ',');
                string childLat = token;    // O(1)
                string childKey = childLon + "," + childLat; // Ключ - строка координат ( O(1))                
                getline(childSS, token, ','); //теперь токен это weight (O(1))
                double weight = stod(token);    // O(1)

                Node* childNode;
                if (nodeMap.find(childKey) == nodeMap.end()) {  // O(1)
                    childNode = new Node{childLon, childLat};    // O(1)
                    nodes.push_back(childNode);   // O(1)
                    nodeMap[childKey] = childNode;   // O(1)
                } else {
                    childNode = nodeMap[childKey];  // O(1)
                }

                parentNode->neighbors.push_back({childNode, weight});   // O(1)
            }
        }
        file.close();   // O(1)
    }
        // Поиск ближайшего узла к заданным координатам
    Node* findClosestNode(double lat, double lon) const {
        Node* closestNode = nullptr; // O(1)
        double minDistance = numeric_limits<double>::infinity(); // O(1)

        for (auto* node : nodes) { // O(V)
            double nodeLat = stod(node->lat); // O(1)
            double nodeLon = stod(node->lon); // O(1)
            double distance = sqrt(pow(lat - nodeLat, 2) + pow(lon - nodeLon, 2)); // O(1)

            if (distance < minDistance) { // O(1)
                minDistance = distance; // O(1)
                closestNode = node; // O(1)
            }
        }
        return closestNode; // O(1)
    }
};
// Алгоритм Дейкстры 
// Асимптотика: O(E + VlogV)
unordered_map<Node*, double> dijkstra(const vector<Node*>& nodes, Node* startNode) {
    unordered_map<Node*, double> distances;
    priority_queue<pair<double, Node*>, vector<pair<double, Node*> >, greater<pair<double, Node*> > > pq;
    for (Node* node : nodes) {
        distances[node] = numeric_limits<double>::infinity(); // инициализируем расстояния до каждого узла как бесконечность
    }
    distances[startNode] = 0.0; // расстояние до стартового узла равно 0
    pq.push({0.0, startNode}); // добавляем стартовый узел в очередь
    while (!pq.empty()) {
        double currentDist = pq.top().first; // берем расстояние до текущего узла
        Node* currentNode = pq.top().second; // берем текущий узел
        pq.pop(); // убираем его из очереди
        if (currentDist > distances[currentNode]) {
            continue; // если расстояние до текущего узла больше, чем уже известное, пропускаем
        }

        for (const auto& neighbor : currentNode->neighbors) {
            Node* nextNode = neighbor.first;
            double weight = neighbor.second;
            double newDist = currentDist + weight;

            if (newDist < distances[nextNode]) {
                distances[nextNode] = newDist;
                pq.push({newDist, nextNode});
            }
        }
    }

    return distances;
}

// Эвристическая функция для алгоритма A*
// Асимптотика: O(1)
double heuristic(Node* a, Node* b) {
    double lat1 = stod(a->lat), lon1 = stod(a->lon); // конвертируем координаты в double
    double lat2 = stod(b->lat), lon2 = stod(b->lon);
    return sqrt(pow(lat1 - lat2, 2) + pow(lon1 - lon2, 2)); // возвращаем расстояние между двумя узлами
}
// Алгоритм A*
// Асимптотика: O(E + VlogV)
unordered_map<Node*, double> a_star(const vector<Node*>& nodes, Node* startNode, Node* endNode) {
    unordered_map<Node*, double> distances;
    unordered_map<Node*, double> f_score;
    priority_queue<pair<double, Node*>, vector<pair<double, Node*> >, greater<pair<double, Node*> > > queue;
    for (Node* node : nodes) {
        distances[node] = numeric_limits<double>::infinity();
        f_score[node] = numeric_limits<double>::infinity();

    }
    distances[startNode] = 0.0;
    f_score[startNode] = heuristic(startNode, endNode);
    queue.push({f_score[startNode], startNode});
    while (!queue.empty()) {
        Node* current = queue.top().second;
        queue.pop();
        if (current == endNode) {
            return distances;
        }
        for (const auto& neighbor_pair : current->neighbors) {
            Node* neighbor = neighbor_pair.first;
            double weight = neighbor_pair.second;
            double tentative_gScore = distances[current] + weight;
            if(tentative_gScore < distances[neighbor]){
                distances[neighbor] = tentative_gScore;
                f_score[neighbor] = tentative_gScore + heuristic(neighbor, endNode);
                 queue.push({f_score[neighbor], neighbor});
            }
        }
    }
     return distances;
}

// Алгоритм BFS
// Асимптотика: O(E + V)
double bfs(const vector<Node*>& nodes, Node* startNode, Node* endNode) {
    queue<pair<Node*, double>> q;
    unordered_set<Node*> visited;
    q.push({startNode, 0.0});
    visited.insert(startNode);
    while (!q.empty()) {
        Node* current = q.front().first;
        double currentDistance = q.front().second;
        q.pop();
        if (current == endNode) {
            return currentDistance;
        }
        for (const auto& neighbor_pair : current->neighbors) {
            Node* neighbor = neighbor_pair.first;
            double weight = neighbor_pair.second;

            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.push({neighbor, currentDistance + weight});
            }
        }
    }

    return -1.0;
}
// Алгоритм DFS
// Асимптотика: O(E + V)
double dfs(const vector<Node*>& nodes, Node* startNode, Node* endNode, unordered_set<Node*>& visited) {
    visited.insert(startNode);
    if (startNode == endNode) {
        return 0.0;
    }
    for (const auto& neighbor_pair : startNode->neighbors) {
        Node* neighbor = neighbor_pair.first;
        double weight = neighbor_pair.second;
        if (visited.find(neighbor) == visited.end()) {
            double result = dfs(nodes, neighbor, endNode, visited);
            if (result != -1.0) {
                return result + weight;
            }
        }
    }
    return -1.0;
}
int main() {
    Graph graph("spb_graph.txt");
    unordered_set<Node*> visited_dfs;
    double mylon = 30.4092608; 
    double mylat = 59.9401823; 
    double finlon = 30.204761;
    double finlat = 60.006301;

    Node* closest_node = graph.findClosestNode(mylat, mylon); // Ближайшая стартовая вершина 
    Node* finish_node = graph.findClosestNode(finlat, finlon); // Ближайшая конечная вершина 

    //dfs
    clock_t start_dfs = clock(); // подсчет времени
    double distance_to_finish_dfs = dfs(graph.nodes, closest_node, finish_node, visited_dfs); // Поиск пути по алгоритму DFS
    clock_t end_dfs = clock();
    cout << "Расстояние по оценке DFS: " << distance_to_finish_dfs << endl;
    

    //bfs
    clock_t start_bfd = clock(); // подсчет времени
    double distance_to_finish_bfs = bfs(graph.nodes, closest_node, finish_node); // Поиск пути по алгоритму BFS
    clock_t end_bfs = clock();
    cout << "Расстояние по оценке BFS: " << distance_to_finish_bfs << endl;

    //dijkstra
    clock_t start_djkstra = clock(); // подсчет времени
    unordered_map<Node*, double> distances_dijkstra = dijkstra(graph.nodes, closest_node); // Поиск пути по алгоритму Дейкстры
    clock_t end_djkstra = clock();
    double distance_to_finish_dijkstra = distances_dijkstra[finish_node];
    cout << "Расстояние по оценке Дейкстры: " << distance_to_finish_dijkstra << endl;

    //a_star
    clock_t start_a_star = clock(); // подсчет времени
    unordered_map<Node*, double> distances_a_star = a_star(graph.nodes, closest_node, finish_node); // Поиск пути по алгоритму A*
    clock_t end_a_star = clock();
    double distance_to_finish_a_star = distances_a_star[finish_node];
    cout << "Расстояние по оценке Дейкстры с эвристикой: " << distance_to_finish_a_star << endl;

    //Вывод времени выполнения алгоритмов
    double time_djkstra = (double)(end_djkstra - start_djkstra) / CLOCKS_PER_SEC;
    double time_a_star = (double)(end_a_star - start_a_star) / CLOCKS_PER_SEC;
    double time_dfs = (double)(end_dfs - start_dfs) / CLOCKS_PER_SEC;
    double time_bfs = (double)(end_bfs - start_bfd) / CLOCKS_PER_SEC;
    cout << "Время выполнения алгоритма DFS: " << time_dfs << endl;
    cout << "Время выполнения алгоритма BFS: " << time_bfs << endl;
    cout << "Время выполнения алгоритма Дейкстры: " << time_djkstra << endl;
    cout << "Время выполнения алгоритма A*: " << time_djkstra << endl;
    return 0;
}
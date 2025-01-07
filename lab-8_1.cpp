#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cmath>
#include <limits>
#include <cassert>

using namespace std;

// Узел графа
struct Node {
    double lon, lat; // O(1) по памяти на узел
    vector<pair<Node*, double>> neighbors; // O(E) памяти, где E — количество рёбер для узла
};

// Класс для представления графа
class Graph {
public:
    vector<Node*> nodes; // O(V) памяти, где V — количество узлов
    unordered_map<string, Node*> nodeMap; // O(V) памяти для хэш-таблицы узлов
    size_t memoryUsage = 0; // O(1) памяти для подсчёта использования

    void loadFromFile(const string& filename) {
        ifstream file(filename); // O(1) по памяти для файлового потока
        if (!file.is_open()) {
            cerr << "Ошибка открытия файла: " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) { // O(V + E) по времени для обработки всех узлов и рёбер
            stringstream ss(line); // O(1) по памяти для каждой строки
            string parentStr, edgeStr;
            getline(ss, parentStr, ':'); // O(1) по времени

            double parentLon, parentLat;
            sscanf(parentStr.c_str(), "%lf,%lf", &parentLon, &parentLat); // O(1) по времени
            Node* parent = getNode(parentLon, parentLat); // O(1) для получения или создания узла

            while (getline(ss, edgeStr, ';')) { // O(E) для обработки рёбер
                double childLon, childLat, weight;
                sscanf(edgeStr.c_str(), "%lf,%lf,%lf", &childLon, &childLat, &weight); // O(1) по времени
                Node* child = getNode(childLon, childLat); // O(1) для получения или создания узла
                parent->neighbors.emplace_back(child, weight); // O(1) для добавления ребра
            }
        }
        file.close(); // O(1) по времени
    }

    Node* findClosestNode(double lat, double lon) {
        Node* closest = nullptr; // O(1) по памяти
        double minDist = numeric_limits<double>::infinity(); // O(1) по памяти

        for (auto* node : nodes) { // O(V) по времени
            double dist = hypot(node->lat - lat, node->lon - lon); // O(1) для вычисления расстояния
            if (dist < minDist) { // O(1) для сравнения
                minDist = dist;
                closest = node; // O(1) для обновления ближайшего узла
            }
        }
        return closest; // O(1) по времени
    }

private:
    Node* getNode(double lon, double lat) {
        string key = toKey(lon, lat); // O(1) по времени для создания ключа
        if (!nodeMap.count(key)) { // O(1) для проверки наличия узла
            Node* newNode = new Node{lon, lat}; // O(1) по памяти на создание узла
            nodes.push_back(newNode); // O(1) для добавления в список
            nodeMap[key] = newNode; // O(1) для добавления в хэш-таблицу
            memoryUsage += sizeof(Node) + sizeof(pair<Node*, double>) * newNode->neighbors.capacity(); // O(1) для обновления памяти
        }
        return nodeMap[key]; // O(1) по времени
    }

    string toKey(double lon, double lat) {
        return to_string(lon) + "," + to_string(lat); // O(1) по времени
    }
};

bool dfs(Node* current, Node* goal, unordered_set<Node*>& visited, double& distance) {
    if (current == goal) return true; // O(1) по времени
    visited.insert(current); // O(1) для добавления узла в множество

    for (auto& neighbor : current->neighbors) { // O(E) по времени для обхода соседей
        if (!visited.count(neighbor.first)) { // O(1) для проверки посещённости
            distance += neighbor.second; // O(1) для обновления расстояния
            if (dfs(neighbor.first, goal, visited, distance)) return true; // Рекурсивный вызов, O(E) в худшем случае
            distance -= neighbor.second; // O(1) для отката расстояния
        }
    }
    return false; // O(1) по времени
}

double bfs(Node* start, Node* goal) {
    if (!start || !goal) return -1.0; // O(1) по времени

    queue<pair<Node*, double>> q; // O(V) памяти в худшем случае
    unordered_set<Node*> visited; // O(V) памяти для хранения посещённых узлов
    q.push({start, 0.0}); // O(1) для добавления в очередь
    visited.insert(start); // O(1) для пометки посещения

    while (!q.empty()) { // O(V + E) по времени
        auto [current, dist] = q.front(); // O(1) для извлечения из очереди
        q.pop(); // O(1) для удаления из очереди

        if (current == goal) return dist; // O(1) для проверки

        for (auto& neighbor : current->neighbors) { // O(E) для обработки соседей
            if (!visited.count(neighbor.first)) { // O(1) для проверки посещённости
                visited.insert(neighbor.first); // O(1) для пометки посещения
                q.push({neighbor.first, dist + neighbor.second}); // O(1) для добавления в очередь
            }
        }
    }
    return -1.0; // O(1) по времени
}

double dijkstra(Node* start, Node* goal, const vector<Node*>& nodes) {
    unordered_map<Node*, double> distances; // O(V) памяти для хранения расстояний
    for (auto* node : nodes) distances[node] = numeric_limits<double>::infinity(); // O(V) для инициализации
    distances[start] = 0.0; // O(1) для установки начального расстояния

    priority_queue<pair<double, Node*>, vector<pair<double, Node*>>, greater<>> pq; // O(V) памяти для очереди
    pq.push({0.0, start}); // O(log(V)) для добавления в очередь

    while (!pq.empty()) { // O((V + E) * log(V)) по времени
        auto [currentDist, currentNode] = pq.top(); // O(1) для извлечения из очереди
        pq.pop(); // O(log(V)) для удаления из очереди

        if (currentNode == goal) return currentDist; // O(1) для проверки

        for (auto& neighbor : currentNode->neighbors) { // O(E) для обработки соседей
            double newDist = currentDist + neighbor.second; // O(1) для вычисления нового расстояния
            if (newDist < distances[neighbor.first]) { // O(1) для сравнения
                distances[neighbor.first] = newDist; // O(1) для обновления расстояния
                pq.push({newDist, neighbor.first}); // O(log(V)) для добавления в очередь
            }
        }
    }
    return -1.0; // O(1) по времени
}

double aStar(Node* start, Node* goal, const vector<Node*>& nodes) {
    auto heuristic = [](Node* a, Node* b) { // O(1) для вычисления эвристики
        return hypot(a->lat - b->lat, a->lon - b->lon); // O(1) по времени
    };

    unordered_map<Node*, double> gScore, fScore; // O(V) памяти для хранения оценок
    for (auto* node : nodes) {
        gScore[node] = fScore[node] = numeric_limits<double>::infinity(); // O(V) для инициализации
    }
    gScore[start] = 0.0; // O(1) для начального узла
    fScore[start] = heuristic(start, goal); // O(1) для эвристики

    priority_queue<pair<double, Node*>, vector<pair<double, Node*>>, greater<>> pq; // O(V) памяти для очереди
    pq.push({fScore[start], start}); // O(log(V)) для добавления в очередь

    while (!pq.empty()) { // O((V + E) * log(V)) по времени
        auto [_, current] = pq.top(); // O(1) для извлечения из очереди
        pq.pop(); // O(log(V)) для удаления из очереди

        if (current == goal) return gScore[current]; // O(1) для проверки

        for (auto& neighbor : current->neighbors) { // O(E) для обработки соседей
            double tentativeG = gScore[current] + neighbor.second; // O(1) для вычисления нового расстояния
            if (tentativeG < gScore[neighbor.first]) { // O(1) для сравнения
                gScore[neighbor.first] = tentativeG; // O(1) для обновления оценки
                fScore[neighbor.first] = tentativeG + heuristic(neighbor.first, goal); // O(1) для обновления оценки
                pq.push({fScore[neighbor.first], neighbor.first}); // O(log(V)) для добавления в очередь
            }
        }
    }
    return -1.0; // O(1) по времени
}

void testDfs(Graph& graph) {
    graph.loadFromFile("spb_graph.txt"); // O(V + E) по времени для загрузки файла

    Node* start = graph.findClosestNode(59.884972, 30.368072); // O(V) по времени
    Node* goal = graph.findClosestNode(59.956248, 30.309215); // O(V) по времени

    assert(start && goal); // O(1) для проверки

    double distance = 0.0;
    unordered_set<Node*> visited; // O(V) памяти для DFS
    assert(dfs(start, goal, visited, distance)); // O(V + E) по времени
    assert(distance == 14.0); // O(1) для проверки
}

void testBfs(Graph& graph) {
    graph.loadFromFile("spb_graph.txt"); // O(V + E) по времени для загрузки файла

    Node* start = graph.findClosestNode(59.884972, 30.368072); // O(V) по времени
    Node* goal = graph.findClosestNode(59.956248, 30.309215); // O(V) по времени

    assert(start && goal); // O(1) для проверки

    double distance = bfs(start, goal); // O(V + E) по времени
    assert(distance == 14.0); // O(1) для проверки
}

void testDijkstra(Graph& graph) {
    graph.loadFromFile("spb_graph.txt"); // O(V + E) по времени для загрузки файла

    Node* start = graph.findClosestNode(59.884972, 30.368072); // O(V) по времени
    Node* goal = graph.findClosestNode(59.956248, 30.309215); // O(V) по времени

    assert(start && goal); // O(1) для проверки

    double distance = dijkstra(start, goal, graph.nodes); // O((V + E) * log(V)) по времени
    assert(distance == 14.0); // O(1) для проверки
}

void testAStar(Graph& graph) {
    graph.loadFromFile("spb_graph.txt"); // O(V + E) по времени для загрузки файла

    Node* start = graph.findClosestNode(59.884972, 30.368072); // O(V) по времени
    Node* goal = graph.findClosestNode(59.956248, 30.309215); // O(V) по времени

    assert(start && goal); // O(1) для проверки

    double distance = aStar(start, goal, graph.nodes); // O((V + E) * log(V)) по времени
    assert(distance == 14.0); // O(1) для проверки
}

int main() {
    Graph graph;

    testDfs(graph); // O(V + E) для тестов
    testBfs(graph); // O(V + E) для тестов
    testDijkstra(graph); // O((V + E) * log(V)) для тестов
    testAStar(graph); // O((V + E) * log(V)) для тестов

    return 0;
}


#include <iostream>
#include <vector>
#include <string>
#include <iomanip> // For setw

using namespace std;

// Using 999 to represent infinity as per the sample
const int INF = 999;

// --- Helper Function ---
void inputCostMatrix(vector<vector<int>>& costMatrix, int nodes) {
    cout << "Enter cost matrix (use " << INF << " for infinity):\n";
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            cin >> costMatrix[i][j];
        }
    }
}

// --- 1. Distance Vector Routing ---
void performDistanceVector(const vector<vector<int>>& costMatrix, int nodes) {
    // dist[i][j] = cost from i to j
    // via[i][j] = next hop from i to get to j
    vector<vector<int>> dist = costMatrix;
    vector<vector<int>> via(nodes, vector<int>(nodes));

    // Initialize distance and via tables
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            via[i][j] = j;
        }
    }

    // Bellman-Ford algorithm
    // We iterate 'nodes' times to ensure convergence
    for (int k = 0; k < nodes; k++) { // Intermediate router
        for (int i = 0; i < nodes; i++) { // Source router
            for (int j = 0; j < nodes; j++) { // Destination router
                // If path via 'k' is shorter
                // Check for INF to prevent overflow/incorrect logic
                if (dist[i][k] != INF && dist[k][j] != INF) {
                    int newDist = dist[i][k] + dist[k][j];
                    if (newDist < dist[i][j]) {
                        dist[i][j] = newDist;
                        via[i][j] = via[i][k]; // Next hop from i to j is via i's next hop to k
                    }
                }
            }
        }
    }

    // Display the final routing table for each router
    cout << "\n--- Final Distance Vector Tables ---\n";
    for (int i = 0; i < nodes; i++) {
        cout << "\nRouting Table for Router " << (i) << ":\n";
        cout << "---------------------------------\n";
        cout << "Dest" << setw(10) << "NextHop" << setw(10) << "Dist\n";
        for (int j = 0; j < nodes; j++) {
            cout << j << setw(10) << via[i][j] << setw(10) << dist[i][j] << endl;
        }
    }
}

// --- 2. Link State Routing ---

// Helper for Dijkstra: Finds the unvisited node with the minimum distance
int findMinNode(const vector<int>& dist, const vector<bool>& visited, int nodes) {
    int minDist = INF;
    int nextNode = -1;

    for (int i = 0; i < nodes; i++) {
        if (!visited[i] && dist[i] < minDist) {
            minDist = dist[i];
            nextNode = i;
        }
    }
    return nextNode;
}

void performLinkState(const vector<vector<int>>& costMatrix, int nodes) {
    int sourceNode;
    cout << "Enter source node (0 to " << (nodes - 1) << "): ";
    cin >> sourceNode;

    if (sourceNode < 0 || sourceNode >= nodes) {
        cout << "Invalid source node.\n";
        return;
    }

    vector<int> dist(nodes);
    vector<bool> visited(nodes);

    // Initialize distance and visited arrays
    for (int i = 0; i < nodes; i++) {
        dist[i] = costMatrix[sourceNode][i];
        visited[i] = false;
    }

    // Source node is visited and distance to itself is 0
    dist[sourceNode] = 0;
    visited[sourceNode] = true;

    // Dijkstra's Algorithm
    for (int count = 0; count < nodes - 1; count++) {
        int u = findMinNode(dist, visited, nodes);

        if (u == -1) {
            break; // No more reachable nodes
        }

        visited[u] = true;

        // Relaxation step
        for (int v = 0; v < nodes; v++) {
            if (!visited[v] && costMatrix[u][v] != INF && dist[u] != INF &&
                (dist[u] + costMatrix[u][v] < dist[v])) {
                dist[v] = dist[u] + costMatrix[u][v];
            }
        }
    }

    // Display the result
    cout << "\n--- Shortest Path from Node " << sourceNode << " (Link State) ---\n";
    cout << "-------------------------------------------\n";
    cout << "Dest" << setw(10) << "Cost\n";
    for (int i = 0; i < nodes; i++) {
        cout << i << setw(10) << (dist[i] == INF ? "INF" : to_string(dist[i])) << endl;
    }
}

// --- Main Menu ---

int main() {
    int choice;
    cout << "--- Routing Algorithms ---\n";
    cout << "Name: Shivam Hippalgave\n";
    cout << "PRN: 202301040046\n";
    cout << "--------------------------\n";
    cout << "Select Algorithm:\n";
    cout << "1. Distance Vector Routing\n";
    cout << "2. Link State Routing\n";
    cout << "Enter choice: ";
    cin >> choice;

    int nodes;
    cout << "\nEnter number of nodes: ";
    cin >> nodes;

    vector<vector<int>> costMatrix(nodes, vector<int>(nodes));
    inputCostMatrix(costMatrix, nodes);

    switch (choice) {
        case 1:
            performDistanceVector(costMatrix, nodes);
            break;
        case 2:
            performLinkState(costMatrix, nodes);
            break;
        default:
            cout << "Invalid choice.\n";
            break;
    }

    return 0;
}
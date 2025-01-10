#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <utility>

using namespace std;

// Union-Find Data Structure
class UnionFind {
public:
    vector<int> parent, rank;
    UnionFind(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; i++) parent[i] = i;
    }

    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]); // Path compression
        return parent[x];
    }

    void unite(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);
        if (rootX != rootY) {
            if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
            } else if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
            } else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }
};

int main() {
    int n, m;
    cin >> n >> m;

    vector<int> values(n);
    for (int i = 0; i < n; i++) {
        cin >> values[i];
    }

    UnionFind uf(n);
    vector<vector<pair<int, int>>> penalties(n); // {neighbor, penalty}

    for (int t = 0; t < m; t++) {
        int type, i, j, x = 0;
        cin >> type >> i >> j;
        i--; j--; // Convert to 0-based indexing
        if (type == 1) {
            cin >> x;
            penalties[i].push_back(make_pair(j, x));
        } else if (type == 2) {
            uf.unite(i, j);
        }
    }

    // Group balls into components using Union-Find
    map<int, vector<int>> components;
    for (int i = 0; i < n; i++) {
        components[uf.find(i)].push_back(i);
    }

    long long maxEarnings = 0;
    vector<int> pickedBalls;

    // Process each component
    for (auto it = components.begin(); it != components.end(); ++it) {
        vector<int> balls = it->second;
        int componentSize = balls.size();
        long long bestValue = 0;
        vector<int> bestSubset;

        // Evaluate all subsets of the current component
        int subsetCount = (1 << componentSize); // Total subsets of balls
        for (int mask = 0; mask < subsetCount; mask++) {
            long long currentValue = 0;
            vector<int> currentSubset;
            set<int> picked;

            // Determine which balls are picked in this subset
            for (int i = 0; i < componentSize; i++) {
                if (mask & (1 << i)) {
                    int ball = balls[i];
                    currentValue += values[ball];
                    currentSubset.push_back(ball);
                    picked.insert(ball);
                }
            }

            // Apply penalties
            bool valid = true;
            for (int ball : currentSubset) {
                for (size_t p = 0; p < penalties[ball].size(); p++) {
                    int neighbor = penalties[ball][p].first;
                    int penalty = penalties[ball][p].second;
                    if (picked.find(neighbor) == picked.end()) {
                        currentValue -= penalty;
                        if (currentValue < 0) {
                            valid = false;
                            break;
                        }
                    }
                }
                if (!valid) break;
            }

            // Update the best value and subset if valid
            if (valid && currentValue > bestValue) {
                bestValue = currentValue;
                bestSubset = currentSubset;
            }
        }

        // Add the best subset of this component to the global result
        if (bestValue > 0) {
            maxEarnings += bestValue;
            pickedBalls.insert(pickedBalls.end(), bestSubset.begin(), bestSubset.end());
        }
    }

    // Output results
    cout << maxEarnings << endl;
    cout << pickedBalls.size() << endl;
    for (size_t i = 0; i < pickedBalls.size(); i++) {
        cout << pickedBalls[i] + 1 << " "; // Convert back to 1-based indexing
    }
    cout << endl;

    return 0;
}

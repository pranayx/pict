#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;

class Graph
{
    int V;
    vector<vector<int>> adj;

public:
    Graph(int V)
    {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int u, int v)
    {
        adj[u].push_back(v);
        adj[v].push_back(u); // Undirected
    }

    void sequentialBFS(int start, vector<int> &sbfsSequence)
    {
        vector<bool> visited(V, false);
        queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty())
        {
            int node = q.front();
            q.pop();
            sbfsSequence.push_back(node);

            for (auto neighbor : adj[node])
            {
                if (!visited[neighbor])
                {
                    q.push(neighbor);
                    visited[neighbor] = true;
                }
            }
        }
    }

    void parallelBFS(int start, vector<int> &pbfsSequence)
    {
        vector<bool> visited(V, false);
        queue<int> q;
        q.push(start);
        visited[start] = true;

        while (!q.empty())
        {
            int size = q.size();
            vector<int> currentLevel;
            for (int i = 0; i < size; ++i)
            {
                int node = q.front();
                q.pop();
                pbfsSequence.push_back(node);
                currentLevel.push_back(node);
            }

#pragma omp parallel for
            for (int i = 0; i < currentLevel.size(); ++i)
            {
                int node = currentLevel[i];
                for (auto neighbor : adj[node])
                {
                    if (!visited[neighbor])
                    {
#pragma omp critical
                        {
                            if (!visited[neighbor])
                            {
                                q.push(neighbor);
                                visited[neighbor] = true;
                            }
                        }
                    }
                }
            }
        }
    }

    void sequentialDFS(int start, vector<int> &sdfsSequence)
    {
        vector<bool> visited(V, false);
        stack<int> s;
        s.push(start);
        visited[start] = true;

        while (!s.empty())
        {
            int node = s.top();
            s.pop();
            sdfsSequence.push_back(node);

            for (auto neighbor : adj[node])
            {
                if (!visited[neighbor])
                {
                    s.push(neighbor);
                    visited[neighbor] = true;
                }
            }
        }
    }

    void parallelDFS(int start, vector<int> &pdfsSequence)
    {
        vector<bool> visited(V, false);
        stack<int> s;
        s.push(start);
        visited[start] = true;

        while (!s.empty())
        {
            int node = s.top();
            s.pop();
            pdfsSequence.push_back(node);

            vector<int> neighbors = adj[node];

#pragma omp parallel for
            for (int i = 0; i < neighbors.size(); ++i)
            {
                int neighbor = neighbors[i];
                if (!visited[neighbor])
                {
#pragma omp critical
                    {
                        if (!visited[neighbor])
                        {
                            s.push(neighbor);
                            visited[neighbor] = true;
                        }
                    }
                }
            }
        }
    }
};

int main()
{
    int V = 100000; // Reduced for clarity when printing sequences
    int K = 100;    // Tune factor (20) based on experiments; increase for denser graphs
    Graph g(V);

    // Generate a dense, cyclic graph
    for (int i = 0; i < V; ++i)
    {
        for (int j = 1; j <= K; ++j)
        {
            int neighbor = (i + j) % V;
            g.addEdge(i, neighbor);
        }
    }

    vector<int> sbfsSequence, pbfsSequence, sdfsSequence, pdfsSequence;

    auto start = high_resolution_clock::now();
    g.sequentialBFS(0, sbfsSequence);
    auto end = high_resolution_clock::now();
    duration<double> sbfstime = end - start;
    cout << "Sequential BFS Time: " << sbfstime.count() << " seconds" << endl;
    // cout << "Sequential BFS Traversal: ";
    // for (auto node : sbfsSequence)
    //     cout << node << " ";
    // cout << endl;
    // cout << endl;

    start = high_resolution_clock::now();
    g.parallelBFS(0, pbfsSequence);
    end = high_resolution_clock::now();
    duration<double> pbfstime = end - start;
    cout << "Parallel BFS Time: " << pbfstime.count() << " seconds" << endl;
    // cout << "Parallel BFS Traversal: ";
    // for (auto node : pbfsSequence)
    //     cout << node << " ";
    // cout << endl;
    // cout << endl;

    start = high_resolution_clock::now();
    g.sequentialDFS(0, sdfsSequence);
    end = high_resolution_clock::now();
    duration<double> sdfstime = end - start;
    cout << "Sequential DFS Time: " << sdfstime.count() << " seconds" << endl;
    // cout << "Sequential DFS Traversal: ";
    // for (auto node : sdfsSequence)
    //     cout << node << " ";
    // cout << endl;
    // cout << endl;

    start = high_resolution_clock::now();
    g.parallelDFS(0, pdfsSequence);
    end = high_resolution_clock::now();
    duration<double> pdfstime = end - start;
    cout << "Parallel DFS Time: " << pdfstime.count() << " seconds" << endl;
    // cout << "Parallel DFS Traversal: ";
    // for (auto node : pdfsSequence)
    //     cout << node << " ";
    // cout << endl;
    // cout << endl;

    return 0;
}

/*
    RUN:
    g++ -fopenmp parallel_graph.cpp -o parallel_graph
    ./parallel_graph
*/
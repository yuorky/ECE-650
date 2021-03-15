#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <cstdio>

using namespace std;

//define the class Graph to store V and E
class Graph{
    int v;
public:
    vector<int> *adj;
    explicit Graph(int v){
        this->v = v;
        this->adj = new vector<int>[v];
    };

};

Graph *graph;


//define add edge function
void add_edge(vector<int>adj[], int src, int dest)
{
    adj[src].push_back(dest);
    adj[dest].push_back(src);
}
//define BFS function
bool BFS(vector<int> adj[], int src, int dest, int v, int pred[])
{
    queue<int> q;
    bool visited[v];

    for (int i=0; i<v; i++){
        visited[i] = false;
        pred[i] = -1;
    }

    visited[src] = true;
    q.push(src);

    //Standard BFS
    while (!q.empty()){
        int u = q.front();
        q.pop();
        for (unsigned int i=0;i<adj[u].size();i++){
            if (!visited[adj[u][i]]){
                visited[adj[u][i]] = true;
                pred[adj[u][i]] = u;
                q.push(adj[u][i]);

                if (adj[u][i]==dest)
                    return true;
            }
        }
    }
    return false;
}

void printShortestDistance(vector<int> adj[], int src, int dest, int v)
{
    int pred[v];

    if (src==dest){
        cout<<src<<endl;
        return;
    }
    if (!BFS(adj, src, dest, v, pred)){
        cerr << "Error: A path does not exist between src and dest." << endl;
        return;
    }
    if (BFS(adj,src,dest,v,pred)) {
        vector<int> path;
        int crawl = dest;
        path.push_back(crawl);
        while (pred[crawl] != -1) {
            path.push_back(pred[crawl]);
            crawl = pred[crawl];
        }

        for (unsigned int i = path.size() - 1; i > 0; i--) {
            cout << path[i] << "-";
        }
        cout << path[0] << endl;
    }
}

int main() {
    //read from stdin until EOF
    while(!std::cin.eof()){
        std::string line;
        std::getline(std::cin,line);
        int v;
        if (line.empty()){
            continue;
        }
        if (line.find('V') == 0){
            v = stoi(line.substr(2));
            delete graph;
            graph = new Graph(v);
        }
        if (line.find('E') == 0){
            size_t sign = line.find('<');
            int pointer = 0;
            while (sign != std::string::npos){
                size_t sign1 = line.find('>', sign);
                string edge = line.substr(sign+1,sign1-sign-1);
                size_t cm = edge.find(',');
                int e0 = stoi(edge.substr(0,cm));
                int e1 = stoi(edge.substr(cm+1));
                if (e0>=v or e1>=v or e0<0 or e1<0 or e0==e1){
                    std::cerr << "Error: edges do not exist." << endl;
                    pointer = 1;
                    break;
                }
                sign = line.find('<',sign+1);
            }
            if (pointer==0){
                size_t sign2 = line.find('<');
                while (sign2 != std::string::npos){
                    size_t sign3 = line.find('>',sign2);
                    string edge = line.substr(sign2+1,sign3-sign2-1);
                    size_t cm = edge.find(',');
                    int e0 = stoi(edge.substr(0,cm));
                    int e1 = stoi(edge.substr(cm+1));
                    add_edge(graph->adj,e0,e1);
                    sign2 = line.find('<',sign2+1);
                }
            }
        }
        if (line.find('s') == 0){
            size_t space1 = line.find(' ');
            size_t space2 = line.find(' ', space1+1);
            int src = stoi(line.substr(space1+1));
            int dest = stoi(line.substr(space2+1));
            printShortestDistance(graph->adj,src,dest,v);

        }
    }
    return 0;
}

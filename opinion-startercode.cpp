#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

/********************DO NOT EDIT**********************/
// Function prototype.
void read_opinions(string filename); 
void read_edges(string filename); 
void build_adj_list(); // Renamed to reflect the new data structure

int total_nodes = 0; 

/****************************************************************/

// Vector to hold each node's opinion (0 or 1)
std::vector<int> opinions;

// OPTIMIZATION: Incoming Adjacency List
// adj_in[i] will contain a list of all nodes that INFLUENCE node i.
std::vector<std::vector<int>> adj_in;

// edge list: each row contains {source, target}
std::vector<std::vector<int>> edge_list;

void build_adj_list()
{
    // (1) Allocate adjacency list of appropriate size (total_nodes)
    adj_in.assign(total_nodes, std::vector<int>());

    // (2) Run through edge list and populate the incoming adjacency list
    // In this simulation, source influences target (source -> target)
    // Therefore, target's opinion is shaped by source.
    for (const auto& edge : edge_list) {
        int source = edge[0];
        int target = edge[1];
        adj_in[target].push_back(source); 
    }
}

double calculate_fraction_of_ones()
{
    // (3) Calculate the fraction of nodes with opinion 1 and return it
    if (total_nodes == 0) return 0.0;
    double count_ones = 0;
    for (int op : opinions) {
        if (op == 1) count_ones++;
    }
    return count_ones / total_nodes;
}

// For a given node, count majority opinion among its neighbours. Tie -> 0.
int get_majority_friend_opinions(int node)
{
    // (4) Count the number of neighbours with opinion 0 and opinion 1
    int count0 = 0;
    int count1 = 0;

    // OPTIMIZATION: Only iterate through the specific nodes that influence this node
    // This avoids checking the entire total_nodes array.
    for (int influencer : adj_in[node]) {
        if (opinions[influencer] == 0) count0++;
        else count1++;
    }

    // Return the majority; if tie, return 0
    return (count1 > count0) ? 1 : 0;
}

// Calculate new opinions for all voters and return if anyone's opinion changed
bool update_opinions()
{
    // (5) Calculate the majority for all nodes before updating
    std::vector<int> next_opinions(total_nodes);
    bool changed = false;

    for (int i = 0; i < total_nodes; i++) {
        next_opinions[i] = get_majority_friend_opinions(i);
        if (next_opinions[i] != opinions[i]) {
            changed = true;
        }
    }

    opinions = next_opinions;
    return changed;
}

int main() {
    // Read input files
    read_opinions("opinions.txt"); 
    read_edges("edge_list.txt");

    // Convert edge list into adjacency list
    build_adj_list();
    
    cout << "Total nodes: " << total_nodes << endl;
    
    // Run simulation
    int max_iterations = 30;
    int iteration = 0;
    bool opinions_changed = true;
    
    // Print initial state
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << calculate_fraction_of_ones() << endl;
    
    // (6) Run until consensus or max iterations
    while (iteration < max_iterations && opinions_changed) {
        iteration++;
        opinions_changed = update_opinions();
        
        cout << "Iteration " << iteration << ": fraction of 1's = " 
             << calculate_fraction_of_ones() << endl;
    }

    ////////////////////////////////////////////////////////
    // Print final result
    double final_fraction = calculate_fraction_of_ones();
    
    if(final_fraction == 1.0)
        cout << "Consensus reached: all 1's" << endl;
    else if(final_fraction == 0.0)
        cout << "Consensus reached: all 0's" << endl;
    else
        cout << "No consensus reached after " << iteration << " iterations" << endl;
    
    return 0;
}

/*********** Functions to read files **************************/ 

void read_opinions(string filename)
{
    ifstream file(filename);
    int id, opinion;
    while(file >> id >> opinion)
    {
        opinions.push_back(opinion);
        if(id >= total_nodes) total_nodes = id+1;
    }
    file.close();
}

void read_edges(string filename)
{
    ifstream file(filename);
    int source, target;
    
    while(file >> source >> target)
    {
        edge_list.push_back({source, target});
        if(source >= total_nodes) total_nodes = source+1;
        if(target >= total_nodes) total_nodes = target+1;
    }
    file.close();
}
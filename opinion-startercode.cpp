#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

/********************DO NOT EDIT**********************/
// Function prototype. Defined later.
void read_opinions(string filename); // reads file into opinions vector and updates total_nodes as needed
void read_edges(string filename); // reads file into edge_list, defined later
void build_adj_matrix(); // convert edge_list to adjacency matrix

int total_nodes = 0; // We keep track of the total number of nodes based on largest node id.


/****************************************************************/

/******** Create adjacency matrix and vector of opinions */
// simple vector to hold each node's opinion (0 or 1)
std::vector<int> opinions;

// global adjacency matrix initialized later
std::vector<std::vector<int>> adj;

// edge list: each row contains {source, target}
std::vector<std::vector<int>> edge_list;

void build_adj_matrix()
{
    // (1) Allocate the adjacency matrix with the appropriate size based on total_nodes [cite: 106]
    adj.resize(total_nodes, std::vector<int>(total_nodes, 0));

    // (2) Iterate through the edge list to populate the adjacency matrix [cite: 89, 107]
    // The format is {source, target}, where the source node influences the target node [cite: 107]
    for (const auto& edge : edge_list) {
        int source = edge[0];
        int target = edge[1];
        // Set the relationship: source influences target [cite: 103, 107]
        adj[target][source] = 1; 
    }
}

double calculate_fraction_of_ones()
{
    // (3) Count nodes with opinion 1 and return the ratio over total nodes [cite: 91, 106]
    if (total_nodes == 0) return 0.0;
    int count = 0;
    for (int val : opinions) {
        if (val == 1) count++;
    }
    return static_cast<double>(count) / total_nodes;
}

// For a given node, count the majority opinion among its neighbors. Tie results in 0. [cite: 94]
int get_majority_friend_opinions(int node)
{
    // (4) Count the number of neighbors with opinion 0 and opinion 1 [cite: 93, 101]
    int count0 = 0;
    int count1 = 0;

    for (int neighbor = 0; neighbor < total_nodes; ++neighbor) {
        // Check if there is an edge from neighbor to the current node [cite: 102, 103]
        if (adj[node][neighbor] == 1) { 
            if (opinions[neighbor] == 0) count0++;
            else count1++;
        }
    }

    // Return the majority opinion; return 0 in case of a tie [cite: 94]
    if (count1 > count0) return 1;
    else return 0;
}

// Calculate new opinions for all voters and return true if any opinion changed [cite: 92, 98]
bool update_opinions()
{
    // (5) Synchronous update: compute all new opinions before updating the global state [cite: 92]
    std::vector<int> new_opinions(total_nodes);
    bool changed = false;

    // First pass: determine the new state for every node [cite: 93, 96]
    for (int i = 0; i < total_nodes; ++i) {
        new_opinions[i] = get_majority_friend_opinions(i);
    }

    // Second pass: apply the changes and track if the system has stabilized [cite: 97, 98]
    for (int i = 0; i < total_nodes; ++i) {
        if (new_opinions[i] != opinions[i]) {
            changed = true;
            opinions[i] = new_opinions[i];
        }
    }
    return changed;
}

int main() {
    // no preallocation; vectors grow on demand

    // Read input files
    read_opinions("opinions.txt"); 
    read_edges("edge_list.txt");

    // convert edge list into adjacency matrix once we know total_nodes
    build_adj_matrix();
    
    cout << "Total nodes: " << total_nodes << endl;
    
    // Run simulation
    int max_iterations = 30;
    int iteration = 0;
    bool opinions_changed = true;
    
    // Print initial state
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << calculate_fraction_of_ones() << endl;
    
    /// (6)  //////////////////////////////////////////////
    
    // (6) Run the simulation until consensus is reached or max_iterations is hit [cite: 97, 98, 100]
    while (opinions_changed && iteration < max_iterations) {
        iteration++;
        // Update all opinions based on the Voter Model rules [cite: 91, 92]
        opinions_changed = update_opinions();
        
        // Print progress at regular intervals [cite: 89]
        if (iteration % 5 == 0) {
            cout << "Iteration " << iteration << ": fraction of 1's = " 
                 << calculate_fraction_of_ones() << endl;
        }
    }
    
    ////////////////////////////////////////////////////////
    // Print final result
    double final_fraction = calculate_fraction_of_ones();
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << final_fraction << endl;
    
    if(final_fraction == 1.0)
        cout << "Consensus reached: all 1's" << endl;
    else if(final_fraction == 0.0)
        cout << "Consensus reached: all 0's" << endl;
    else
        cout << "No consensus reached after " << iteration << " iterations" << endl;
    
    return 0;
}


/*********** Functions to read files **************************/ 

// Read opinion vector from file.
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

// Read edge list from file and update total nodes as needed.
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

/********************************************************************** */
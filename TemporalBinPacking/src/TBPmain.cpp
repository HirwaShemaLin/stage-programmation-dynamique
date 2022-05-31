#include "../include/TBPProblem.hpp"
#include <vector>
#include <iomanip>

void display_items(vector<Item> items){
    cout << "(";
    for (int i=0; i<items.size(); i++) {
        cout << items[i].id;
        if (i<items.size()-1) {
            cout << ",";
        }
    }
    cout << ")";
}

void successive_display(Vertice u, int column){
    if (u.successors.empty()) {
        cout << "()" << endl;
        return;
    }
    //cout << "here, again" << endl;
    display_items(u.items);
    int n=0;
    for (Vertice v : u.successors) {
        if (n>0) {
            for (int i=0; i<column; i++) {
                cout << setw(5) << left << " ";
            }
        }
        cout << setw(5) << left << " -> ";
        successive_display(v, column+1);
        n++;
    }
}


void display_graph(vector<vector<Vertice>> graph){
    //cout << graph[0].size() << endl;
    successive_display(graph[0][0], 1);
}


int main(int argc, char * argv[]){
    TemporalBPData data;
    TBPProblem * problem = new TBPProblem(data);

    vector<vector<Vertice>> graph = problem->getGraph();

    display_graph(graph);

}
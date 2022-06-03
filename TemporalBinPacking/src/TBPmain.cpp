#include "../include/TBPProblem.hpp"
#include <string>
#include <vector>
#include <iomanip>

string list_items(vector<Item> items){
    string sItems = "(";
    for (int i=0; i<items.size(); i++) {
        sItems += to_string(items[i]._id);
        if (i<items.size()-1) {
            sItems += ",";
        }
    }
    sItems += ")";
    return sItems;
}

string list_arcs(vector<Arc> arcs){
    string sItems = "{";
    for (int i=0; i<arcs.size(); i++) {
        sItems += to_string(arcs[i]._successorId);
        if (arcs[i]._cost==0)
        {
            sItems += "-";
        }else
        {
            sItems += "+";
        }
        
        
        if (i<arcs.size()-1) {
            sItems += ",";
        }
    }
    sItems += "}";
    return sItems;
}
/*
void successive_display(TBPProblem * problem, Vertex* u, int column){
    if (u->_arcs.empty()) {
        cout << "()" << endl;
        return;
    }
    //cout << "here, again" << endl;
    display_items(u->_items);
    int n=0;
    for (Arc a : u->_arcs) {
        if (n>0) {
            for (int i=0; i<column; i++) {
                cout << setw(5) << left << " ";
            }
        }
        cout << setw(5) << left << " -> ";
        successive_display(problem, problem->getVertexById(*a._successorId), column+1);
        n++;
    }
}


void display_graph(TBPProblem * problem){
    //cout << graph[0].size() << endl;
    vector<Vertex> graph = problem->getGraph();
    successive_display(problem, problem->getVertexById(0), 1);
}
*/

int main(int argc, char * argv[]){
    TemporalBPData data;

    for(int c=0 ; c<data.getNbColumns() ; ++c){
        for (int v = 0; v < data._graph[c].size(); v++)
        {
            Vertex * u = & data._graph[c][v];
            cout << "Vertex " << u->_id << " : \tItems = " << list_items(u->_items) << ",\t Arcs = " << list_arcs(u->_arcs) << endl;
        }
    }

}
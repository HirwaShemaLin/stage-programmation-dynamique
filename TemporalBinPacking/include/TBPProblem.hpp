//
// Created by lhirwashema on 2022-06-01.
//

#pragma once

#include "TBPdata.hpp"
#include <algorithm>
#include <vector>
#include <cmath>


class Arc {
public:
    int * _successorId;
    int _cost;
    vector<Item> _newItems;
};

class Vertex {
public:
    int _id; 
    vector<Item> _items;
    vector<Arc> _arcs;
};


class TBPProblem{
public:
    TemporalBPData _data;

    TBPProblem(TemporalBPData & ItemData){
        _data = ItemData;
    }


    /**
     * Determines whether a selection of items can be packed into one bin
     * without violating the capacity constraint
     */
    bool isFeasible(vector<Item> selection){
        sort(selection.begin(), selection.end(), _data.compareEntries);
        int weight = 0;
        vector<Item> ongoing;
        for (int i=0; i<selection.size(); i++) {
            Item curItem = selection[i];
            weight += curItem._size;
            if (weight > _data.getCapacity()) {          ///< to save time, we only check if any item has left the bin
                for (int j=ongoing.size()-1; j>=0; j--) {   //< when the current total weight surpasses the bin capacity
                    Item pastItem = ongoing[j];
                    if (pastItem._exit <= curItem._entry) {
                        weight -= pastItem._size;
                        ongoing.erase(ongoing.begin()+j);
                    }
                }
            }
            if (weight > _data.getCapacity()) return false;

            ongoing.push_back(curItem);
        }
        return true;
    }

    /**
     * Computes a list of all feasible combinations of items within a clique.
     * Each combination must respect the capacity constraint
     */
    vector<vector<Item>> getFeasibleCombinations(vector<Item> clique){
        vector<vector<Item>> group;
        if (clique.empty()) {
            vector<Item> empty;
            group.push_back(empty);
        }else{
            Item item = clique[clique.size()-1];
            clique.pop_back();

            for (vector<Item> combo : getFeasibleCombinations(clique)) {
                group.push_back(combo);
                combo.push_back(item);
                if (isFeasible(combo)) {
                    group.push_back(combo);
                }
            }
        }

        return group;
    }

    bool isSuccessor(Vertex v, vector<Item> include, vector<Item> exclude){
        for (Item it : include) {
            if (!_data.isItemIn(it, v._items)) {
                return false;
            }
        }
        for (Item it : exclude) {
            if (_data.isItemIn(it, v._items)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Computes a graph to modelize the TBP problem as a minimum cost flow problem.
     * The graph is constructed from sink to start, linking each created vertex to its predecessor in the next step
     */
    vector<vector<Vertex>> getGraph(){
        vector<vector<Item>> cliques = _data.getMaxCliques();

        vector<vector<Vertex>> graph;

        Vertex start;
        start._id = 0;

        Vertex sink;
        sink._id = pow(10, cliques.size());
        vector<Vertex> sinkColumn;  ///< First we create the sink vertex and put it in the final column of our graph
        sinkColumn.push_back(sink);
        graph.push_back(sinkColumn);

        ///< We iterate over all maximum cliques from last to first, each clique corresponding to a column in our graph
        for (int a=cliques.size()-1; a>=0; a--) {
            vector<vector<Item>> combos = getFeasibleCombinations(cliques[a]);
            vector<Vertex> column;
            for (int c=0; c<combos.size(); c++) {
                vector<Item> selection = combos[c];
                Vertex u;   ///< for each feasible combination of items in a clique, we create a vertex
                u._items = selection;
                u._id = pow(10, a) + c + 1;
                if (a==cliques.size()-1) {  ///< any vertex constructed in the last clique connects to the sink
                    Arc toSink = {&sink._id, 0};
                    u._arcs.push_back(toSink);
                }else{  ///< otherwise that vertex must connect to a vertex in the next clique (previous clique in our exploration order)
                    vector<Item> exclude;   ///< vertices containing these items should be excuded
                    vector<Item> include;   ///< vertices containing these items should be incuded
                    for (Item it : selection) {
                        if (_data.isItemIn(it, cliques[a+1])) {
                            ///< if an item in our vertex exist in the next clique
                            include.push_back(it);  ///< then we can only connect to vertices containing that item in the next clique
                        }
                    }
                    for (Item it : cliques[a+1]) {
                        if (_data.isItemIn(it, cliques[a]) && !_data.isItemIn(it, selection)) {
                            ///< if an item exists in the current clique but not in our vertex
                            exclude.push_back(it);  ///< then we cannot connect to vertices containing that item in the next clique
                        }
                    }
                    ///< We create an arc connecting to any vertex that respects our inclusion and exclusion requirements in the next clique
                    for (Vertex v : graph[graph.size()-1]) {
                        if (isSuccessor(v, include, exclude)) {
                            vector<Item> new_items; ///< we check what items are new between u and v to add them to the arc
                            for (Item it : v._items) {
                                if (!_data.isItemIn(it, u._items)) {
                                    new_items.push_back(it);
                                }
                            }
                            Arc toNext = {&v._id, !new_items.empty(), new_items};
                            u._arcs.push_back(toNext);
                        }
                    }
                }
                if (a==0) { ///< any vertex constructed in the first clique connects to the start
                    Arc fromStart = {&u._id, !u._items.empty(), u._items};
                    start._arcs.push_back(fromStart);
                }
                column.push_back(u);
            }
            graph.push_back(column);
        }
        vector<Vertex> firstColumn; ///< Finally, we add the start vertex to our graph in the first column
        firstColumn.push_back(start);
        graph.push_back(firstColumn);

        ///< we reverse the order of our graph so the columns are chronologically aligned
        rotate(graph.begin(), graph.end()-1, graph.end());

        return graph;
    }

};
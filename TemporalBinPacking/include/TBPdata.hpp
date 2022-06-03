//
// Created by lhirwashema on 2022-06-01.
//

#pragma once

#include <vector>
#include <algorithm>

using namespace std;

class Item{
public:
    int _id;
    int _size;
    int _entry;
    int _exit;

    bool equals(Item item){
        return this->_id==item._id;
    }
};



class Arc {
public:
    int _successorId;
    int _cost;
    vector<Item> _newItems;
};

class Vertex {
public:
    int _id; 
    vector<Item> _items;
    vector<Arc> _arcs;
};



/**
 * TBP: Temporal Bin Packing
 *
 * minimize the number of bins needed to pack all items of the list
 * Each item has a size and a date of entry and exit that determins when the the item is available to be packed
 * The sum of the sizes in a bin is less than or equal to the bin's capacity at any moment in time.
 */
class TemporalBPData{
public:
    int _capacity;           ///< capacity of a bin
    vector<Item> _items;     ///< list of objects
    vector<vector<Vertex>> _graph;
    int _maxElemsClique;

    static bool smallerEntry(Item i, Item j) {    ///< item entry date comparator used when sorting items
	    return (i._entry < j._entry);
    }

    TemporalBPData(){       ///< parser not implemented yet
        _capacity = 15;
        _items = {{1,6,6,20}, {2,4,10,26}, {3,8,12,16}, {4,7,17,23}, {5,4,24,30}, {6,7,28,32}};
        sort(_items.begin(), _items.end(), smallerEntry);
        _graph = buildGraph();
    }

    int getNbItems() const { return _items.size(); }
    int getCapacity() const { return _capacity; }
    int getNbColumns() const { return _graph.size(); }
    int getUpperBound()   const { return _items.size(); }
    int getMaxElemsClique() const { return _maxElemsClique; }

    /**
     * Determines whether an item is present in a list
     */
    bool isItemIn(Item item, vector<Item> items){
        for (Item it : items) {
            if (item.equals(it)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Computes the maximum cliques within our list of items
     */
    vector<vector<Item>> getMaxCliques(){
        vector<vector<Item>> cliques;   ///< list of maximum cliques
        int maxElemsClique = 0;
        vector<Item> ongoing;           ///< list of items who have yet to exit
        for (int i=0; i<_items.size(); i++) {   ///< we iterate over items sorted by increasing entry date
            Item curItem = _items[i];   
            for (int j=ongoing.size()-1; j>=0; j--) {   ///<we iterate over ongoing items
                Item pastItem = ongoing[j];
                if (pastItem._exit <= _items[i]._entry) {   ///< if it is time for an ongoing item to leave
                    if ( cliques.size()==0 || !isItemIn(ongoing[ongoing.size()-1], cliques[cliques.size()-1])){
                        ///< if the last item added in the ongoing list does not belong in our latest clique
                        vector<Item> clique = ongoing;      ///< then we have a new clique with at least the last item of ongoing
                        cliques.push_back(clique);         //< as a new item in a clique.
                        if (clique.size()>maxElemsClique)
                            maxElemsClique = clique.size();     
                    }
                    ongoing.erase(ongoing.begin()+j);
                }
            }
            ongoing.push_back(curItem);
        }
        if (ongoing.size()>maxElemsClique)
            maxElemsClique = ongoing.size(); 

        _maxElemsClique = maxElemsClique;

        cliques.push_back(ongoing);     ///< the final items present since the last entry also form a clique
        return cliques;
    }



    /**
     * Determines whether a selection of items can be packed into one bin
     * without violating the capacity constraint
     */
    bool isFeasible(vector<Item> selection){
        sort(selection.begin(), selection.end(), smallerEntry);
        int weight = 0;
        vector<Item> ongoing;
        for (int i=0; i<selection.size(); i++) {
            Item curItem = selection[i];
            weight += curItem._size;
            if (weight > getCapacity()) {          ///< to save time, we only check if any item has left the bin
                for (int j=ongoing.size()-1; j>=0; j--) {   //< when the current total weight surpasses the bin capacity
                    Item pastItem = ongoing[j];
                    if (pastItem._exit <= curItem._entry) {
                        weight -= pastItem._size;
                        ongoing.erase(ongoing.begin()+j);
                    }
                }
            }
            if (weight > getCapacity()) return false;

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
            if (!isItemIn(it, v._items)) {
                return false;
            }
        }
        for (Item it : exclude) {
            if (isItemIn(it, v._items)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Computes a graph to modelize the TBP problem as a minimum cost flow problem.
     * The graph is constructed from sink to start, linking each created vertex to its predecessor in the next step
     */
    vector<vector<Vertex>> buildGraph(){
        vector<vector<Item>> cliques = getMaxCliques();

        vector<vector<Vertex>> graph;
        Vertex start;
        int nbVertices = 0;
        Vertex sink;
        sink._id = nbVertices;
        vector<Vertex> sinkColumn;  ///< First we create the sink vertex and put it in the final column of our graph
        sinkColumn.push_back(sink);
        graph.push_back(sinkColumn);

        nbVertices++;
        ///< We iterate over all maximum cliques from last to first, each clique corresponding to a column in our graph
        for (int a=cliques.size()-1; a>=0; a--) {
            vector<vector<Item>> combos = getFeasibleCombinations(cliques[a]);
            vector<Vertex> column;
            for (int c=combos.size()-1; c>=0; c--) {
                vector<Item> selection = combos[c];
                Vertex u;   ///< for each feasible combination of items in a clique, we create a vertex
                u._items = selection;
                u._id = nbVertices;
                if (a==cliques.size()-1) {  ///< any vertex constructed in the last clique connects to the sink
                    Arc toSink = {sink._id, 0};
                    u._arcs.push_back(toSink);
                }else{  ///< otherwise that vertex must connect to a vertex in the next clique (previous clique in our exploration order)
                    vector<Item> exclude;   ///< vertices containing these items should be excuded
                    vector<Item> include;   ///< vertices containing these items should be incuded
                    for (Item it : selection) {
                        if (isItemIn(it, cliques[a+1])) {
                            ///< if an item in our vertex exist in the next clique
                            include.push_back(it);  ///< then we can only connect to vertices containing that item in the next clique
                        }
                    }
                    for (Item it : cliques[a+1]) {
                        if (isItemIn(it, cliques[a]) && !isItemIn(it, selection)) {
                            ///< if an item exists in the current clique but not in our vertex
                            exclude.push_back(it);  ///< then we cannot connect to vertices containing that item in the next clique
                        }
                    }
                    ///< We create an arc connecting to any vertex that respects our inclusion and exclusion requirements in the next clique
                    for (Vertex v : graph[graph.size()-1]) {
                        if (isSuccessor(v, include, exclude)) {
                            vector<Item> new_items; ///< we check what items are new between u and v to add them to the arc
                            for (Item it : v._items) {
                                if (!isItemIn(it, u._items)) {
                                    new_items.push_back(it);
                                }
                            }
                            Arc toNext = {v._id, !new_items.empty(), new_items};
                            u._arcs.push_back(toNext);
                        }
                    }
                }
                if (a==0) { ///< any vertex constructed in the first clique connects to the start
                    Arc fromStart = {u._id, !u._items.empty(), u._items};
                    start._arcs.push_back(fromStart);
                }
                column.push_back(u);
                nbVertices++;
            }
            graph.push_back(column);
        }
        start._id = nbVertices;
        vector<Vertex> firstColumn; ///< Finally, we add the start vertex to our graph in the first column
        firstColumn.push_back(start);
        graph.push_back(firstColumn);
        nbVertices++;

        ///< we reverse the order of our graph so the columns are chronologically aligned
         //< and we assign the chronologically correct IDs to vertices

        int id = 0;
        for (int c=graph.size()-1; c>=0; c--) {
            for (int v=graph[c].size()-1; v>=0; v--) {
                graph[c][v]._id = nbVertices - graph[c][v]._id -1;
                for (int a=0; a<graph[c][v]._arcs.size(); a++) {
                    graph[c][v]._arcs[a]._successorId = nbVertices - graph[c][v]._arcs[a]._successorId -1;
                }
                reverse(graph[c][v]._arcs.begin(), graph[c][v]._arcs.end());
                id++;
            }
            reverse(graph[c].begin(), graph[c].end());
        }
        reverse(graph.begin(), graph.end());
/*
        if (id!=nbVertices) {
            cout << "Error, unexpected number of vertices" << endl;
        }
*/

        return graph;
    }

};
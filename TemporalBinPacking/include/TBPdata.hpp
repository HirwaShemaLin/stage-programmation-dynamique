//
// Created by lhirwashema on 2022-06-01.
//


#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;

class Item{
public:
    int _id;
    int _size;
    int _entry;
    int _exit;

    Item(int id, int size, int entry, int exit) :
        _id(id),
        _size(size),
        _entry(entry),
        _exit(exit){}

    bool equals(const Item & item) const {
        return this->_id==item._id;
    }
};



class Arc {
public:
    int _successorId;
    vector<int> _newItems;
};

class Vertex {
public:
    int _id; 
    vector<int> _items;
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
    int _maxNbCombosClique;

    TemporalBPData();   // dummy instance for tests
    TemporalBPData(bool reduced);   // dummy instance for tests
    TemporalBPData(std::ifstream &in); // read the instance from a file
    TemporalBPData(std::ifstream &in, bool reduced); // read the instance from a file

    int getNbItems() const { return _items.size(); }
    int getCapacity() const { return _capacity; }
    int getNbColumns() const { return _graph.size(); }
    int getUpperBound()   const { return getMaxNbCombosClique(); }
    // Implement parsers
    // Clean up everything
    int getMaxNbCombosClique() const { return _maxNbCombosClique; }

    /**
     * Computes the maximum cliques within our list of items
     */
    vector<vector<int>> getMaxCliques();

    /**
     * Determines whether a selection of items can be packed into one bin
     * without violating the capacity constraint
     */
    bool isFeasible(vector<int> & selection);

    /**
     * Computes a list of all feasible combinations of items within a clique.
     * Each combination must respect the capacity constraint
     */
    vector<vector<int>> getFeasibleCombinations(vector<int> clique);

    /**
     * Computes a graph to modelize the TBP problem as a minimum cost flow problem.
     * The graph is constructed from sink to start, linking each created vertex to its predecessor in the next step
     */
    vector<vector<Vertex>> buildGraph();

    vector<vector<int>> getReducedCliques();
    
    vector<vector<Vertex>> buildReducedGraph();

    int getVertexYPos(int vertexId, int column);

};
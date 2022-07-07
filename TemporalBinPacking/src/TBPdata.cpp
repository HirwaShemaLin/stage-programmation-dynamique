//
// Created by lhirwashema on 2022-06-01.
//

#include "../include/TBPdata.hpp"

using namespace std;

/**
 * TBP: Temporal Bin Packing
 *
 * minimize the number of bins needed to pack all items of the list
 * Each item has a size and a date of entry and exit that determins when the the item is available to be packed
 * The sum of the sizes in a bin is less than or equal to the bin's capacity at any moment in time.
 */

static bool smallerEntry(const Item & i, const Item & j) {    ///< item entry date comparator used when sorting items
    return (i._entry < j._entry);
}



TemporalBPData::TemporalBPData(bool reduced){       ///< parser not implemented yet
/*    _capacity = 15;
    _items = {{0,6,6,20},
                {1,4,10,26},
                {2,8,12,16},
                {3,7,17,23},
                {4,4,24,30},
                {5,7,28,32}}; */
    _capacity = 15;
    _items={{0,6,6,20},
            {1,4,10,26},
            {2,8,12,16},
            {3,10,17,23},
            {4,4,24,30},
            {5,7,28,32}}; 
    
    sort(_items.begin(), _items.end(), smallerEntry);
    
    if (reduced)
        _graph = buildReducedGraph();
    else
        _graph = buildGraph();   
}

TemporalBPData::TemporalBPData(){       ///< parser not implemented yet
    TemporalBPData(false);
}

TemporalBPData::TemporalBPData(std::ifstream &in, bool reduced){
    int nbItems;
    in >> nbItems;
    in >> _capacity;
    int id;
    int entry;
    int exit;
    int size;

    in >> entry;
    in >> exit;

    for (unsigned int i = 0; i < nbItems; ++i) {
        in >> id;
        in >> entry;
        in >> exit;
        in >> size;
        _items.push_back(Item(id, size, entry, exit));
    }

    sort(_items.begin(), _items.end(), smallerEntry);

    for (unsigned int i = 0; i < nbItems; ++i) { // changing IDs to chronological entry order
        _items[i]._id = i;
    }

    if (reduced)
        _graph = buildReducedGraph();
    else
        _graph = buildGraph();    
}


TemporalBPData::TemporalBPData(std::ifstream &in){
    TemporalBPData(in, false);
}

/**
 * Determines whether an item's id is present in a list
 */
bool isItemIn(const int & id, const vector<int> & ids){
    for (const auto & i : ids)
        if (i == id) return true;
    return false;
}

/**
 * Computes the maximum cliques within our list of items
 */
vector<vector<int>> TemporalBPData::getMaxCliques(){
    vector<vector<int>> cliques;   ///< list of maximum cliques
    vector<int> ongoing;           ///< list of items who have yet to exit
    for (const auto & curItem : _items) {   ///< we iterate over items sorted by increasing entry date
        for (int j=ongoing.size()-1; j>=0; j--) {   ///<we iterate over ongoing items
            Item & pastItem = _items[ongoing[j]];
            if (pastItem._exit <= curItem._entry) {   ///< if it is time for an ongoing item to leave
                if ( cliques.size()==0 || !isItemIn(ongoing[ongoing.size()-1], cliques[cliques.size()-1])){
                    ///< if the last item added in the ongoing list does not belong in our latest clique
                    vector<int> clique = ongoing;      ///< then we have a new clique with at least the last item of ongoing
                    cliques.push_back(clique);         //< as a new item in a clique.   
                }
                ongoing.erase(ongoing.begin()+j);
            }
        }
        ongoing.push_back(curItem._id);
    }

    cliques.push_back(ongoing);     ///< the final items present since the last entry also form a clique
    return cliques;
}



/**
 * Determines whether a selection of items can be packed into one bin
 * without violating the capacity constraint
 */
bool TemporalBPData::isFeasible(vector<int> & selection){
    sort(selection.begin(), selection.end());    ///< Sorting items by entry date from IDs since these were assigned by increasing entry date
    int weight = 0;
    vector<int> ongoing;
    for (const auto & itemId : selection) {
        const Item & curItem = _items[itemId];
        weight += curItem._size;
        if (weight > getCapacity()) {          ///< to save time, we only check if any item has left the bin
            for (int j=ongoing.size()-1; j>=0; j--) {   //< when the current total weight surpasses the bin capacity
                const Item & pastItem = _items[ongoing[j]];
                if (pastItem._exit <= curItem._entry) {
                    weight -= pastItem._size;
                    ongoing.erase(ongoing.begin()+j);
                }
            }
        }
        if (weight > getCapacity()) return false;

        ongoing.push_back(itemId);
    }
    return true;
}

/**
 * Computes a list of all feasible combinations of items within a clique.
 * Each combination must respect the capacity constraint
 */
vector<vector<int>> TemporalBPData::getFeasibleCombinations(vector<int> clique){
    vector<vector<int>> group;
    if (clique.empty()) {
        vector<int> empty;
        group.push_back(empty);
    }else{
        int itemId = clique[clique.size()-1];
        clique.pop_back();

        for (vector<int> combo : getFeasibleCombinations(clique)) {
            group.push_back(combo);
            combo.push_back(itemId);
            if (isFeasible(combo)) {
                group.push_back(combo);
            }
        }
    }

    return group;
}

bool isSuccessor(const Vertex & v, const vector<int> & include, const vector<int> & exclude){
    for (const int & it : include) {
        if (!isItemIn(it, v._items)) {
            return false;
        }
    }
    for (const int & it : exclude) {
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
vector<vector<Vertex>> TemporalBPData::buildGraph(){
    vector<vector<int>> cliques = getMaxCliques();
    int maxNbCombosClique = 0;
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
        vector<vector<int>> combos = getFeasibleCombinations(cliques[a]);
        if (combos.size() > maxNbCombosClique)
        {
            maxNbCombosClique = combos.size();
        }
        
        vector<Vertex> column;
        for (int c=combos.size()-1; c>=0; c--) {
            vector<int> selection = combos[c];
            Vertex u;   ///< for each feasible combination of items in a clique, we create a vertex
            u._items = selection;
            u._id = nbVertices;
            if (a==cliques.size()-1) {  ///< any vertex constructed in the last clique connects to the sink
                Arc toSink = {sink._id};
                u._arcs.push_back(toSink);
            }else{  ///< otherwise that vertex must connect to a vertex in the next clique (previous clique in our exploration order)
                vector<int> exclude;   ///< vertices containing these items should be excuded
                vector<int> include;   ///< vertices containing these items should be incuded
                for (const auto & it : selection) {
                    if (isItemIn(it, cliques[a+1])) {
                        ///< if an item in our vertex exist in the next clique
                        include.push_back(it);  ///< then we can only connect to vertices containing that item in the next clique
                    }
                }
                for (const auto & it : cliques[a+1]) {
                    if (isItemIn(it, cliques[a]) && !isItemIn(it, selection)) {
                        ///< if an item exists in the current clique but not in our vertex
                        exclude.push_back(it);  ///< then we cannot connect to vertices containing that item in the next clique
                    }
                }
                ///< We create an arc connecting to any vertex that respects our inclusion and exclusion requirements in the next clique
                for (Vertex v : graph[graph.size()-1]) {
                    if (isSuccessor(v, include, exclude)) {
                        vector<int> new_items; ///< we check what items are new between u and v to add them to the arc
                        for (const auto & it : v._items) {
                            if (!isItemIn(it, u._items)) {
                                new_items.push_back(it);
                            }
                        }
                        Arc toNext = {v._id, new_items};
                        u._arcs.push_back(toNext);
                    }
                }
            }
            if (a==0) { ///< any vertex constructed in the first clique connects to the start
                Arc fromStart = {u._id, u._items};
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

    _maxNbCombosClique = maxNbCombosClique;

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



vector<vector<int>> TemporalBPData::getReducedCliques()
{
    vector<vector<int>> cliques = getMaxCliques();
    //for (int a=cliques.size()-1; a>=1; a--) {
    for (int a=1; a<cliques.size(); a++) {
        //cout << "Clique " << a << " : " << listi_items(cliques[a]) << endl;
        for (int it=cliques[a].size()-1; it>=0; it--)
        {
            const int & item = cliques[a][it];
            //cout << "item considered: " << item << endl;
            if ((isItemIn(item,cliques[a-1]) && a==cliques.size()-1)
             || (isItemIn(item,cliques[a-1]) && !isItemIn(item, cliques[a+1])))
            {
                //cout << "item " << item << " to be removed from clique " << a << endl;
                cliques[a].erase(cliques[a].begin()+it);
            }
            
        }
    }
    return cliques;
}

vector<vector<Vertex>> TemporalBPData::buildReducedGraph(){
    cout << "here";
    vector<vector<int>> reducedCliques = getReducedCliques();

    int maxNbCombosClique = 0;
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
    for (int a=reducedCliques.size()-1; a>=0; a--) {
        vector<vector<int>> combos = getFeasibleCombinations(reducedCliques[a]);
        if (combos.size() > maxNbCombosClique)
        {
            maxNbCombosClique = combos.size();
        }
        
        vector<Vertex> column;
        for (int c=combos.size()-1; c>=0; c--) {
            vector<int> selection = combos[c];
            Vertex u;   ///< for each feasible combination of items in a clique, we create a vertex
            u._items = selection;
            u._id = nbVertices;
            if (a==reducedCliques.size()-1) {  ///< any vertex constructed in the last clique connects to the sink
                Arc toSink = {sink._id};
                u._arcs.push_back(toSink);
            }else{  ///< otherwise that vertex must connect to a vertex in the next clique (previous clique in our exploration order)
                ///< We create an arc connecting to any vertex in the next clique
                /*for (Vertex v : graph[graph.size()-1]) {
                    Arc toNext = {v._id, v._items};
                    u._arcs.push_back(toNext);
                }*/
                vector<int> exclude;   ///< vertices containing these items should be excuded
                vector<int> include;   ///< vertices containing these items should be incuded
                for (const auto & it : selection) {
                    if (isItemIn(it, reducedCliques[a+1])) {
                        ///< if an item in our vertex exist in the next clique
                        include.push_back(it);  ///< then we can only connect to vertices containing that item in the next clique
                    }
                }
                for (const auto & it : reducedCliques[a+1]) {
                    if (isItemIn(it, reducedCliques[a]) && !isItemIn(it, selection)) {
                        ///< if an item exists in the current clique but not in our vertex
                        exclude.push_back(it);  ///< then we cannot connect to vertices containing that item in the next clique
                    }
                }
                ///< We create an arc connecting to any vertex that respects our inclusion and exclusion requirements in the next clique
                for (Vertex v : graph[graph.size()-1]) {
                    if (isSuccessor(v, include, exclude)) {
                        vector<int> new_items; ///< we check what items are new between u and v to add them to the arc
                        for (const auto & it : v._items) {
                            if (!isItemIn(it, u._items)) {
                                new_items.push_back(it);
                            }
                        }
                        Arc toNext = {v._id, new_items};
                        u._arcs.push_back(toNext);
                    }
                }
            }
            if (a==0) { ///< any vertex constructed in the first clique connects to the start
                Arc fromStart = {u._id, u._items};
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

    _maxNbCombosClique = maxNbCombosClique;

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






int TemporalBPData::getVertexYPos(int vertexId, int column){
    for (int v = 0; v < _graph[column].size(); v++)
    {
        if (_graph[column][v]._id == vertexId){
            return v;
        }
    }
    return -1;
    
}

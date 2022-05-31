#include "TBPdata.hpp"
#include <algorithm>
#include <vector>

class Vertice {
public:
    vector<Item> items;
    vector<Vertice> successors;
};

class TBPProblem{
public:
    TemporalBPData data;

    TBPProblem(TemporalBPData ItemData){
        data = ItemData;
    }

    bool isFeasible(vector<Item> selection){
        sort(selection.begin(), selection.end(), data.compareEntries);
        int weight = 0;
        vector<Item> ongoing;
        for (int i=0; i<selection.size(); i++) {
            Item curItem = selection[i];
            weight += curItem.size;
            if (weight > data.getCapacity()) {   
                for (int j=0; j<ongoing.size(); j++) {
                    Item pastItem = ongoing[j];
                    if (pastItem.exit <= curItem.entry) {
                        weight -= pastItem.size;
                        ongoing.erase(ongoing.begin()+j);
                    }
                }
            }
            if (weight > data.getCapacity()) return false;

            ongoing.push_back(curItem);
        }
        return true;
    }

    vector<vector<Item>> getFeasibleCombinations(vector<Item> list){
        vector<vector<Item>> group;
        if (list.empty()) {
            vector<Item> empty;
            group.push_back(empty);
        }else{
            Item item = list[list.size()-1];
            list.pop_back();

            for (vector<Item> combo : getFeasibleCombinations(list)) {
                group.push_back(combo);
                combo.push_back(item);
                if (isFeasible(combo)) {
                    group.push_back(combo);
                }
            }
        }

        return group;
    }

    bool isSuccessor(Vertice v, vector<Item> include, vector<Item> exclude){
        for (Item it : include) {
            if (!data.isItemIn(it, v.items)) {
                return false;
            }
        }
        for (Item it : exclude) {
            if (data.isItemIn(it, v.items)) {
                return false;
            }
        }
        return true;
    }

    vector<vector<Vertice>> getGraph(){
        vector<vector<Vertice>> graph;
        Vertice start;
        Vertice final;
        vector<Vertice> finalColumn;
        finalColumn.push_back(final);
        graph.push_back(finalColumn);

        vector<vector<Item>> bands = data.getMaxBands();

        for (int a=bands.size()-1; a>=0; a--) {
            vector<vector<Item>> combos = getFeasibleCombinations(bands[a]);
            vector<Vertice> column;
            for (vector<Item> selection : combos) {
                Vertice u;
                u.items = selection;
                if (a==bands.size()-1) {
                    u.successors.push_back(final);
                }else{
                    vector<Item> exclude;
                    vector<Item> include;
                    for (Item it : selection) {
                        if (data.isItemIn(it, bands[a+1])) {
                            include.push_back(it);
                        }
                    }
                    for (Item it : bands[a+1]) {
                        if (data.isItemIn(it, bands[a]) && !data.isItemIn(it, selection)) {
                            exclude.push_back(it);
                        }
                    }
                    for (Vertice v : graph[graph.size()-1]) {
                        if (isSuccessor(v, include, exclude)) {
                            u.successors.push_back(v);
                        }
                    }
                }
                if (a==0) {
                    start.successors.push_back(u);
                }
                column.push_back(u);
            }
            graph.push_back(column);
        }
        vector<Vertice> firstColumn;
        firstColumn.push_back(start);
        graph.push_back(firstColumn);

        rotate(graph.begin(), graph.end()-1, graph.end());

        return graph;
    }

};
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

    vector<Vertice> getVertices(){
        vector<Vertice> vertices;
        Vertice final;
        vertices.push_back(final);

        vector<vector<Item>> bands = data.getMaxBands();

        for (int a=bands.size()-1; a>=0; a++) {
            vector<vector<Item>> combos = getFeasibleCombinations(bands[a]);
            for (vector<Item> selection : combos) {
                Vertice v;
                v.items = selection;
                if (a==bands.size()-1) {
                    v.successors.push_back(final);
                }
            }

        }

    }
};
#include <vector>
#include <algorithm>

using namespace std;

class Item{
public:
    int size;
    int entry;
    int exit;

    bool equals(Item item){
        return this->size==item.size && this->entry==item.entry && this->exit==item.exit;
    }
};

class TemporalBPData{
public:
    int capacity;
    vector<Item> items;

    static bool compareEntries(Item i, Item j) {
	    return (i.entry < j.entry);
    }

    TemporalBPData(){
        capacity = 15;
        items = {{6,6,20}, {4,10,26}, {8,12,16}, {7,17,23}, {4,24,30}, {7,28,32}};
        sort(items.begin(), items.end(), compareEntries);
    }

    int getNbItems() const {
        return items.size();
    }
    int getCapacity() const {
        return capacity;
    }

    bool isItemIn(Item item, vector<Item> items){
        for (Item it : items) {
            if (item.equals(it)) {
                return true;
            }
        }
        return false;
    }

    vector<vector<Item>> getMaxBands(){
        vector<vector<Item>> bands;
        vector<Item> ongoing;
        for (int i=0; i<items.size(); i++) {
            Item curItem = items[i];
            for (int j=0; j<ongoing.size(); j++) {
                Item pastItem = ongoing[j];
                if (pastItem.exit <= items[i].entry && !isItemIn(pastItem, bands.back())) {
                    vector<Item> band = ongoing;
                    bands.push_back(band);
                    ongoing.erase(ongoing.begin()+j);
                }
            }
            ongoing.push_back(curItem);
        }
        bands.push_back(ongoing);
        return bands;
    }

    vector<vector<Item>> getAllCombinations(vector<Item> list){
        vector<vector<Item>> group;
        if (list.empty()) {
            vector<Item> empty;
            group.push_back(empty);
        }else{
            Item item = list[list.size()-1];
            list.pop_back();

            for (vector<Item> combo : getAllCombinations(list)) {
                group.push_back(combo);
                combo.push_back(item);
                group.push_back(combo);
            }
        }

        return group;
    }
};
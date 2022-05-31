#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

class Item{
public:
    int id;
    int size;
    int entry;
    int exit;

    bool equals(Item item){
        return this->id==item.id;
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
        items = {{1,6,6,20}, {2,4,10,26}, {3,8,12,16}, {4,7,17,23}, {5,4,24,30}, {6,7,28,32}};
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
    vector<vector<Item>> getMaxBands(){
        // Needs some corrections!!!, III(2,5) is not considered because
        vector<vector<Item>> bands;
        vector<Item> ongoing;
        for (int i=0; i<items.size(); i++) {
            Item curItem = items[i];
            //cout << "cuI " << curItem.id  << ", entry= " << curItem.entry << endl;
            for (int j=ongoing.size()-1; j>=0; j--) {
                Item pastItem = ongoing[j];
            //cout << "  paI " << pastItem.id  << ", exit= " << pastItem.exit << endl;
                if (pastItem.exit <= items[i].entry) {
                    if ( bands.size()==0 || !isItemIn(ongoing[ongoing.size()-1], bands[bands.size()-1])){
                    //Correction: if there is even 1 new element in ongoing whenever any one element
                    // is leaving oongoing, we have a new band. DONE!
                        vector<Item> band = ongoing;
/*
                        cout << "band: ";
                        display_items(band);
                        cout << endl;
*/
                        bands.push_back(band);
                    }
                    ongoing.erase(ongoing.begin()+j);
                }
            }
            ongoing.push_back(curItem);
        }
        /*cout << "band: ";
        display_items(ongoing);
        cout << endl;*/
        bands.push_back(ongoing);
        return bands;
    }
/*
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
*/
};
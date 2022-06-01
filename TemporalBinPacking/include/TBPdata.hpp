#include <vector>
#include <algorithm>
#include <iostream>
#include <string>

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

    static bool compareEntries(Item i, Item j) {    ///< item entry date comparator used when sorting items
	    return (i._entry < j._entry);
    }

    TemporalBPData(){       ///< parser not implemented yet
        _capacity = 15;
        _items = {{1,6,6,20}, {2,4,10,26}, {3,8,12,16}, {4,7,17,23}, {5,4,24,30}, {6,7,28,32}};
        sort(_items.begin(), _items.end(), compareEntries);
    }

    int getNbItems() const { return _items.size(); }
    int getCapacity() const { return _capacity; }
    int getUpperBound()   const { return _items.size(); }

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
                    }
                    ongoing.erase(ongoing.begin()+j);
                }
            }
            ongoing.push_back(curItem);
        }
        cliques.push_back(ongoing);     ///< the final items present since the last entry also form a clique
        return cliques;
    }

};
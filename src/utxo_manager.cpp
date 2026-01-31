#include "utxo_manager.h"

//UTXO Implementation
UTXO::UTXO(){}

UTXO::UTXO(string tx_id, string owner_id, double amount, int index){
    this->tx_id = tx_id;
    this->owner_id = owner_id;
    this->amount = amount;
    this->index = index;
}

string UTXO::getTxId() const { return this->tx_id; }

int UTXO::getIndex() const { return this->index; }

string UTXO::getOwnerId() const { return this->owner_id; }

double UTXO::getAmountInUTXO() const { return this->amount; }

void UTXO::printData(){
    cout << tx_id << " " << index << " " << owner_id << " " << amount << endl;
}


// UTXO_manager Implementation
bool UTXO_manager::add_utxo(string tx_id, string owner_id, double amount, int index){
    UTXO newUTXO(tx_id, owner_id, amount, index);
    pair<string,int> p = {tx_id, index};
    
    if(UTXO_set.find(p) != UTXO_set.end()){
        printErrorMessage("UTXO already exists");
        return false;
    }
    
    UTXO_set[p] = newUTXO;
    return true;
}

bool UTXO_manager::remove_utxo(string tx_id, int index){
    pair<string,int> p = {tx_id, index};
    
    if(UTXO_set.find(p) == UTXO_set.end()){
        printErrorMessage("Removing unknown transaction");
        return false;
    }
    
    UTXO_set.erase(p);
    return true;
}

double UTXO_manager::get_balance(string user_id) const {
    double totalBalance = 0.0;
    for(const auto& it : UTXO_set){
        if(it.second.getOwnerId() == user_id){
            totalBalance += it.second.getAmountInUTXO();
        }
    }
    return totalBalance;
}

bool UTXO_manager::is_unspent(string tx_id, int index) const {
    pair<string,int> p = {tx_id, index};
    return UTXO_set.find(p) != UTXO_set.end();
}

vector<UTXO> UTXO_manager::getUTXOs(string user_id){
    vector<UTXO> utxosOfUser;
    for(const auto& it : UTXO_set){
        if(it.second.getOwnerId() == user_id){
            utxosOfUser.push_back(it.second);
        }
    }
    return utxosOfUser;
}

bool UTXO_manager::isBelongsTo(string tx_id, int index, string owner_id){
    pair<string,int> p = {tx_id, index};
    if(UTXO_set.find(p) != UTXO_set.end()){
        return UTXO_set[p].getOwnerId() == owner_id;
    }
    return false;
}

double UTXO_manager::getUTXOValue(string tx_id, int index){
    pair<string,int> p = {tx_id, index};
    if(UTXO_set.count(p) == 0){
        return 0.0;
    }
    return UTXO_set[p].getAmountInUTXO();
}

vector<UTXO> UTXO_manager::getAllUTXOs(){
    vector<UTXO> res;
    for(auto& it : UTXO_set){
        res.push_back(it.second);
    }
    return res;
}
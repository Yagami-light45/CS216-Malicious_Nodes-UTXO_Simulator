#ifndef UTXO_MANAGER_H
#define UTXO_MANAGER_H

#include "utils.h"

// Represents an UTXO.
class UTXO {
    private:
        string tx_id;
        int index;
        string owner_id;
        double amount;
    public:
        UTXO();
        UTXO(string tx_id, string owner_id, double amount, int index);
        
        string getTxId() const;
        int getIndex() const;
        string getOwnerId() const;
        double getAmountInUTXO() const;
        void printData();
};

// Manages the UTXOs set
class UTXO_manager {
    private:
        // Maps (Transaction ID, Index)
        map<pair<string,int>, UTXO> UTXO_set;
    public: 
        bool add_utxo(string tx_id, string owner_id, double amount, int index);
        bool remove_utxo(string tx_id, int index);
        
        double get_balance(string user_id) const;
        bool is_unspent(string tx_id, int index) const;
        
        vector<UTXO> getUTXOs(string user_id);
        bool isBelongsTo(string tx_id, int index, string owner_id);
        double getUTXOValue(string tx_id, int index);
        vector<UTXO> getAllUTXOs();
};

#endif
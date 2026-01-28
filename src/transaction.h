#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "utils.h"

class TransactionInputs {
    private:
        string tx_id;
        int index;
        string owner_id;
    public:
        TransactionInputs();
        //SETTERS
        void setTx_id(string s);
        void setIndex(int n);
        void setOwnerId(string s);
        //GETTERS
        string getTx_id() const;
        string getOwner_id() const;
        int getIndex() const;
};
//OUTPUT STRUCTURE
class TransactionOutputs {
    private:
        double amount;
        string address;
    public: 
        TransactionOutputs();
        void setAmount(double amount);
        void setOwnerId(string owner_id);
        double getAmount() const;
        string getAddress() const;
};

// The main Transaction container
class Transaction {
    private:
        string tx_id;
        vector<TransactionInputs> inputs;
        vector<TransactionOutputs> outputs;
    public:
        Transaction();
        Transaction(string tx_id, vector<TransactionInputs> inputs, vector<TransactionOutputs> outputs);
        
        vector<TransactionInputs> getTransactionInputs() const;
        vector<TransactionOutputs> getTransactionOutputs() const;
        string getTx_id() const;
};

#endif
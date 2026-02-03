#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "utils.h"

// Represents an input to a transaction
class TransactionInputs {
    private:
        string tx_id;
        int index;
        string owner_id;
    public:
        TransactionInputs();
        TransactionInputs(string tx_id, int index, string owner_id);
        //SETTERS
        void setTx_id(const string& s);
        void setIndex(int n);
        void setOwnerId(string s);
        //GETTERS
        string getTx_id() const;
        string getOwner_id() const;
        int getIndex() const;
        
        void printData();
};

// Represents an output of a transaction
class TransactionOutputs {
    private:
        double amount;
        string address;
    public: 
        TransactionOutputs();
        TransactionOutputs(double amount, string owner_id);
        
        void setAmount(double amount);
        void setOwnerId(string owner_id);
        
        double getAmount() const;
        string getAddress() const;
        
        void printData();
};

// Represents a transaction
class Transaction {
    private:
        string tx_id;
        vector<TransactionInputs> inputs;
        vector<TransactionOutputs> outputs;
    public:
        Transaction();
        Transaction(string& tx_id, vector<TransactionInputs>& inputs, vector<TransactionOutputs>& outputs);
        
        vector<TransactionInputs> getTransactionInputs() const;
        vector<TransactionOutputs> getTransactionOutputs() const;
        string getTx_id() const;
        
        void printData();
};

#endif
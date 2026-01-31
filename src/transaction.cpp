#include "transaction.h"

// TransactionInputs Implementation 
TransactionInputs::TransactionInputs(){}

TransactionInputs::TransactionInputs(string tx_id, int index, string owner_id){
    this->tx_id = tx_id;
    this->index = index;
    this->owner_id = owner_id;
}

void TransactionInputs::setTx_id(string s) { this->tx_id = s; }
void TransactionInputs::setIndex(int n) { this->index = n; }
void TransactionInputs::setOwnerId(string s) { this->owner_id = s; }

string TransactionInputs::getTx_id() const { return this->tx_id; }
string TransactionInputs::getOwner_id() const { return this->owner_id; }
int TransactionInputs::getIndex() const { return this->index; }

void TransactionInputs::printData(){
    cout << tx_id << " " << index << " " << owner_id << endl;
}


//TransactionOutputs Implementation 
TransactionOutputs::TransactionOutputs(){};

TransactionOutputs::TransactionOutputs(double amount, string owner_id){
    this->amount = amount;
    this->address = owner_id;
}

void TransactionOutputs::setAmount(double amount) { this->amount = amount; }
void TransactionOutputs::setOwnerId(string owner_id) { this->address = owner_id; }

double TransactionOutputs::getAmount() const { return this->amount; }
string TransactionOutputs::getAddress() const { return this->address; }

void TransactionOutputs::printData(){
    cout << amount << " btc " << address << endl;
}


// Transaction Implementation
Transaction::Transaction(){};

Transaction::Transaction(string tx_id, vector<TransactionInputs> inputs, vector<TransactionOutputs> outputs){
    this->tx_id = tx_id;
    this->inputs = inputs;
    this->outputs = outputs;
}

vector<TransactionInputs> Transaction::getTransactionInputs() const { return this->inputs; }
vector<TransactionOutputs> Transaction::getTransactionOutputs() const { return this->outputs; }
string Transaction::getTx_id() const { return this->tx_id; }

void Transaction::printData(){
    cout << "Transaction ID " << tx_id << endl;
    cout << "Inputs: " << endl;
    for(auto& it : inputs){
        it.printData();
    }
    cout << "Outputs: " << endl;
    for(auto& it : outputs){
        it.printData();
    }
}
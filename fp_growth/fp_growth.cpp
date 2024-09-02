#include<bits/stdc++.h>
using namespace std;
int global_id = 0;
class TrieNode {
public:
    string item;
    int count;
    int id;
    unordered_map<string, TrieNode*> children;
    bool isEndOfWord;

    TrieNode(const string& item) : item(item), isEndOfWord(false),count(0),id(global_id++) {}
};

class Trie {
public:
    TrieNode* root;
    string r = "root";

    Trie() {
        root = new TrieNode(r);
    }
};

class Transaction {
public:
    set<string> items;

    // Constructor to handle sets with custom comparators
    template <typename Comparator>
    Transaction(const set<string, Comparator>& items) : items(items.begin(), items.end()) {}

    // Constructor for standard sets
    Transaction(const set<string>& items) : items(items) {}
};

class Fp_growth {
public:
    vector<Transaction> transactions;
    int minSupport;
    unordered_map<string, vector<TrieNode*>> sisterNode;

    Fp_growth(const vector<Transaction>& transactions, int minSupport)
        : transactions(transactions), minSupport(minSupport) {}

    TrieNode* tree_constructor() {
        Trie* trie = new Trie();
        vector<Transaction> new_transactions = top_sorted(transactions, minSupport);
//----------------------------------------------------------------
        //printing the new transactions
        cout<<"new transactions"<<endl;
        for(Transaction& tx : new_transactions){
            for(auto& ts: tx.items){
                cout<<ts<<" ";
            }
            cout<<endl;
        }
        cout<<"________________________________________________________________"<<endl;
//---------------------------------------
        for (auto& transaction : new_transactions) {
            TrieNode* currentNode = trie->root;
            currentNode->count++; // Count the root node for each transaction
            for (auto& item : transaction.items) {
                if (currentNode->children.find(item) == currentNode->children.end()) {
                    currentNode->children[item] = new TrieNode(item);
                }
                if (sisterNode.find(item) == sisterNode.end())sisterNode[item] = vector<TrieNode*>();
                sisterNode[item].push_back(currentNode);
                currentNode = currentNode->children[item];
                currentNode->count++;
            }
            currentNode->isEndOfWord = true;
        }
        return trie->root;
    }

    vector<Transaction> top_sorted(const vector<Transaction>& transactions, int minSupport) {
        map<string, int> item_frequency;

        // Calculate frequency of each item
        for (auto& transaction : transactions) {
            for (auto& item : transaction.items) {
                item_frequency[item]++;
            }
        }
        // Print the frequency of each item
        cout<<"without min_support"<<endl;
        for(auto& item : item_frequency){
            cout<<item.first<<"-->"<<item.second<<endl;
        }
        cout<<"________________________________________________________________"<<endl;
        //Print the frequency of each item with respect to min support
        cout<<"with min_support"<<endl;
        for(auto& item : item_frequency){
            if(item.second>=minSupport)
                cout<<item.first<<"-->"<<item.second<<endl;
        }
        cout<<"________________________________________________________________"<<endl;

        // Custom comparator for sorting items based on frequency
        struct CustomComparator {
            map<string, int>& item_frequency;
            CustomComparator(map<string, int>& freq) : item_frequency(freq) {}
            bool operator()(const string& a, const string& b) const {
                return (item_frequency)[a] > (item_frequency)[b];
            }
        };

        vector<Transaction> new_transactions;

        // Sort and filter transactions based on minSupport
        for (auto& transaction : transactions) {
            set<string, CustomComparator> new_items{CustomComparator(item_frequency)};
            for (auto& item : transaction.items) {
                if (item_frequency[item] >= minSupport) {
                    new_items.insert(item);
                }
            }
            if (!new_items.empty()) {
                new_transactions.push_back(Transaction(new_items));
            }
        }
        return new_transactions;
    }

};
void print_trie(TrieNode* node, const string& prefix = "", bool isLast = true) {
        if (!node) return;

        // Print the current node
        cout << prefix;
        cout << (isLast ? "└── " : "├── ") << node->item << " (" << node->count << ")" << endl;

        // Recursively print the children nodes
        for (auto it = node->children.begin(); it != node->children.end(); ++it) {
            bool lastChild = (next(it) == node->children.end());
            print_trie(it->second, prefix + (isLast ? "    " : "│   "), lastChild);
        }
    }
void generate_dot(TrieNode* node, ofstream& out) {
    if (!node) return;
    
    for (auto& child : node->children) {
        out << "\"" 
            << "Id:" << node->id << "\\n" // First line: ID
            <<"Name:"<< node->item << "\\n"        // Second line: Item
            << "Freq:" << node->count     // Third line: Frequency
            << "\" -> \"" 
            << "Id:" << child.second->id << "\\n" // First line: ID
            <<"Name:"<< child.second->item << "\\n"        // Second line: Item
            << "Freq:" << child.second->count     // Third line: Frequency
            << "\";" << endl;

        generate_dot(child.second, out);
    }
}
void export_to_dot(TrieNode* root, const string& filename) {
    ofstream out(filename);
    out << "digraph Trie {" << endl;
    generate_dot(root, out);
    out << "}" << endl;
    out.close();
}
int main() {
    // Sample transactions
    vector<Transaction> transactions = {
        Transaction({ "I1", "I2", "I5", "I7" }),
        Transaction({ "I2", "I4", "I6" }),
        Transaction({ "I2", "I3", "I8" }),
        Transaction({ "I1", "I2", "I4", "I9" }),
        Transaction({ "I1", "I3", "I10" }),
        Transaction({ "I2", "I3", "I7" }),
        Transaction({ "I1", "I3", "I11" }),
        Transaction({ "I1", "I2", "I3", "I5", "I9" }),
        Transaction({ "I1", "I2", "I3" }),
        Transaction({ "I2", "I5", "I8", "I10" }),
        Transaction({ "I3", "I6", "I8", "I11" }),
        Transaction({ "I4", "I5", "I7" }),
        Transaction({ "I1", "I3", "I7", "I11" }),
        Transaction({ "I2", "I4", "I9", "I10" }),
        Transaction({ "I1", "I2", "I8", "I10" })
    };

    int minSupport = 5;

    // Create Fp_growth object and build the tree
    Fp_growth fp(transactions, minSupport);
    TrieNode* root = fp.tree_constructor();

    // Print the Trie structure
    cout << "Trie structure:\n";
    print_trie(root);
    export_to_dot(root, "trie.dot");
    //running bash command
    int result = system("dot -Tpng trie.dot -o trie.png");

    if (result == 0) {
        cout << "Trie successfully exported to trie.png" << endl;
    } else {
        cout << "Error: Failed to generate the PNG image." << endl;
    }
    return 0;
}
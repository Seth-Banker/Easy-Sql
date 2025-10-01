// B+ Tree
#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
using namespace std;

namespace EasyTables {

    // Types default to strings. NA is usually used for errors.
    enum NType {
        INT, // int64_t
        DBL, // double
        STR, // string
        BOOL, // boolean
        NA // Not Available, Unknown type, Error
    };

    // Clean these up later.
    extern NType strToNType(const string& typeStr);
    extern string nTypeToStr(const NType& type);
    extern bool isValidType(const string& str, const NType& type);
    extern NType whatType(const string& str);

    class Node {
    public:
        int order;

        
        vector<vector<string>> values;// Each value index is a Row of values.

        vector<Node*> children; // For value nodes.

        vector<vector<string>> keys; // For leaf nodes.
        Node* nextKey;
        Node* parent;
        bool check_leaf;
        Node(int order) {
            this->order = order;
            this->nextKey = nullptr;
            this->parent = nullptr;
            this->check_leaf = false;
        }

        /*
        Most code from here down is almost always changing. This will be annotated on a more stable verison.
        */

        void insert_at_leaf(Node* leaf, vector<string> value, string key) {
            if (!values.empty()) {
                for (int i = 0; i < values.size(); i++) {
                    if (value == values[i]) {
                        keys[i].push_back(key);
                        break;
                    }
                    else if (value < values[i]) {
                        values.insert(values.begin() + i, vector<string>{value});
                        keys.insert(keys.begin() + i, vector<string>{key});
                        break;
                    }
                    else if (i + 1 == values.size()) {
                        values.push_back(vector<string>{value});
                        keys.push_back(vector<string>{key});
                        break;
                    }
                }
            }
            else {
                values.push_back(vector<string>{value});
                keys.push_back(vector<string>{key});
            }
        }

        // Delete from leaf node
        bool delete_from_leaf(const vector<string>& value, const string& key) {
            for (int i = 0; i < values.size(); i++) {

                if (values[i] == value) { // Compare entire vectors

                    for (int j = 0; j < keys[i].size(); j++) {
                        if (keys[i][j] == key) {
                            keys[i].erase(keys[i].begin() + j);
                            // If no more keys for this value, remove the value entirely
                            if (keys[i].empty()) {
                                values.erase(values.begin() + i);
                                keys.erase(keys.begin() + i);
                            }
                            return true;
                        }
                    }
                }
            }
            return false;
        }
    };

    // B+ tree
    class BplusTree {
    public:

        Node* root;
        vector<NType> types;
        int degree;
        string name;

        BplusTree(string name_ = "default", int degree_ = 5, vector<NType> types_ = { STR, STR}) {
            root = new Node(degree);
            root->check_leaf = true;

			degree = degree_;
            types = types_;
			name = name_;
        }

        // insert operation
        void insert(const vector<string>& value) {

			string key = value[0];
            Node* old_node = search(value);
            old_node->insert_at_leaf(old_node, value, key);
            if (old_node->values.size() == old_node->order) {
                Node* node1 = new Node(old_node->order);
                node1->check_leaf = true;
                node1->parent = old_node->parent;
                int mid = ceil(old_node->order / 2.0) - 1;
                node1->values.assign(old_node->values.begin() + mid + 1, old_node->values.end());
                node1->keys.assign(old_node->keys.begin() + mid + 1, old_node->keys.end());
                node1->nextKey = old_node->nextKey;
                old_node->values.resize(mid + 1);
                old_node->keys.resize(mid + 1);
                old_node->nextKey = node1;
                insert_in_parent(old_node, node1->values[0][0], node1);
            }
        }

        // outward facing delete handle
        bool delete_entry(vector<string> value) {

            string key = value[0];
			//value.erase(value.begin()); // remove key from value vector for searching

            Node* leaf = search(value);

            // key not found
            if (!leaf->delete_from_leaf(value, key)) {
                return false; 
            }

            // check for rebalancing
            int min_keys = ceil(degree / 2.0) - 1;
            if (leaf == root) {
                // root can have fewer keys
                if (leaf->values.empty() && !leaf->children.empty()) {
                    root = leaf->children[0];
                    root->parent = nullptr;
                    delete leaf;
                }
                return true;
            }

            if (leaf->values.size() >= min_keys) {
                return true; // no underflow
            }

            // Handle underflow
            handle_underflow(leaf);
            return true;
        }

        // handle underflow in a node
        void handle_underflow(Node* node) {
            Node* parent = node->parent;
            if (parent == nullptr) return; // root

            int node_index = -1;
            for (int i = 0; i < parent->children.size(); i++) {
                if (parent->children[i] == node) {
                    node_index = i;
                    break;
                }
            }

            int min_keys = ceil(degree / 2.0) - 1;

            // Try to borrow from left sibling
            if (node_index > 0) {
                Node* left_sibling = parent->children[node_index - 1];
                if (left_sibling->values.size() > min_keys) {
                    borrow_from_left(node, left_sibling, parent, node_index);
                    return;
                }
            }

            // try to borrow from right sibling
            if (node_index < parent->children.size() - 1) {
                Node* right_sibling = parent->children[node_index + 1];
                if (right_sibling->values.size() > min_keys) {
                    borrow_from_right(node, right_sibling, parent, node_index);
                    return;
                }
            }

            // merge with sibling
            if (node_index > 0) {
                Node* left_sibling = parent->children[node_index - 1];
                merge_nodes(left_sibling, node, parent, node_index - 1);
            }
            else {
                Node* right_sibling = parent->children[node_index + 1];
                merge_nodes(node, right_sibling, parent, node_index);
            }
        }

        // Borrow from left sibling
        void borrow_from_left(Node* node, Node* left_sibling, Node* parent, int node_index) {
            if (node->check_leaf) {
                // Move the last value from left sibling to beginning of node
                node->values.insert(node->values.begin(), left_sibling->values.back());
                node->keys.insert(node->keys.begin(), left_sibling->keys.back());
                left_sibling->values.pop_back();
                left_sibling->keys.pop_back();

                // update parent key
                parent->values[node_index - 1][0] = node->values[0][0];
            }
            else {
                // For internal nodes
                node->values.insert(node->values.begin(), parent->values[node_index - 1]);
                parent->values[node_index - 1] = left_sibling->values.back();
                left_sibling->values.pop_back();

                node->children.insert(node->children.begin(), left_sibling->children.back());
                left_sibling->children.pop_back();
                node->children[0]->parent = node;
            }
        }

        // Borrow from right sibling
        void borrow_from_right(Node* node, Node* right_sibling, Node* parent, int node_index) {
            if (node->check_leaf) {
                // Move the first value from right sibling to end of node
                node->values.push_back(right_sibling->values[0]);
                node->keys.push_back(right_sibling->keys[0]);
                right_sibling->values.erase(right_sibling->values.begin());
                right_sibling->keys.erase(right_sibling->keys.begin());

                // Update parent key
                parent->values[node_index][0] = right_sibling->values[0][0];
            }
            else {
                // For internal nodes
                node->values.push_back(parent->values[node_index]);
                parent->values[node_index] = right_sibling->values[0];
                right_sibling->values.erase(right_sibling->values.begin());

                node->children.push_back(right_sibling->children[0]);
                right_sibling->children.erase(right_sibling->children.begin());
                node->children.back()->parent = node;
            }
        }

        // Merge two nodes
        void merge_nodes(Node* left_node, Node* right_node, Node* parent, int index) {
            if (left_node->check_leaf) {
                // Merge leaf nodes
                left_node->values.insert(left_node->values.end(), right_node->values.begin(), right_node->values.end());
                left_node->keys.insert(left_node->keys.end(), right_node->keys.begin(), right_node->keys.end());
                left_node->nextKey = right_node->nextKey;
            }
            else {
                // Merge internal nodes
                left_node->values.push_back(parent->values[index]);
                left_node->values.insert(left_node->values.end(), right_node->values.begin(), right_node->values.end());
                left_node->children.insert(left_node->children.end(), right_node->children.begin(), right_node->children.end());

                // Update parent pointers
                for (Node* child : right_node->children) {
                    child->parent = left_node;
                }
            }

            // Remove the separator key from parent
            parent->values.erase(parent->values.begin() + index);
            parent->children.erase(parent->children.begin() + index + 1);

            delete right_node;

            // Check if parent needs rebalancing
            int min_keys = ceil(degree / 2.0) - 1;
            if (parent == root) {
                if (parent->values.empty()) {
                    root = left_node;
                    root->parent = nullptr;
                    delete parent;
                }
            }
            else if (parent->values.size() < min_keys) {
                handle_underflow(parent);
            }
        }

        // search operation for different operations
        Node* search(vector<string> value) {
            Node* current_node = root;
            while (!current_node->check_leaf) {
                for (int i = 0; i < current_node->values.size(); i++) {
                    if (value == current_node->values[i]) {
                        current_node = current_node->children[i + 1];
                        break;
                    }
                    else if (value < current_node->values[i]) {
                        current_node = current_node->children[i];
                        break;
                    }
                    else if (i + 1 == current_node->values.size()) {
                        current_node = current_node->children[i + 1];
                        break;
                    }
                }
            }
            return current_node;
        }

        vector<string> searchByKey(string value) {
            Node* current_node = root;
            while (!current_node->check_leaf) {
                for (int i = 0; i < current_node->values.size(); i++) {
                    if (value == current_node->values[i][0]) {
                        current_node = current_node->children[i + 1];
                        break;
                    }
                    else if (value < current_node->values[i][0]) {
                        current_node = current_node->children[i];
                        break;
                    }
                    else if (i + 1 == current_node->values.size()) {
                        current_node = current_node->children[i + 1];
                        break;
                    }
                }
            }

            for (int i = 0; i < current_node->values.size(); i++) {
                if (current_node->values[i][0] == value) {
                    return current_node->values[i];
                }
            }

            return {};
        }

        // find the node
        bool find(vector<string> value) {

			string key = value[0];

            Node* l = search(value);
            for (int i = 0; i < l->values.size(); i++) {
                if (!l->values[i].empty() && l->values[i] == value) {
                    for (int j = 0; j < l->keys[i].size(); j++) {
                        if (l->keys[i][j] == key) {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        // inserting at the parent
        void insert_in_parent(Node* n, string value, Node* ndash) {
            if (root == n) {
                Node* rootNode = new Node(n->order);
                rootNode->values.push_back(vector<string>{value});
                rootNode->children.push_back(n);
                rootNode->children.push_back(ndash);
                root = rootNode;
                n->parent = rootNode;
                ndash->parent = rootNode;
                return;
            }
            Node* parentNode = n->parent;
            for (int i = 0; i < parentNode->children.size(); i++) {
                if (parentNode->children[i] == n) {
                    parentNode->values.insert(parentNode->values.begin() + i, vector<string>{value});
                    int mid = ceil(parentNode->order / 2.0) - 1;
                    parentNode->values.insert(parentNode->values.begin() + i, vector<string>{value});
                    string value_ = parentNode->values[mid][0];
                    parentNode->children.insert(parentNode->children.begin() + i + 1, ndash);
                    if (parentNode->children.size() > parentNode->order) {
                        Node* parentdash = new Node(parentNode->order);
                        parentdash->parent = parentNode->parent;
                        parentdash->values.assign(parentNode->values.begin() + mid + 1, parentNode->values.end());
                        parentdash->children.assign(parentNode->children.begin() + mid + 1, parentNode->children.end());
                        string value_ = parentNode->values[mid][0];
                        parentNode->values.resize(mid);
                        parentNode->children.resize(mid + 1);
                        insert_in_parent(parentNode, value_, parentdash);
                    }
                    break;
                }
            }
        }
    };
}
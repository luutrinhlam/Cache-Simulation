#ifndef CACHE_H
#define CACHE_H

#include "main.h"

class Node {    /// Node for replacement polices
public:
    Elem *value;
    Node *prev, *next;
    int freq;

    Node(Elem *v) : value(v), prev(nullptr), next(nullptr) {}

    Node() : value(nullptr), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {
    Node *head, *tail;
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    ~DoublyLinkedList() {
        Node *temp = head;
        while (temp) {
            delete temp->value;
            Node *temp1 = temp;
            temp = temp->next;
            delete temp1;
        }
    }

    Node **addToHead(Elem *value) {
        Node *temp = new Node(value);
        if (!head && !tail) {
            head = tail = temp;
        } else {
            temp->next = head;
            head->prev = temp;
            head = temp;
        }
        Node **x = &temp;
        return x;
    }

    Node **addToTail(Elem *value) {
        Node *temp = new Node(value);
        if (!head && !tail) {
            head = tail = temp;
        } else {
            temp->prev = tail;
            tail->next = temp;
            tail = temp;
        }
        Node **x = &temp;
        return x;
    }

    void moveToHead(Node *&temp) {
        if (temp == head) {
            return;
        }
        if (temp == tail) {
            tail = tail->prev;
            tail->next = nullptr;
        } else {
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;
        }

        temp->next = head;
        temp->prev = nullptr;
        head->prev = temp;
        head = temp;
    }

    Elem *removeTail() {
        if (head == tail) {
            tail->next = nullptr;
            tail->prev = nullptr;
            Elem *temp = tail->value;
            delete tail;
            head = tail = nullptr;
            return temp;
        } else {
            Node *temp = tail;
            tail = tail->prev;
            tail->next = nullptr;
            Elem *returnElem = temp->value;
            delete temp;
            return returnElem;
        }
    }

    Elem *removeHead() {
        if (head == tail) {
            tail->next = nullptr;
            tail->prev = nullptr;
            Elem *returnElem = tail->value;
            delete tail;
            head = tail = nullptr;
            return returnElem;
        } else {
            Node *temp = head;
            head = head->next;
            head->prev = nullptr;
            Elem *returnElem = temp->value;
            delete temp;
            return returnElem;
        }
    }

    void print() {
        Node *temp = head;
        while (temp) {
            temp->value->print();
            temp = temp->next;
        }
    }
};

class ReplacementPolicy {
protected:
    int count;

public:
    virtual void update(Node *&temp) = 0;

    virtual Elem *remove() = 0; ///just need the
    virtual Node **insert(Elem *elem) = 0;  /// return a Node in RP to insert to SE
    virtual void print() = 0;

    virtual ~ReplacementPolicy() = 0;

    bool isFull() { return count == MAXSIZE; }

    bool isEmpty() { return count == 0; }
};

ReplacementPolicy::~ReplacementPolicy() {}

////////////////////////
class FIFO : public ReplacementPolicy {
    DoublyLinkedList *list;
public:
    FIFO() {
        count = 0;
        list = new DoublyLinkedList();
    }

    void update(Node *&temp) {

    };

    Elem *remove() {
        count--;
        return list->removeHead();
    };

    Node **insert(Elem *elem) {
        count++;
        return list->addToTail(elem);
    };

    void print() {
        list->print();
    };

    ~FIFO() {
        delete list;
    };
}; ///done
class MRU : public ReplacementPolicy {
    DoublyLinkedList *list;
public:
    MRU() {
        count = 0;
        list = new DoublyLinkedList();
    }

    void update(Node *&temp) {
        if (temp == nullptr) return;
        list->moveToHead(temp);
    };

    Elem *remove() {
        count--;
        return list->removeHead();
    };

    Node **insert(Elem *elem) {
        count++;
        return list->addToHead(elem);
    };

    void print() {
        list->print();
    };

    ~MRU() {
        delete list;
    };
}; ///done
class LRU : public ReplacementPolicy {
    DoublyLinkedList *list;
public:
    LRU() {
        count = 0;
        list = new DoublyLinkedList();
    }

    void update(Node *&temp) {
        if (temp == nullptr) return;
        list->moveToHead(temp);
    };

    Elem *remove() {
        count--;
        return list->removeTail();
    };

    Node **insert(Elem *elem) {
        count++;
        return list->addToHead(elem);
    };

    void print() {
        list->print();
    };

    ~LRU() {
        delete list;
    };
}; ///done
class LFU : public ReplacementPolicy {
    Node **arr;

    void heapDown(int n, int i) {
        int largest = i;
        int l = 2 * i + 1; // left
        int r = 2 * i + 2; // right
        if (r < n && arr[largest]->freq >= arr[r]->freq) {   //compare to the right
            largest = r;
        }
        if (l < n && arr[largest]->freq >= arr[l]->freq) {   //compare to the left
            largest = l;
        }
        if (largest != i) {
            //swap(arr[i], arr[largest]);
            {
                Node *x = arr[i];
                arr[i] = arr[largest];
                arr[largest] = x;
            }
            heapDown(n, largest);
        }
    }

    void heapUp(int n, int i) {
        if (i == 0)
            return;
        int par = (i % 2 == 1) ? i / 2 : i / 2 - 1;
        if (arr[i]->freq < arr[par]->freq) {
            {
                Node *x = arr[i];
                arr[i] = arr[par];
                arr[par] = x;
            }
            heapUp(n, par);
        }
    }

public:
    LFU() {
        count = 0;
        arr = new Node *[MAXSIZE];
    }

    ~LFU() {
        for (int i = 0; i < count; i++) {
            delete arr[i]->value;
            delete arr[i];
        }
        delete[] arr;
    }

    void update(Node *&a) {

        a->freq++;
        int i;
        //i = (arr[0] - a) / sizeof(Node *);
        for (i = 0; i < count; i++) {
            if (arr[i]->value->addr == a->value->addr) break;
        }

        heapDown(count, i);

    };

    Elem *remove() {
        Elem *returnValue = arr[0]->value;
        delete arr[0];
        arr[0] = arr[count - 1];
        count--;
        heapDown(count, 0);

        return returnValue;
    };

    Node **insert(Elem *elem) {
        Node *temp = new Node(elem);
        temp->freq = 0;
        arr[count] = temp;
        count++;
        heapUp(count, count - 1);
        Node **x = &temp;
        return x;
    };

    void print() {
        for (int i = 0; i < count; i++) {
            arr[i]->value->print();
        }
    };
}; ///done


class node {
public:
    Elem *elem;
    node *left;
    node *right;
    int height;
    Node *NodeInRP;

    friend class AVLTree;
    friend class DBHashing;
    friend  class AVL;

public:
    node(Elem *elem, node *left, node *right, int height, Node *&RP_Node) {
        //cout << "create node: " <<elem->addr <<endl;
        this->elem = elem;
        this->left = left;
        this->right = right;
        this->height = height;
        this->NodeInRP = RP_Node;
    }

    node() {
        elem = nullptr;
        left = nullptr;
        right = nullptr;
        height = 0;
    }

    ~node() {
        //cout << "delete node: " <<elem->addr <<endl;
        if (left) delete left;
        if (right) delete right;
    }

    void reheight();

    int balance();

    node *rotate_right();

    node *rotate_left();

    node *find_left_most(node *rightchild);
};      // for AVLTree
class AVLTree {
public:
    node *root;

    AVLTree() {
        root = nullptr;
    }

    ~AVLTree() {
        delete root;
    }

    void insert(node *&root, Elem *elem, Node **RP_Node);

    void remove(node *&root, Elem *elem);

    node **search(int addr);

    void printInOrder(node *root);

    void printPreOrder(node *root);

};

class SearchEngine {
public:
    virtual ~SearchEngine() = 0;

    virtual node **search(int key) = 0; // also keep the address of Node* in RP
    virtual void insert(Node *temp) = 0;

    virtual void deleteNode(Elem *elem) = 0; //because AVL take elem*
    virtual void print() = 0;
};

SearchEngine::~SearchEngine() {}

class AVL : public SearchEngine {
    AVLTree *avl;
public:
    AVL() {
        avl = new AVLTree();
    }

    ~AVL() { delete avl; }

    void insert(Node *temp) {
        if (temp == nullptr) return;
        avl->insert(avl->root, temp->value, &temp);
    }

    void deleteNode(Elem *elem) {
        avl->remove(avl->root, elem);
    }

    void print() {
        cout << "Print AVL in inorder:\n";
        avl->printInOrder(avl->root);
        cout << "Print AVL in preorder:\n";
        avl->printPreOrder(avl->root);
    }

    node **search(int key) {

        node **temp = avl->search(key);
        if (temp) {
            return temp;
        } else return nullptr;
    }
};

class DBHashing : public SearchEngine {
    class HashNode {
        int status;
        /// i = 0 : empty
        /// i = 1 : occupied
        /// i = 2 : deleted
        node *nodeData;

        friend class DBHashing;

        HashNode(node *nD) : status(0), nodeData(nD) {}

        ~HashNode() { if (nodeData) delete nodeData; }
    };

    HashNode **hashTable;

    int (*H1)(int a);

    int (*H2)(int a);

    int tableSize;
public:
    DBHashing(int (*h1)(int), int (*h2)(int), int s) {
        hashTable = new HashNode *[s];
        H1 = h1;
        H2 = h2;
        tableSize = s;
        for (int i = 0; i < tableSize; i++) {
            hashTable[i] = new HashNode(nullptr);
        }
    }

    ~DBHashing() {
        for (int i = 0; i < tableSize; i++) {
            delete hashTable[i];
        }
        delete[] hashTable;
    }

    node **search(int key) {
        int h1 = (*H1)(key);
        int h2 = (*H2)(key);
        node **result = nullptr;
        for (int i = 0; i < tableSize; i++) {
            int hashValue = (h1 + i * h2) % tableSize;
            if (hashTable[hashValue]->status == 0) { //empty
                return result;
            } else if (hashTable[hashValue]->status == 1) { //occupied
                if (hashTable[hashValue]->nodeData->elem->addr == key) {
                    result = &hashTable[hashValue]->nodeData;
                    return result;
                } else continue;
            } else { //deleted
                continue;
            }
        }
        return nullptr;
    }; // also keep the address of Node* in RP
    void insert(Node *temp) {
        int h1 = (*H1)(temp->value->addr);
        int h2 = (*H2)(temp->value->addr);
        for (int i = 0; i < tableSize; i++) {
            int hashValue = (h1 + i * h2) % tableSize;
            if (hashTable[hashValue]->status == 1) { //occupied
                continue;
            } else { //empty or deleted
                hashTable[hashValue]->nodeData = new node(temp->value, nullptr, nullptr, 0, temp);
                hashTable[hashValue]->status = 1; //occupied
                return;
            }
        }
    };

    void deleteNode(Elem *elem) {
        int h1 = (*H1)(elem->addr);
        int h2 = (*H2)(elem->addr);
        for (int i = 0; i < tableSize; i++) {
            int hashValue = (h1 + i * h2) % tableSize;
            if (hashTable[hashValue]->nodeData == nullptr) continue;
            if (hashTable[hashValue]->nodeData->elem->addr == elem->addr) { //find the HashNode
                //hashTable[hashValue]->nodeData->elem = nullptr;
                hashTable[hashValue]->nodeData->NodeInRP = nullptr;
                delete hashTable[hashValue]->nodeData;
                hashTable[hashValue]->nodeData = nullptr;
                hashTable[hashValue]->status = 2;
                break;
            }
        }
    };

    void print() {
        cout << "Prime memory:\n";
        for (int i = 0; i < tableSize; i++) {
            if (hashTable[i]->nodeData != nullptr) {
                hashTable[i]->nodeData->elem->print();
            }
        }
    };
};


///////////////////////////////// node implement /////////////////////////////////
void node::reheight() {
    int l = -1;
    int r = -1;
    if (left) l = left->height;
    if (right) r = right->height;
    if (l > r) {
        height = l + 1;
    } else {
        height = r + 1;
    }
}

int node::balance() {
    int l = -1;
    int r = -1;
    if (left) l = left->height;
    if (right) r = right->height;
    return l - r;
}

node *node::rotate_right() {

    node *leftchild = left;
    node *temp = leftchild->right;

    this->left = temp;
    leftchild->right = this;

    this->reheight();
    leftchild->reheight();

    return leftchild;
}

node *node::rotate_left() {
    node *rightchild = right;
    node *temp = rightchild->left;

    this->right = temp;
    rightchild->left = this;

    this->reheight();
    rightchild->reheight();

    return rightchild;
}

node *node::find_left_most(node *rightchild) {
    while (rightchild->left) {
        rightchild = rightchild->left;
    }
    return rightchild;
}

///////////////////////////////// AVL Tree implement /////////////////////////////////

void AVLTree::insert(node *&root, Elem *elem, Node **RP_Node) {
    if (!root) {
        root = new node(elem, nullptr, nullptr, 0, (*RP_Node));
        return;
    }
    if (elem->addr >= root->elem->addr) {
        if (root->right) {
            insert(root->right, elem, RP_Node);
        } else {
            root->right = new node(elem, nullptr, nullptr, 0, (*RP_Node));
        }
    } else {
        if (root->left) {
            insert(root->left, elem, RP_Node);
        } else {
            root->left = new node(elem, nullptr, nullptr, 0, (*RP_Node));
        }
    }
    root->reheight();
    int bal = root->balance();
    // Left Left
    if (bal > 1 && root->left->balance() >= 0) {
        root = root->rotate_right();
    }

        // Right Right
    else if (bal < -1 && root->right->balance() <= 0)
        root = root->rotate_left();

        // Left Right
    else if (bal > 1 && root->left->balance() < 0) {
        root->left = root->left->rotate_left();
        root = root->rotate_right();
    }

        // Right Left
    else if (bal < -1 && root->right->balance() > 0) {
        root->right = root->right->rotate_right();
        root = root->rotate_left();
    }
}

void AVLTree::remove(node *&root, Elem *elem) {
    if (!root) return;
    if (elem->addr < root->elem->addr) {              //go to left
        remove(root->left, elem);
    } else if (elem->addr > root->elem->addr) {       //go to right
        remove(root->right, elem);
    } else if (elem != root->elem) {
        remove(root->left, elem);
        remove(root->right, elem);
    } else {                                               //found the node
        // node with only one child or no child
        if (!root->left || !root->right) {
            node *temp = root->left ?
                         root->left :
                         root->right;

            // No child case
            if (!temp) {
                root->NodeInRP = nullptr;
                delete root;
                root = nullptr;
            } else {
                node *x = root;
                root = temp;
                x->right = nullptr;
                x->left = nullptr;
                x->NodeInRP = nullptr;
                delete x;
            }
        } else {
            node *temp = root->find_left_most(root->right);

            root->elem = temp->elem;
            root->NodeInRP = temp->NodeInRP;

            remove(root->right, temp->elem);
        }
    }
    if (!root) return;
    root->reheight();
    int bal = root->balance();
    // Left Left
    if (bal > 1 && root->left->balance() >= 0)
        root = root->rotate_right();

        // Right Right
    else if (bal < -1 && root->right->balance() <= 0)
        root = root->rotate_left();

        // Left Right
    else if (bal > 1 && root->left->balance() < 0) {
        root->left = root->left->rotate_left();
        root = root->rotate_right();
    }

        // Right Left
    else if (bal < -1 && root->right->balance() > 0) {
        root->right = root->right->rotate_right();
        root = root->rotate_left();
    }

}

node **AVLTree::search(int addr) {
    node **result = nullptr;
    node *temp = root;
    while (temp) {
        if (temp->elem->addr == addr) {
            result = &temp;
            break;
        }
        if (addr < temp->elem->addr) temp = temp->left;
        else temp = temp->right;

    }
    return result;
}

void AVLTree::printInOrder(node *root) {
    if (!root) return;
    printInOrder(root->left);
    root->elem->print();
    printInOrder(root->right);
}

void AVLTree::printPreOrder(node *root) {
    if (!root) return;
    root->elem->print();
    printPreOrder(root->left);
    printPreOrder(root->right);
}

#endif
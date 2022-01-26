#include "main.h"

Cache::Cache(SearchEngine *s, ReplacementPolicy *r) : rp(r), s_engine(s) {

}

Cache::~Cache() {
    delete s_engine;
    delete rp;

}

Data *Cache::read(int addr) {
    //cout <<"read " << addr <<endl;
    node **temp = s_engine->search(addr);
    if (temp) {
        Data *x = (*temp)->elem->data;
        rp->update((*temp)->NodeInRP);
        return x;
    } else
    {
        return nullptr;
    }
}

Elem *Cache::put(int addr, Data *cont) {
    //cout <<"put " <<addr <<endl;
    Elem *temp = nullptr;
    if (rp->isFull()) {
        temp = rp->remove();
        s_engine->deleteNode(temp);
    }
    Elem *in = new Elem(addr, cont, true);
    Node **in1 = nullptr;
    in1 = rp->insert(in);
    s_engine->insert(*in1);
    return temp;
}

Elem *Cache::write(int addr, Data *cont) {
    //cout <<"write " << addr <<endl;
    node **temp = s_engine->search(addr);

    if (temp) {  // found a node
        node *x = *temp;
        rp->update((*temp)->NodeInRP);
        delete x->elem->data;
        x->elem->data = cont;
        x->elem->sync = false;
        return nullptr;
    } else {      // didn't find
        Elem *x = nullptr;
        if (rp->isFull()) {
            x = rp->remove();
            s_engine->deleteNode(x);
        }
        Elem *in = new Elem(addr, cont, false);
        Node **in1 = nullptr;
        in1 = rp->insert(in);
        s_engine->insert(*in1);
        return x;
    }
}

void Cache::printRP() {
    rp->print();
}

void Cache::printSE() {
    s_engine->print();
}





////////////////////////////////////////////////////////////////////////



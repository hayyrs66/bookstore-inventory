#include "BTree.h"

BTreeNode::BTreeNode(int t1, bool leaf1) {
    t = t1;
    leaf = leaf1;
}

void BTreeNode::traverse() {
    int i;
    for (i = 0; i < keys.size(); i++) {
        if (!leaf)
            children[i]->traverse();
        cout << " " << keys[i].isbn;
    }

    if (!leaf)
        children[i]->traverse();
}

BTreeNode* BTreeNode::search(const string& isbn) {
    int i = 0;
    while (i < keys.size() && isbn > keys[i].isbn)
        i++;

    if (i < keys.size() && keys[i].isbn == isbn)
        return this;

    if (leaf)
        return nullptr;

    return children[i]->search(isbn);
}

void BTree::insert(const Book& k) {
    if (root == nullptr) {
        root = new BTreeNode(t, true);
        root->keys.push_back(k);
    }
    else {
        if (root->keys.size() == 2 * t - 1) {
            BTreeNode* s = new BTreeNode(t, false);
            s->children.push_back(root);
            s->splitChild(0, root);

            int i = 0;
            if (s->keys[0].isbn < k.isbn)
                i++;
            s->children[i]->insertNonFull(k);

            root = s;
        }
        else
            root->insertNonFull(k);
    }
}

void BTreeNode::insertNonFull(const Book& k) {
    int i = keys.size() - 1;

    if (leaf) {
        keys.push_back(k);
        while (i >= 0 && keys[i].isbn > k.isbn) {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = k;
    }
    else {
        while (i >= 0 && keys[i].isbn > k.isbn)
            i--;

        if (children[i + 1]->keys.size() == 2 * t - 1) {
            splitChild(i + 1, children[i + 1]);

            if (keys[i + 1].isbn < k.isbn)
                i++;
        }
        children[i + 1]->insertNonFull(k);
    }
}

void BTreeNode::splitChild(int i, BTreeNode* y) {
    /*cout << "Iniciando splitChild..." << endl;
    cout << "Índice i: " << i << endl;
    cout << "Tamaño de children: " << children.size() << endl;
    cout << "Tamaño de keys en y: " << y->keys.size() << endl;*/

    // Asegurarse de que i y t estén dentro de los límites
    if (i < 0 || i >= children.size()) {
        //cerr << "Error: índice i fuera de los límites." << endl;
        return;
    }
    if (t <= 1) {
        //cerr << "Error: valor de t inválido." << endl;
        return;
    }

    BTreeNode* z = new BTreeNode(y->t, y->leaf);

    // Asegurarse de que y tenga suficientes claves para dividir
    if (y->keys.size() < 2 * t - 1) {
        //cerr << "Error: y no tiene suficientes claves para dividir." << endl;
        delete z;
        return;
    }

    // Almacenar la clave mediana antes de redimensionar y
    Book median = y->keys[t - 1];

    // Copiar las últimas (t-1) claves de y a z
    for (int j = 0; j < t - 1; j++) {
        if (j + t < y->keys.size()) {
            z->keys.push_back(y->keys[j + t]);
            //cout << "Copiando clave " << y->keys[j + t].isbn << " a z" << endl;
        }
        else {
            //cerr << "Error: índice j + t fuera de los límites de y->keys." << endl;
            delete z;
            return;
        }
    }

    // Copiar los últimos t hijos de y a z si y no es una hoja
    if (!y->leaf) {
        for (int j = 0; j < t; j++) {
            if (j + t < y->children.size()) {
                z->children.push_back(y->children[j + t]);
                //cout << "Copiando hijo " << j + t << " a z" << endl;
            }
            else {
                //cerr << "Error: índice j + t fuera de los límites de y->children." << endl;
                delete z;
                return;
            }
        }
    }

    // Redimensionar y para que contenga solo las primeras (t-1) claves
    y->keys.resize(t - 1);
    if (!y->leaf) {
        y->children.resize(t);
    }

    //cout << "Después de redimensionar y, tamaño de keys: " << y->keys.size() << endl;
    //cout << "Después de redimensionar y, tamaño de children: " << y->children.size() << endl;

    // Insertar el nuevo nodo z en children
    if (i + 1 <= children.size()) {
        children.insert(children.begin() + i + 1, z);
        //cout << "Insertado nodo z en children en la posición " << i + 1 << endl;
    }
    else {
        //cerr << "Error: índice i + 1 fuera de los límites de children." << endl;
        delete z; // Limpiar memoria asignada a z
        return;
    }

    // Insertar la clave mediana en el nodo actual
    if (i <= keys.size()) {
        keys.insert(keys.begin() + i, median);
        //cout << "Insertada clave mediana " << median.isbn << " en el nodo actual" << endl;
    }
    else {
        //cerr << "Error: índice i fuera de los límites de keys." << endl;
        children.erase(children.begin() + i + 1); // Revertir la inserción de z
        delete z; // Limpiar memoria asignada a z
    }

    //cout << "Finalizado splitChild" << endl;
}



void BTree::remove(const string& isbn) {
    if (!root)
        return;

    root->remove(isbn);

    if (root->keys.size() == 0) {
        BTreeNode* tmp = root;
        if (root->leaf)
            root = nullptr;
        else
            root = root->children[0];

        delete tmp;
    }
}

void BTreeNode::remove(const string& isbn) {
    int idx = findKey(isbn);

    if (idx < keys.size() && keys[idx].isbn == isbn) {
        if (leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
    }
    else {
        if (leaf)
            return;

        bool flag = (idx == keys.size());

        if (children[idx]->keys.size() < t)
            fill(idx);

        if (flag && idx > keys.size())
            children[idx - 1]->remove(isbn);
        else
            children[idx]->remove(isbn);
    }
}

int BTreeNode::findKey(const string& isbn) {
    int idx = 0;
    while (idx < keys.size() && keys[idx].isbn < isbn)
        ++idx;
    return idx;
}

void BTreeNode::removeFromLeaf(int idx) {
    keys.erase(keys.begin() + idx);
}

void BTreeNode::removeFromNonLeaf(int idx) {
    Book k = keys[idx];

    if (children[idx]->keys.size() >= t) {
        Book pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred.isbn);
    }
    else if (children[idx + 1]->keys.size() >= t) {
        Book succ = getSucc(idx);
        keys[idx] = succ;
        children[idx + 1]->remove(succ.isbn);
    }
    else {
        merge(idx);
        children[idx]->remove(k.isbn);
    }
}

Book BTreeNode::getPred(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->leaf)
        cur = cur->children[cur->keys.size()];
    return cur->keys[cur->keys.size() - 1];
}

Book BTreeNode::getSucc(int idx) {
    BTreeNode* cur = children[idx + 1];
    while (!cur->leaf)
        cur = cur->children[0];
    return cur->keys[0];
}

void BTreeNode::fill(int idx) {
    if (idx != 0 && children[idx - 1]->keys.size() >= t)
        borrowFromPrev(idx);
    else if (idx != keys.size() && children[idx + 1]->keys.size() >= t)
        borrowFromNext(idx);
    else {
        if (idx != keys.size())
            merge(idx);
        else
            merge(idx - 1);
    }
}

void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx - 1];

    child->keys.insert(child->keys.begin(), keys[idx - 1]);

    if (!child->leaf)
        child->children.insert(child->children.begin(), sibling->children[sibling->keys.size()]);

    keys[idx - 1] = sibling->keys[sibling->keys.size() - 1];

    sibling->keys.pop_back();
    if (!sibling->leaf)
        sibling->children.pop_back();
}

void BTreeNode::borrowFromNext(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys.push_back(keys[idx]);

    if (!child->leaf)
        child->children.push_back(sibling->children[0]);

    keys[idx] = sibling->keys[0];

    sibling->keys.erase(sibling->keys.begin());
    if (!sibling->leaf)
        sibling->children.erase(sibling->children.begin());
}

void BTreeNode::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx + 1];

    child->keys.push_back(keys[idx]);

    for (int i = 0; i < sibling->keys.size(); ++i)
        child->keys.push_back(sibling->keys[i]);

    if (!child->leaf) {
        for (int i = 0; i <= sibling->keys.size(); ++i)
            child->children.push_back(sibling->children[i]);
    }

    keys.erase(keys.begin() + idx);
    children.erase(children.begin() + idx + 1);

    delete sibling;
}

void BTreeNode::printNode() {
    for (int i = 0; i < keys.size(); i++) {
        if (!leaf)
            children[i]->printNode();
        cout << keys[i].toJson() << endl;
    }

    if (!leaf)
        children[keys.size()]->printNode();
}

void BTree::printTree() {
    if (root != nullptr)
        root->printNode();
}
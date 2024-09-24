#include "BTree.h"

BTreeNode::BTreeNode(int t1, bool leaf1)
{
    t = t1;
    leaf = leaf1;
    keys.reserve(2 * t - 1);
    children.reserve(2 * t);
}

void BTreeNode::traverse()
{
    int i;
    for (i = 0; i < keys.size(); i++)
    {
        if (!leaf)
            children[i]->traverse();
        cout << " " << keys[i].isbn;
    }

    if (!leaf)
        children[i]->traverse();
}

BTreeNode *BTreeNode::search(const std::string &isbn)
{
    int i = 0;
    // Buscar la primera clave que es mayor o igual al ISBN
    while (i < keys.size() && isbn > keys[i].isbn)
    {
        i++;
    }

    // Si la clave es igual al ISBN, devolver el nodo actual
    if (i < keys.size() && keys[i].isbn == isbn)
    {
        return this;
    }

    // Si el nodo es una hoja, no hay más nodos para buscar
    if (leaf)
    {
        return nullptr;
    }

    // Buscar en el hijo correspondiente
    return children[i]->search(isbn);
}

void BTree::insert(const Book &k)
{
    // Check if the book with the same ISBN already exists
    if (search(k.isbn) != nullptr)
    {
        std::cerr << "[insert] Duplicate ISBN detected: " << k.isbn << ". Insertion skipped.\n";
        return;
    }

    std::cerr << "[insert] Attempting to insert ISBN: " << k.isbn << "\n";

    if (!root)
    {
        std::cerr << "[insert] Creating root node for the first time\n";
        root = std::make_unique<BTreeNode>(t, true);
        root->keys.push_back(k);
    }
    else
    {
        if (root->keys.size() == 2 * t - 1)
        {
            std::cerr << "[insert] Root node is full, creating a new root and splitting\n";

            // Create a new root node
            auto s = std::make_unique<BTreeNode>(t, false); // New root

            // Move the current root as the first child of the new root
            s->children.push_back(std::move(root)); // Move root ownership to children

            // Split the child (use the raw pointer from the unique_ptr)
            s->splitChild(0, s->children[0]); // Pass the raw pointer using get()

            // Determine which child will receive the new key
            int i = 0;
            if (s->keys[0].isbn < k.isbn)
                i++;

            // Insert the new key into the correct child
            s->children[i]->insertNonFull(k);

            // Update the root
            root = std::move(s); // Move the new root into the root member
        }
        else
        {
            // If the root is not full, insert the key into the root
            root->insertNonFull(k);
        }
    }

    std::cerr << "[insert] Book with ISBN: " << k.isbn << " inserted successfully into the B-tree\n";
}

void BTreeNode::insertNonFull(const Book &k)
{
    std::cerr << "[insertNonFull] Inserting ISBN: " << k.isbn << " into node: " << this << "\n";

    int i = keys.size() - 1;

    if (leaf)
    {
        keys.push_back(Book()); // Añadir espacio
        while (i >= 0 && keys[i].isbn > k.isbn)
        {
            keys[i + 1] = std::move(keys[i]);
            i--;
        }
        keys[i + 1] = k;
        std::cerr << "[insertNonFull] Inserted ISBN: " << k.isbn << " at position " << i + 1 << " in leaf node\n";
    }
    else
    {
        while (i >= 0 && keys[i].isbn > k.isbn)
            i--;

        std::cerr << "[insertNonFull] Recur to child at index " << i + 1 << "\n";
        if (children[i + 1]->keys.size() == 2 * t - 1)
        {
            std::cerr << "[insertNonFull] Child " << children[i + 1].get() << " is full, need to split\n";
            splitChild(i + 1, children[i + 1]);

            if (keys[i + 1].isbn < k.isbn)
                i++;
        }
        children[i + 1]->insertNonFull(k);
    }
}

void BTreeNode::splitChild(int i, std::unique_ptr<BTreeNode> &y)
{
    // Asegurarse de que i y t estén dentro de los límites
    if (i < 0 || i >= children.size())
    {
        return;
    }
    if (t <= 1)
    {
        return;
    }

    // Create a new node z as a unique_ptr
    std::unique_ptr<BTreeNode> z = std::make_unique<BTreeNode>(y->t, y->leaf);

    if (y->keys.size() < 2 * t - 1)
    {
        return;
    }

    Book median = y->keys[t - 1];

    for (int j = 0; j < t - 1; j++)
    {
        if (j + t < y->keys.size())
        {
            z->keys.push_back(y->keys[j + t]);
        }
        else
        {
            return;
        }
    }

    if (!y->leaf)
    {
        for (int j = 0; j < t; j++)
        {
            if (j + t < y->children.size())
            {
                // Use std::move to transfer ownership of the children
                z->children.push_back(std::move(y->children[j + t]));
            }
            else
            {
                return;
            }
        }
    }

    y->keys.resize(t - 1);
    if (!y->leaf)
    {
        y->children.resize(t);
    }

    // Insert z into children at the appropriate index
    if (i + 1 <= children.size())
    {
        children.insert(children.begin() + i + 1, std::move(z));
    }
    else
    {
        return;
    }

    // Insert the median key into the current node's keys
    if (i <= keys.size())
    {
        keys.insert(keys.begin() + i, median);
    }
    else
    {
        return;
    }
}

void BTree::remove(const string &isbn)
{
    if (!root)
        return;

    root->remove(isbn);

    // If the root has no keys left, we need to update the root
    if (root->keys.size() == 0)
    {
        // If the root has children, the new root will be its first child
        if (root->leaf)
        {
            root.reset(); // root is a unique_ptr, reset releases the memory
        }
        else
        {
            // Transfer ownership of the first child to root
            root = std::move(root->children[0]);
        }

        cout << "Root node updated after removal" << endl;
    }
}
void BTreeNode::remove(const std::string &isbn)
{
    int idx = findKey(isbn);

    if (idx < keys.size() && keys[idx].isbn == isbn)
    {
        if (leaf)
        {
            removeFromLeaf(idx);
        }
        else
        {
            removeFromNonLeaf(idx);
        }
    }
    else
    {
        if (leaf)
        {
            std::cerr << "Key not found in tree\n";
            return;
        }

        bool flag = (idx == keys.size());

        if (children[idx]->keys.size() < t)
        {
            fill(idx);
        }

        if (flag && idx > keys.size())
        {
            children[idx - 1]->remove(isbn);
        }
        else
        {
            children[idx]->remove(isbn);
        }
    }
}

int BTreeNode::findKey(const string &isbn)
{
    int idx = 0;
    while (idx < keys.size() && keys[idx].isbn < isbn)
    {
        ++idx;
    }
    cout << "Key found at index: " << idx << " for ISBN: " << isbn << endl; // Debugging
    return idx;
}

void BTreeNode::removeFromLeaf(int idx)
{
    cout << "Removing from leaf node at index: " << idx << endl; // Debugging
    keys.erase(keys.begin() + idx);
}

void BTreeNode::removeFromNonLeaf(int idx)
{
    cout << "Removing from non-leaf node at index: " << idx << " with key: " << keys[idx].isbn << endl;
    Book k = keys[idx];
    cout << "Removing from non-leaf node, ISBN: " << k.isbn << endl; // Debugging

    if (children[idx]->keys.size() >= t)
    {
        Book pred = getPred(idx);
        cout << "Replacing with predecessor: " << pred.isbn << endl;
        keys[idx] = pred;
        children[idx]->remove(pred.isbn);
    }
    else if (children[idx + 1]->keys.size() >= t)
    {
        Book succ = getSucc(idx);
        cout << "Replacing with successor: " << succ.isbn << endl;
        keys[idx] = succ;
        children[idx + 1]->remove(succ.isbn);
    }
    else
    {
        cout << "Merging at index: " << idx << endl;
        merge(idx);
        children[idx]->remove(k.isbn);
    }
}

Book BTreeNode::getPred(int idx)
{
    BTreeNode *cur = children[idx].get();
    while (!cur->leaf)
        cur = cur->children[cur->keys.size()].get();
    return cur->keys[cur->keys.size() - 1];
}

Book BTreeNode::getSucc(int idx)
{
    BTreeNode *cur = children[idx + 1].get();
    while (!cur->leaf)
        cur = cur->children[0].get();
    return cur->keys[0];
}

void BTreeNode::fill(int idx)
{
    if (idx != 0 && children[idx - 1]->keys.size() >= t)
    {
        borrowFromPrev(idx);
    }
    else if (idx != keys.size() && children[idx + 1]->keys.size() >= t)
    {
        borrowFromNext(idx);
    }
    else
    {
        if (idx != keys.size())
        {
            merge(idx);
        }
        else
        {
            merge(idx - 1);
        }
    }
}

void BTreeNode::borrowFromPrev(int idx)
{
    BTreeNode *child = children[idx].get();
    BTreeNode *sibling = children[idx - 1].get();

    child->keys.insert(child->keys.begin(), keys[idx - 1]);

    if (!child->leaf)
    {
        child->children.insert(child->children.begin(), std::move(sibling->children[sibling->keys.size()]));
    }

    keys[idx - 1] = sibling->keys[sibling->keys.size() - 1];

    sibling->keys.pop_back();
    if (!sibling->leaf)
        sibling->children.pop_back();
}

void BTreeNode::borrowFromNext(int idx)
{
    BTreeNode *child = children[idx].get();
    BTreeNode *sibling = children[idx + 1].get();

    child->keys.push_back(keys[idx]);

    if (!child->leaf)
        child->children.push_back(std::move(sibling->children[0]));

    keys[idx] = sibling->keys[0];

    sibling->keys.erase(sibling->keys.begin());
    if (!sibling->leaf)
        sibling->children.erase(sibling->children.begin());
}

void BTreeNode::merge(int idx)
{
    std::unique_ptr<BTreeNode> &child = children[idx];
    std::unique_ptr<BTreeNode> &sibling = children[idx + 1];

    // Move the middle key from the current node into the child node
    child->keys.push_back(std::move(keys[idx]));

    // Move all keys from sibling into child
    for (int i = 0; i < sibling->keys.size(); ++i)
        child->keys.push_back(std::move(sibling->keys[i]));

    // Move all children from sibling into child if not a leaf
    if (!child->leaf)
    {
        for (int i = 0; i < sibling->children.size(); ++i)
            child->children.push_back(std::move(sibling->children[i]));
    }

    // Remove the key from the current node
    keys.erase(keys.begin() + idx);

    // Remove the sibling node from children and let unique_ptr handle deletion
    children.erase(children.begin() + idx + 1);
}

void BTreeNode::printNode()
{
    for (int i = 0; i < keys.size(); i++)
    {
        if (!leaf)
            children[i]->printNode();
        cout << keys[i].toJson() << endl;
    }

    if (!leaf)
        children[keys.size()]->printNode();
}

void BTree::printTree()
{
    if (root != nullptr)
        root->printNode();
}

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
    size_t i;
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
    size_t i = 0;
    while (i < keys.size() && isbn > keys[i].isbn)
    {
        i++;
    }
    if (i < keys.size() && keys[i].isbn == isbn)
    {
        return this;
    }
    if (leaf)
    {
        return nullptr;
    }

    return children[i]->search(isbn);
}

void BTree::insert(const Book &k)
{
    if (search(k.isbn) != nullptr)
    {
        return;
    }

    if (!root)
    {
        root = std::make_unique<BTreeNode>(t, true);
        root->keys.push_back(k);
    }
    else
    {
        if (root->keys.size() == static_cast<size_t>(2 * t - 1))
        {
            auto s = std::make_unique<BTreeNode>(t, false);
            s->children.push_back(std::move(root));
            s->splitChild(0, s->children[0]);
            size_t i = 0;
            if (s->keys[0].isbn < k.isbn)
                i++;
            s->children[i]->insertNonFull(k);

            root = std::move(s);
        }
        else
        {
            root->insertNonFull(k);
        }
    }
}

void BTreeNode::insertNonFull(const Book &k)
{
    int i = static_cast<int>(keys.size()) - 1;

    if (leaf)
    {
        keys.push_back(Book());
        while (i >= 0 && keys[i].isbn > k.isbn)
        {
            keys[i + 1] = std::move(keys[i]);
            i--;
        }
        keys[i + 1] = k;
    }
    else
    {
        while (i >= 0 && keys[i].isbn > k.isbn)
            i--;
        if (children[i + 1]->keys.size() == static_cast<size_t>(2 * t - 1))
        {
            splitChild(i + 1, children[i + 1]);

            if (keys[i + 1].isbn < k.isbn)
                i++;
        }
        children[i + 1]->insertNonFull(k);
    }
}

void BTreeNode::splitChild(int i, std::unique_ptr<BTreeNode> &y)
{
    if (i < 0 || static_cast<size_t>(i) >= children.size())
    {
        return;
    }
    if (t <= 1)
    {
        return;
    }

    std::unique_ptr<BTreeNode> z = std::make_unique<BTreeNode>(y->t, y->leaf);

    if (y->keys.size() < static_cast<size_t>(2 * t - 1))
    {
        return;
    }

    Book median = y->keys[t - 1];

    for (int j = 0; j < t - 1; j++)
    {
        if (static_cast<size_t>(j + t) < y->keys.size())
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
            if (static_cast<size_t>(j + t) < y->children.size())
            {
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

    if (static_cast<size_t>(i + 1) <= children.size())
    {
        children.insert(children.begin() + i + 1, std::move(z));
    }
    else
    {
        return;
    }
    if (static_cast<size_t>(i) <= keys.size())
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
    if (root->keys.empty())
    {
        if (root->leaf)
        {
            root.reset();
        }
        else
        {
            root = std::move(root->children[0]);
        }
    }
}

void BTreeNode::remove(const std::string &isbn)
{
    size_t idx = findKey(isbn);

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
            return;
        }

        bool flag = (idx == keys.size());

        if (children[idx]->keys.size() < static_cast<size_t>(t))
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

size_t BTreeNode::findKey(const string &isbn)
{
    size_t idx = 0;
    while (idx < keys.size() && keys[idx].isbn < isbn)
    {
        ++idx;
    }
    return idx;
}

void BTreeNode::removeFromLeaf(size_t idx)
{
    keys.erase(keys.begin() + idx);
}

void BTreeNode::removeFromNonLeaf(size_t idx)
{
    Book k = keys[idx];

    if (children[idx]->keys.size() >= static_cast<size_t>(t))
    {
        Book pred = getPred(idx);
        keys[idx] = std::move(pred);
        children[idx]->remove(keys[idx].isbn);
    }
    else if (children[idx + 1]->keys.size() >= static_cast<size_t>(t))
    {
        Book succ = getSucc(idx);
        keys[idx] = std::move(succ);
        children[idx + 1]->remove(keys[idx].isbn);
    }
    else
    {
        merge(idx);
        children[idx]->remove(k.isbn);
    }
}

Book BTreeNode::getPred(size_t idx)
{
    BTreeNode *cur = children[idx].get();
    while (!cur->leaf)
        cur = cur->children[cur->keys.size()].get();
    return cur->keys.back();
}

Book BTreeNode::getSucc(size_t idx)
{
    BTreeNode *cur = children[idx + 1].get();
    while (!cur->leaf)
        cur = cur->children[0].get();
    return cur->keys.front();
}

void BTreeNode::fill(size_t idx)
{
    if (idx != 0 && children[idx - 1]->keys.size() >= static_cast<size_t>(t))
    {
        borrowFromPrev(idx);
    }
    else if (idx != keys.size() && children[idx + 1]->keys.size() >= static_cast<size_t>(t))
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

void BTreeNode::borrowFromPrev(size_t idx)
{
    std::unique_ptr<BTreeNode> &child = children[idx];
    std::unique_ptr<BTreeNode> &sibling = children[idx - 1];

    child->keys.insert(child->keys.begin(), keys[idx - 1]);
    if (!child->leaf)
    {
        child->children.insert(child->children.begin(), std::move(sibling->children.back()));
        sibling->children.pop_back();
    }

    keys[idx - 1] = sibling->keys.back();
    sibling->keys.pop_back();
}

void BTreeNode::borrowFromNext(size_t idx)
{
    std::unique_ptr<BTreeNode> &child = children[idx];
    std::unique_ptr<BTreeNode> &sibling = children[idx + 1];

    child->keys.push_back(keys[idx]);
    if (!child->leaf)
    {
        child->children.push_back(std::move(sibling->children.front()));
        sibling->children.erase(sibling->children.begin());
    }

    keys[idx] = sibling->keys.front();
    sibling->keys.erase(sibling->keys.begin());
}

void BTreeNode::merge(size_t idx)
{
    std::unique_ptr<BTreeNode> &child = children[idx];
    std::unique_ptr<BTreeNode> &sibling = children[idx + 1];

    child->keys.push_back(std::move(keys[idx]));

    child->keys.insert(child->keys.end(), std::make_move_iterator(sibling->keys.begin()), std::make_move_iterator(sibling->keys.end()));

    if (!child->leaf)
    {
        child->children.insert(child->children.end(), std::make_move_iterator(sibling->children.begin()), std::make_move_iterator(sibling->children.end()));
    }
    keys.erase(keys.begin() + idx);
    children.erase(children.begin() + idx + 1);
}

void BTreeNode::printNode()
{
    for (size_t i = 0; i < keys.size(); i++)
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

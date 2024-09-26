#pragma once
#include <iostream>
#include <vector>
#include "Book.h"

using namespace std;

class BTreeNode
{
public:
	vector<Book> keys;
	std::vector<std::unique_ptr<BTreeNode>> children;
	int t;
	bool leaf;

	BTreeNode(int _t, bool leaf);

	void traverse();
	BTreeNode *search(const string &isbn);
	void insertNonFull(const Book &book);
	void splitChild(int i, std::unique_ptr<BTreeNode> &y);
	size_t findKey(const string &isbn);
	void remove(const string &isbn);
	void removeFromLeaf(size_t idx);
	void removeFromNonLeaf(size_t idx);
	Book getPred(size_t idx);		
	Book getSucc(size_t idx);		
	void fill(size_t idx);			
	void borrowFromPrev(size_t idx);
	void borrowFromNext(size_t idx);
	void merge(size_t idx);			
	void printNode();

	friend class BTree;

	~BTreeNode() = default;
};

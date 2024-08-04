#pragma once
#include <iostream>
#include <vector>
#include "Book.h"

using namespace std;

class BTreeNode
{
public:
	vector<Book> keys;
	vector<BTreeNode*> children;
	// minimum degree
	int t;
	bool leaf;
	
	BTreeNode(int _t, bool leaf);

	void traverse();
	BTreeNode* search(const string& isbn);
	void insertNonFull(const Book& book);
	void splitChild(int i, BTreeNode* y);
	int findKey(const string& isbn);
	void remove(const string& isbn);
	void removeFromLeaf(int idx);
	void removeFromNonLeaf(int idx);
	Book getPred(int idx);
	Book getSucc(int idx);
	void fill(int idx);
	void borrowFromPrev(int idx);
	void borrowFromNext(int idx);
	void merge(int idx);
	void printNode();

	friend class BTree;

};


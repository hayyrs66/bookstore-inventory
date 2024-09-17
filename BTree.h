#pragma once
#include "BTreeNode.h"
#include "Book.h"

class BTree
{
public:
	std::unique_ptr<BTreeNode> root;
	int t;

	BTree(int _t) : root(nullptr), t(_t) {}

	void traverse() {
		if (root != nullptr) root->traverse();
	}

	BTreeNode* search(const string& isbn) {
		return (root == nullptr) ? nullptr : root->search(isbn);
	}

	void insert(const Book& book);
	void remove(const string& isbn);
	void printTree();
};

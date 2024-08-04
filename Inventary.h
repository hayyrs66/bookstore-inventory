#pragma once
#include "BTree.h"
#include <json/json.h>

class Inventary
{
	BTree tree;
public:
	Inventary(int t) : tree(t) {}

	void insert(const Book& book) {
		tree.insert(book);
	}

	void remove(const string& isbn) {
		tree.remove(isbn);
	}

	void update(const Json::Value& book) {
		BTreeNode* node = tree.search(book["isbn"].asString());
		if (node != nullptr) {
			for (auto& b : node->keys) {
				if (b.isbn == book["isbn"].asString()) {
					if(book.isMember("name")) b.name = book["name"].asString();
					if(book.isMember("author")) b.author = book["author"].asString();
					if(book.isMember("category")) b.category = book["category"].asString();
					if(book.isMember("price")) b.price = book["price"].asString();
					if(book.isMember("quantity")) b.quantity = book["quantity"].asString();
				}
			}
		} else 
			cerr << "Book not found at update" << endl;
	}

	vector<Book> searchByName(const string& name) {
		vector<Book> results;
		searchRecursive(tree.root, name, results);
		return results;
	}

	void searchRecursive(BTreeNode* node, const string& name, vector<Book>& results) {
		if (node == nullptr) return;

		for (const auto& b : node->keys) {
			if (b.name == name)
				results.push_back(b);
		}

		if (!node->leaf) {
			for(auto child : node->children)
				searchRecursive(child, name, results);
		}
	}

	void printInventary() {
		tree.printTree();
	}

};


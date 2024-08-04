#pragma once
#include <iostream>
#include <json/json.h>

using namespace std;

class Book
{
public:
	string isbn;
	string name;
	string author;
	string category;
	string price;
	string quantity;

	Book(const string& _isbn, const string& _name, const string& _author, const string& _price, const string& _quantity)
		: isbn(_isbn), name(_name), author(_author), price(_price), quantity(_quantity) {}

	Book() : price("0.0"), quantity("0") {}

	Json::Value toJson() const {
		Json::Value book;
		book["isbn"] = isbn;
		book["name"] = name;
		book["author"] = author;
		book["category"] = category;
		book["price"] = price;
		book["quantity"] = quantity;
		return book;
	}

	static Book fromJson(const Json::Value& book) {
		return Book(
			book["isbn"].asString(), 
			book["name"].asString(), 
			book["author"].asString(), 
			book["price"].asString(),
			book["quantity"].asString()
		);
	}
};

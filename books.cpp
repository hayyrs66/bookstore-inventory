#include <iostream>
#include "Testing.h"

int main() {
	
	Testing test(3);
	test.uploadBooks("input.csv");
	test.executeSearch("search.csv", "output.txt");
	
	//test.printInventory();

	return 0;
}
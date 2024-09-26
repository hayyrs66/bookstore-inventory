#include <iostream>
#include "Testing.h"

int main() {
	Testing test(20);
	test.uploadBooks("./compress/100K/100Klab01_books.csv");
	test.executeSearch("./compress/100K/100Klab01_search.csv", "output.txt");
	return 0;
}
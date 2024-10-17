#include <iostream>
#include "Testing.h"

int main() {
	Testing test(20);
	test.uploadBooks("./100Klab01_books.csv");
	test.executeSearch("./100Klab01_search.csv", "output.txt");
	return 0;
}
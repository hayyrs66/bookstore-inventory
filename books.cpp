#include <iostream>
#include "Testing.h"

int main() {
	
	Testing test(20);
	test.uploadBooks("./compress/Ejemplo_lab01_books.csv");
	test.executeSearch("./compress/Ejemplo_lab01_search.csv", "output.txt");
	return 0;
}
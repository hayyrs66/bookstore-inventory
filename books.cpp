#include <iostream>
#include "headers/Testing.h"

int main() {
	Testing test(20);
	test.uploadBooks("100Klab01_books.csv");
	test.executeSearch("100Klab01_search.csv", "new_output.txt" ,"new_output.txt.enc", "ok:uo1IN");
	return 0;
}
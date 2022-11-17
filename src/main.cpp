#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

#include "word.h"
#include "crossword.h"
#include "tools.h"

int main() {
    // Can be fancier, but for now, define the list of words here
    std::vector<std::string> words = {"africa", "america", "australia", "antarctica"};

    // Generate all possible reasonable crosswords
    std::vector<Crossword> cw = CreateCrosswords(words);

    // Count valid Crosswords
    // A crossword is valid for 4 words if there are 4 intersections
    int n = words.size() == 4 ? CountCrosswordsThatAreValid(cw) : 0;

    std::cout << "Found " << cw.size() << " reasonable crosswords!" << std::endl;
    std::cout << "Found " << n << " crosswords valid!" << std::endl;
    return 0;
}

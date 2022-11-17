#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

#include "word.h"
#include "crossword.h"
#include "constants.h"

std::vector<Word> Intersect(Word w1, Word w2);

void AddNewWord(Word w, Crossword cw, std::vector<Crossword> &outcome_crosswords);

std::vector<Crossword> CreateCrosswords(std::vector<std::string> words);

int CountCrosswordsThatAreValid(std::vector<Crossword> cw);

#endif

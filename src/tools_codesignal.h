#ifndef TOOLS_CS_H
#define TOOLS_CS_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

#include "word.h"
#include "crossword.h"

/**
 * Returns a vector of words (w1a,w2a,w1b,w2b...) of all the possible
 * intersections between w1 and w2. The new words will have the availability
 * updated, and the correspondin anchor of the first word will be updated if the
 * anchor of the second word is set.
 * w1: Word to intersect
 * w2: Word to which we want to intersect w1
 **/
std::vector<Word> IntersectCS(Word w1, Word w2);

/**
 * Adds a new word w to the crossword cw, and returns in outcome_crosswords
 * all possible crosswords that are valid in which w has been added to cw.
 * This number can get high pretty fast.
 **/
void AddNewWordCS(Word w, Crossword cw, std::vector<Crossword> &outcome_crosswords);

/**
 * Given a vector of word strings, it returns all the valid crosswords
 * that can be generated with those words. Note that rotated crosswords
 * are considered different crosswords.
 **/
std::vector<Crossword> CreateCrosswordsCS(std::vector<std::string> words);

/**
 * Will count and print the 4-worded crosswords with 4 intersections
 * This was the solution to the question asked in codesignal.
 * However, I wanted to make something a bit more general and fun.
 */
int CountCrosswordsThatAreValidCS(std::vector<Crossword> cw);

#endif

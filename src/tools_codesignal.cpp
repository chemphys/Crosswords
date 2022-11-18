#include "tools.h"

std::vector<Word> IntersectCS(Word w1, Word w2) {
    std::vector<Word> updated_words;  // {w1a,w2a,w1b,w2b,...}

    // If the orientation is the same or is not set, the intersection is not possible
    if (w1.orientation_ == w2.orientation_) return updated_words;
    if (w1.orientation_ == 0 || w2.orientation_ == 0) return updated_words;

    // For sake of indexing consistency, define the "big" and the "small" word
    // (aka long and short, just realized it now.)
    Word wbig = w1;
    Word wsmall = w2;
    bool swapped = false;

    if (w2.size() > w1.size()) {
        wbig = w2;
        wsmall = w1;
        swapped = true;
    }

    for (size_t i = 0; i < wbig.size(); i++) {
        for (size_t j = 0; j < wsmall.size(); j++) {
            // If the letters are the same, intersection may be possible
            if (wbig.w_[i] == wsmall.w_[j]) {
                // Both positions in both words must be available
                if ((wbig.availability_[i + 1] == 0) && (wsmall.availability_[j + 1] == 0)) {
                    // The intersection is valid.
                    // Define new words to push to the solution
                    Word w1a = wbig;
                    Word w2b = wsmall;
                    size_t i2 = i;
                    size_t j2 = j;
                    if (swapped) {
                        w1a = wsmall;
                        w2b = wbig;
                        i2 = j;
                        j2 = i;
                    }

                    // Update their availability
                    w1a.availability_[i2] = 1;
                    w1a.availability_[i2 + 1] = 2;
                    w1a.availability_[i2 + 2] = 1;

                    w2b.availability_[j2] = 1;
                    w2b.availability_[j2 + 1] = 2;
                    w2b.availability_[j2 + 2] = 1;

                    // Update anchor for w1 (w1a) if anchor for w2b is not empty.
                    if (w2b.anchor_.size() != 0 and w1a.anchor_.size() == 0) {
                        std::vector<int> anchor = {0, 0};
                        if (w2b.orientation_ == 1) {
                            // w2 is horizontal
                            anchor[0] = w2b.anchor_[0] - i2;
                            anchor[1] = w2b.anchor_[1] + j2;
                        } else {
                            // w2 is vertical
                            anchor[0] = w2b.anchor_[0] + j2;
                            anchor[1] = w2b.anchor_[1] - i2;
                        }
                        w1a.anchor_ = anchor;
                    }

                    updated_words.push_back(w1a);
                    updated_words.push_back(w2b);
                }
            }
        }
    }

    return updated_words;
}

void AddNewWordCS(Word w, Crossword cw, std::vector<Crossword> &outcome_crosswords) {
    // this function cannot be used if the crossword is empty.
    if (cw.size() == 0) {
        return;
    }

    // Loop over the words in the crossword, and try to intersect w with each one of them
    for (size_t i = 0; i < cw.size(); i++) {
        // Need to make a copy of the word object, since it will be modified.
        Word wc = cw.words_[i];

        // Gett all updated words that could be added
        std::vector<Word> new_words = IntersectCS(w, wc);

        // If no words can be added, continue to nexxt word
        if (new_words.size() == 0) continue;

        // Otherwise, we add the word at the crossword and update
        // the crossword word for each possible crossword
        size_t n_extra = new_words.size() / 2;  // It is ensured to be even

        // Create a new crossword with the new addition for each possible intersection
        for (size_t j = 0; j < n_extra; j++) {
            Crossword updated_cw = cw;
            updated_cw.words_[i] = new_words[2 * j + 1];
            bool fine = updated_cw.AddWord(new_words[2 * j]);

            // Check if the word was succesfully added
            if (!fine) continue;

            // If it was, just add it to the outcome crosswords!
            outcome_crosswords.push_back(updated_cw);
        }
    }
}

std::vector<Crossword> CreateCrosswordsCS(std::vector<std::string> words) {
    std::vector<Crossword> output_crosswords;

    Word wh(words[0], 1);

    std::vector<int> wh_anchor = {int(GRID_SIZE / 2), int(GRID_SIZE / 2)};
    wh.anchor_ = wh_anchor;

    Crossword cw;
    cw.AddWord(wh);

    for (size_t i = 1; i < words.size(); i++) {
        Word wv(words[i], 2);
        AddNewWordCS(wv, cw, output_crosswords);
    }

    for (size_t k = 0; k < output_crosswords.size(); k++) {
        if (output_crosswords[k].size() == 4) continue;
        std::vector<std::string> missing_words;
        for (size_t i = 0; i < words.size(); i++) {
            if (output_crosswords[k].Contains(words[i]))
                continue;
            else
                missing_words.push_back(words[i]);
        }

        if (missing_words.size() != 2) continue;

        Word wh1(missing_words[0], 1);
        Word wv1(missing_words[0], 2);
        Word wh2(missing_words[1], 1);
        Word wv2(missing_words[1], 2);

        Word cww1 = output_crosswords[k].words_[0];
        Word cww2 = output_crosswords[k].words_[1];

        std::vector<Word> new_words_cww1 = IntersectCS(wv1, cww1);
        std::vector<Word> new_words_cww2 = IntersectCS(wh2, cww2);

        for (size_t i = 0; i < new_words_cww1.size() / 2; i++) {
            for (size_t j = 0; j < new_words_cww2.size() / 2; j++) {
                Word w1 = new_words_cww1[2 * i];
                Word w2 = new_words_cww2[2 * j];
                std::vector<Word> intersection = IntersectCS(w1, w2);
                for (size_t m = 0; m < intersection.size() / 2; m++) {
                    Crossword cr = output_crosswords[k];
                    cr.words_[0] = new_words_cww1[2 * i + 1];
                    cr.words_[1] = new_words_cww2[2 * j + 1];
                    bool fine = cr.AddWord(intersection[2 * m]);
                    if (!fine) continue;
                    fine = cr.AddWord(intersection[2 * m + 1]);
                    if (!fine) continue;

                    for (size_t n = 0; n < output_crosswords.size(); n++) {
                        if (cr == output_crosswords[n]) {
                            fine = false;
                            break;
                        }
                    }

                    if (fine && std::count(cr.grid_.begin(), cr.grid_.end(), 2) == 4) {
                        output_crosswords.push_back(cr);
                    }
                }
            }
        }

        new_words_cww1 = IntersectCS(wh1, cww2);
        new_words_cww2 = IntersectCS(wv2, cww1);

        for (size_t i = 0; i < new_words_cww1.size() / 2; i++) {
            for (size_t j = 0; j < new_words_cww2.size() / 2; j++) {
                Word w1 = new_words_cww1[2 * i];
                Word w2 = new_words_cww2[2 * j];
                std::vector<Word> intersection = IntersectCS(w1, w2);
                for (size_t m = 0; m < intersection.size() / 2; m++) {
                    Crossword cr = output_crosswords[k];
                    cr.words_[1] = new_words_cww1[2 * i + 1];
                    bool fine = cr.AddWord(intersection[2 * m]);
                    if (!fine) continue;
                    cr.words_[0] = new_words_cww2[2 * j + 1];
                    fine = cr.AddWord(intersection[2 * m + 1]);
                    if (!fine) continue;

                    for (size_t n = 0; n < output_crosswords.size(); n++) {
                        if (cr == output_crosswords[n]) {
                            fine = false;
                            break;
                        }
                    }

                    if (fine && std::count(cr.grid_.begin(), cr.grid_.end(), 2) == 4) {
                        output_crosswords.push_back(cr);
                    }
                }
            }
        }
    }

    for (size_t k = 0; k < output_crosswords.size() - 1; k++) {
        size_t l = k + 1;
        while (l < output_crosswords.size()) {
            if (output_crosswords[k] == output_crosswords[l]) {
                output_crosswords.erase(output_crosswords.begin() + l);
            } else {
                l++;
            }
        }
    }

    return output_crosswords;
}

int CountCrosswordsThatAreValidCS(std::vector<Crossword> cw) {
    int count = 0;

    for (size_t i = 0; i < cw.size(); i++) {
        if (std::count(cw[i].grid_.begin(), cw[i].grid_.end(), 2) == 4) {
            count++;
            cw[i].Print();
        }
    }

    return count;
}

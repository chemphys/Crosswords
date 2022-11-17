#include "tools.h"

std::vector<Word> Intersect(Word w1, Word w2) {
    std::vector<Word> updated_words;  // {w1a,w2a,w1b,w2b,...}

    if (w1.orientation_ == w2.orientation_) return updated_words;
    if (w1.orientation_ == 0 || w2.orientation_ == 0) return updated_words;

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
            if (wbig.w_[i] == wsmall.w_[j]) {
                if ((wbig.availability_[i + 1] == 0) && (wsmall.availability_[j + 1] == 0)) {
                    // The intersection is valid.
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

void AddNewWord(Word w, Crossword cw, std::vector<Crossword> &outcome_crosswords) {
    if (cw.size() == 0) {
        return;
    }

    for (size_t i = 0; i < cw.size(); i++) {
        Word wc = cw.words_[i];

        // Gett all updated words that could be added
        std::vector<Word> new_words = Intersect(w, wc);

        // If no words can be added, continue to nexxt word
        if (new_words.size() == 0) continue;

        // Otherwise, we add the word at the crossword and update
        // the crossword word for each possible crossword
        size_t n_extra = new_words.size() / 2;  // It is ensured to be even

        for (size_t j = 0; j < n_extra; j++) {
            Crossword updated_cw = cw;
            updated_cw.words_[i] = new_words[2 * j + 1];
            bool fine = updated_cw.AddWord(new_words[2 * j]);

            if (!fine) continue;

            outcome_crosswords.push_back(updated_cw);
        }
    }
}

std::vector<Crossword> CreateCrosswords(std::vector<std::string> words) {
    std::vector<Crossword> output_crosswords;

    for (size_t i = 0; i < words.size(); i++) {
        Word wh(words[i], 1);
        Word wv(words[i], 2);

        std::vector<int> wh_anchor = {int(GRID_SIZE / 2), int(GRID_SIZE / 2)};
        wh.anchor_ = wh_anchor;
        wv.anchor_ = wh_anchor;

        Crossword cw, cw2;
        cw.AddWord(wh);
        cw2.AddWord(wv);

        output_crosswords.push_back(cw);
        output_crosswords.push_back(cw2);
    }

    for (size_t i = 0; i < words.size(); i++) {
        // orientation of first word is horizontal.
        Word wh(words[i], 1);
        Word wv(words[i], 2);

        for (size_t k = 0; k < output_crosswords.size(); k++) {
            if (output_crosswords[k].Contains(words[i])) continue;
            AddNewWord(wh, output_crosswords[k], output_crosswords);
            if (output_crosswords[k].Contains(words[i])) continue;
            AddNewWord(wv, output_crosswords[k], output_crosswords);
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
    }

    return output_crosswords;
}

int CountCrosswordsThatAreValid(std::vector<Crossword> cw) {
    int count = 0;

    for (size_t i = 0; i < cw.size(); i++) {
        if (std::count(cw[i].grid_.begin(), cw[i].grid_.end(), 2) == 4) {
            count++;
            cw[i].Print();
        }
    }

    return count;
}

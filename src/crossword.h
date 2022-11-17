#ifndef CROSSWORD_H
#define CROSSWORD_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

// Contains the words and their position information in the smallest possible grid
class Crossword {
   public:
    // Default constructor. No extra work needed
    Crossword() {}

    // Default destructor. No extra work needed
    ~Crossword() {}

    // Quick access to the number of words in the crossword
    size_t size() { return words_.size(); }

    // Given a new grid size, relocates the existing words in the new grid
    // to accomodate for a new word.
    // new_size_rows: new number of rows
    // new_size_cols: new number of columns
    // old_anchors: anchor of the words in the crossword before rearranging
    void Reanchor(int new_size_rows, int new_size_cols, std::vector<int> old_anchors) {
        // Define new grids with new sizes
        std::vector<int> new_grid(new_size_rows * new_size_cols, 0);
        std::vector<std::string> new_grids(new_size_rows * new_size_cols, "");

        // Find the minimum row and colum. That will be set to 0
        // and the other values will be scaled accordingly
        int min_row = word_anchors_[0];
        int min_col = word_anchors_[1];
        for (size_t i = 1; i < word_anchors_.size() / 2; i++) {
            if (word_anchors_[2 * i] < min_row) min_row = word_anchors_[2 * i];
            if (word_anchors_[2 * i + 1] < min_col) min_col = word_anchors_[2 * i + 1];
        }

        // Set new anchors
        for (size_t i = 0; i < word_anchors_.size() / 2; i++) {
            word_anchors_[2 * i] -= min_row;
            word_anchors_[2 * i + 1] -= min_col;
        }

        // Put each existing word in the new grid in angreement with the new anchors
        // This is the reason that the old anchors are needed, to access the right place
        // in the old grid.
        for (size_t i = 0; i < words_.size(); i++) {
            for (size_t j = 0; j < words_[i].size(); j++) {
                if (words_[i].orientation_ == 1) {
                    new_grid[(word_anchors_[2 * i] * new_size_cols) + j + word_anchors_[2 * i + 1]] =
                        grid_[old_anchors[2 * i] * grid_cols_ + j + old_anchors[2 * i + 1]];
                    new_grids[(word_anchors_[2 * i] * new_size_cols) + j + word_anchors_[2 * i + 1]] = words_[i].w_[j];
                } else {
                    new_grid[(j + word_anchors_[2 * i]) * new_size_cols + word_anchors_[2 * i + 1]] =
                        grid_[(j + old_anchors[2 * i]) * grid_cols_ + old_anchors[2 * i + 1]];
                    new_grids[(j + word_anchors_[2 * i]) * new_size_cols + word_anchors_[2 * i + 1]] = words_[i].w_[j];
                }
            }
        }

        // Update Corssword class dimentions and grids
        grid_ = new_grid;
        grids_ = new_grids;
        grid_rows_ = new_size_rows;
        grid_cols_ = new_size_cols;
    }

    // Returns true if the result is a valid crossword
    bool AddWord(Word w) {
        // If word is in crossword, return false
        for (size_t i = 0; i < words_.size(); i++) {
            if (w.w_ == words_[i].w_) return false;
        }

        // If, including the new word, the balance between vertical and horizontal
        // words is greater than 1, then that addition is not valid.
        int diff = 0;
        for (size_t i = 0; i < words_.size(); i++) {
            if (words_[i].orientation_ == 1) {
                diff += 1;
            } else {
                diff -= 1;
            }
        }
        if (w.orientation_ == 1) {
            diff += 1;
        } else {
            diff -= 1;
        }

        if (std::abs(diff) > 1) return false;

        size_t w_size = w.size();
        if (words_.size() == 0) {
            // In the case of an empty crossword, jsut add the word.
            words_.push_back(w);
            if (w.orientation_ == 1) {
                grid_ = std::vector<int>(w_size, 1);
                grids_ = std::vector<std::string>(w_size, "");
                for (size_t i = 0; i < w_size; i++) grids_[i] = w.w_[i];
                grid_rows_ = 1;
                grid_cols_ = w_size;
                word_high_anchors_.push_back(0);
                word_high_anchors_.push_back(w.size());
            } else {
                grid_ = std::vector<int>(w_size, 0);
                grids_ = std::vector<std::string>(w_size, "");
                for (size_t i = 0; i < w_size; i++) {
                    grid_[i] = 1;
                    grids_[i] = w.w_[i];
                }
                grid_rows_ = w_size;
                grid_cols_ = 1;
                word_high_anchors_.push_back(w.size());
                word_high_anchors_.push_back(0);
            }
            // Add anchor of the first word, which is 0,0
            word_anchors_.push_back(0);
            word_anchors_.push_back(0);
            return true;
        } else {  // not empty
            // Get new anchors for the new word.
            // Note that they can be negative. It will be fixed a few lines down.
            int new_anchor_row = w.anchor_[0] - words_[0].anchor_[0] + word_anchors_[0];
            int new_anchor_col = w.anchor_[1] - words_[0].anchor_[1] + word_anchors_[1];
            word_anchors_.push_back(new_anchor_row);
            word_anchors_.push_back(new_anchor_col);

            // Add the high anchors
            if (w.orientation_ == 1) {
                word_high_anchors_.push_back(new_anchor_row);
                word_high_anchors_.push_back(new_anchor_col + w.size());
            } else {
                word_high_anchors_.push_back(new_anchor_row + w.size());
                word_high_anchors_.push_back(new_anchor_col);
            }

            // Store a copy of the old anchors, needed by Reanchor
            std::vector<int> old_anchors = word_anchors_;

            // TODO
            // This part may be duplicate work from Reanchor, will investigate if I have time.
            int min_row = word_anchors_[0];
            int min_col = word_anchors_[1];
            for (size_t i = 1; i < word_anchors_.size() / 2; i++) {
                if (word_anchors_[2 * i] < min_row) min_row = word_anchors_[2 * i];
                if (word_anchors_[2 * i + 1] < min_col) min_col = word_anchors_[2 * i + 1];
            }

            // Set new anchors
            for (size_t i = 0; i < word_anchors_.size() / 2; i++) {
                word_anchors_[2 * i] -= min_row;
                word_anchors_[2 * i + 1] -= min_col;
                word_high_anchors_[2 * i] -= min_row;
                word_high_anchors_[2 * i + 1] -= min_col;
            }

            min_row = 0;
            min_col = 0;

            // Find the new high_anchors
            int max_row = word_high_anchors_[0];
            int max_col = word_high_anchors_[1];
            for (size_t i = 1; i < word_anchors_.size() / 2; i++) {
                if (word_high_anchors_[2 * i] > max_row) max_row = word_high_anchors_[2 * i];
                if (word_high_anchors_[2 * i + 1] > max_col) max_col = word_high_anchors_[2 * i + 1];
            }

            // Find out new minimum grid size
            int new_grid_size_rows = max_row - min_row;
            int new_grid_size_cols = max_col - min_col;

            // Quick check to remove some undesired crosswords
            // We don't want snake crosswords:
            /*
                     p a p
                         e
                         p o p
                             i
                             p
            But rather:
                     p a p
                     i   e
                     p o p

            */

            // This next check ensures that the crosswords are not gonna be snake-like
            int sum_rows = 0;
            int sum_cols = 0;
            for (size_t i = 0; i < words_.size(); i++) {
                if (words_[i].orientation_ == 1) sum_cols += words_[i].size();
                if (words_[i].orientation_ == 2) sum_rows += words_[i].size();
            }

            if (words_.size() > 1) {
                if (w.orientation_ == 1) sum_cols += w.size();
                if (w.orientation_ == 2) sum_rows += w.size();
                if (new_grid_size_rows == sum_rows - 2) return false;
                if (new_grid_size_cols == sum_cols - 2) return false;
                if (new_grid_size_rows == sum_rows - 1) return false;
                if (new_grid_size_cols == sum_cols - 1) return false;
            }

            // Add word and check validity of crossword
            // If the new word intersects another word in the Crossword,
            // but the letter in the intersection is not the same in both words,
            // the addition will be invalid.
            // Also, only 2 words can intersect in the same place.
            new_anchor_row = word_anchors_[word_anchors_.size() - 2];
            new_anchor_col = word_anchors_[word_anchors_.size() - 1];

            //  Reanchor words
            Reanchor(new_grid_size_rows, new_grid_size_cols, old_anchors);

            if (w.orientation_ == 1) {
                for (size_t i = 0; i < w.size(); i++) {
                    grid_[new_anchor_row * new_grid_size_cols + i + new_anchor_col] += 1;
                    if (grid_[new_anchor_row * new_grid_size_cols + i + new_anchor_col] > 2) return false;
                    grids_[new_anchor_row * new_grid_size_cols + i + new_anchor_col] += w.w_[i];
                    if (std::count(grids_[new_anchor_row * new_grid_size_cols + i + new_anchor_col].begin(),
                                   grids_[new_anchor_row * new_grid_size_cols + i + new_anchor_col].end(), w.w_[i]) !=
                        grids_[new_anchor_row * new_grid_size_cols + i + new_anchor_col].size()) {
                        return false;
                    } else {
                        grids_[new_anchor_row * new_grid_size_cols + i + new_anchor_col] = w.w_[i];
                    }
                }
            } else {
                for (size_t i = 0; i < w.size(); i++) {
                    grid_[(i + new_anchor_row) * new_grid_size_cols + new_anchor_col] += 1;
                    if (grid_[(i + new_anchor_row) * new_grid_size_cols + new_anchor_col] > 2) return false;
                    grids_[(i + new_anchor_row) * new_grid_size_cols + new_anchor_col] += w.w_[i];
                    if (std::count(grids_[(i + new_anchor_row) * new_grid_size_cols + new_anchor_col].begin(),
                                   grids_[(i + new_anchor_row) * new_grid_size_cols + new_anchor_col].end(), w.w_[i]) !=
                        grids_[(i + new_anchor_row) * new_grid_size_cols + new_anchor_col].size()) {
                        return false;
                    } else {
                        grids_[(i + new_anchor_row) * new_grid_size_cols + new_anchor_col] = w.w_[i];
                    }
                }
            }
        }

        // Everything looks fine, push the word
        words_.push_back(w);
        return true;
    }

    // Overloaded == operator.
    // Two crosswords are the same if their string grids are the same.
    bool operator==(const Crossword cw) {
        bool out = true;
        if (grid_.size() != cw.grid_.size()) return false;
        for (size_t i = 0; i < grids_.size(); i++) {
            if (grids_[i] != cw.grids_[i]) return false;
        }

        return true;
    }

    // Print the crossword on the screen
    void Print() {
        std::cout << std::endl;
        std::cout << grid_rows_ << " x " << grid_cols_ << std::endl;
        std::cout << std::endl;
        for (int row = 0; row < grid_rows_; row++) {
            for (int col = 0; col < grid_cols_; col++) {
                std::cout << std::setw(3) << grids_[row * grid_cols_ + col];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }

    // Checks if a string is already in the crossword
    bool Contains(std::string w) {
        for (size_t i = 0; i < words_.size(); i++) {
            if (words_[i].w_ == w) return true;
        }
        return false;
    }

    // Words in the crossword
    std::vector<Word> words_;

    // Position of the first letter of the word in the grid.
    // The size is 2*N, where N is the number of words.
    // The first element of every 2-element group is the row anchor,
    // and the second one is the column anchor
    std::vector<int> word_anchors_;

    // Similar to word_anchors, but for the last letter of the word
    std::vector<int> word_high_anchors_;

    // Numeric crossowrd grid. The number in each element is the number of
    // words occupying that cell. 0 is empty, 1 is single words, 2 is an intersection.
    std::vector<int> grid_;

    // Actual crossword string in a grid.
    std::vector<std::string> grids_;

    // Number of rows in the crossword
    int grid_rows_;

    // Number of columns in the crossword
    int grid_cols_;
};

#endif

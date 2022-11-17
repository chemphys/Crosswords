#ifndef CROSSWORD_H
#define CROSSWORD_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

class Crossword {
   public:
    Crossword() {}
    ~Crossword() {}

    size_t size() { return words_.size(); }

    void Reanchor(int new_size_rows, int new_size_cols, std::vector<int> old_anchors) {
        std::vector<int> new_grid(new_size_rows * new_size_cols, 0);
        std::vector<std::string> new_grids(new_size_rows * new_size_cols, "");

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

        // Empty Crossword
        size_t w_size = w.size();
        if (words_.size() == 0) {
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
            // Get new anchors
            int new_anchor_row = w.anchor_[0] - words_[0].anchor_[0] + word_anchors_[0];
            int new_anchor_col = w.anchor_[1] - words_[0].anchor_[1] + word_anchors_[1];
            word_anchors_.push_back(new_anchor_row);
            word_anchors_.push_back(new_anchor_col);

            if (w.orientation_ == 1) {
                word_high_anchors_.push_back(new_anchor_row);
                word_high_anchors_.push_back(new_anchor_col + w.size());
            } else {
                word_high_anchors_.push_back(new_anchor_row + w.size());
                word_high_anchors_.push_back(new_anchor_col);
            }

            std::vector<int> old_anchors = word_anchors_;

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

            int max_row = word_high_anchors_[0];
            int max_col = word_high_anchors_[1];
            for (size_t i = 1; i < word_anchors_.size() / 2; i++) {
                if (word_high_anchors_[2 * i] > max_row) max_row = word_high_anchors_[2 * i];
                if (word_high_anchors_[2 * i + 1] > max_col) max_col = word_high_anchors_[2 * i + 1];
            }

            // Find out new grid size
            int new_grid_size_rows = max_row - min_row;
            int new_grid_size_cols = max_col - min_col;

            // Quick check to remove some undesired crossowrds
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

            //// Add word and check validity of crossword
            new_anchor_row = word_anchors_[word_anchors_.size() - 2];
            new_anchor_col = word_anchors_[word_anchors_.size() - 1];

            //// Then check if needs expansion on the right/bottom
            // if (w.orientation_ == 2) {
            //     if (new_grid_size_rows < w.size() + new_anchor_row) new_grid_size_rows = w.size() + new_anchor_row;
            // } else {
            //     if (new_grid_size_cols < w.size() + new_anchor_col) new_grid_size_cols = w.size() + new_anchor_col;
            // }
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
        words_.push_back(w);
        return true;
    }

    bool operator==(const Crossword cw) {
        bool out = true;
        if (grid_.size() != cw.grid_.size()) return false;
        for (size_t i = 0; i < grids_.size(); i++) {
            if (grids_[i] != cw.grids_[i]) return false;
        }

        return true;
    }

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

    bool Contains(std::string w) {
        for (size_t i = 0; i < words_.size(); i++) {
            if (words_[i].w_ == w) return true;
        }
        return false;
    }

    std::vector<Word> words_;
    // Pairs. first is row anchor, second is column anchor
    std::vector<int> word_anchors_;
    std::vector<int> word_high_anchors_;
    std::vector<int> grid_;
    std::vector<std::string> grids_;

    int grid_rows_;
    int grid_cols_;
};

#endif

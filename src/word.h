#ifndef WORD_H
#define WORD_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

// Word class stores a word and useful information of that word
class Word {
   public:
    // Default constructor
    Word() {
        w_ = "";
        orientation_ = 0;
        availability_.clear();
        anchor_.clear();
    }

    // Overloaded constructor.
    // w: string with the word
    // orientation: either ifthe word is vertical (top to bottom)
    //              or horizontal (left to right)
    Word(std::string w, int orientation) {
        w_ = w;
        orientation_ = orientation;
        availability_ = std::vector<int>(w.size() + 2, 0);
    }

    // Default destructor. No extra work needed
    ~Word() {}

    // Overloaded == operator. Two words are equal if the orientation, availability,
    // and actual word are the same. Anchor does not matter.
    bool operator==(const Word w) {
        bool out = true;
        out = out && orientation_ == w.orientation_;
        out = out && availability_ == w.availability_;
        out = out && w_ == w.w_;
        return out;
    }

    // Useful tool to print the information on the standard output.
    void Print() {
        std::cout << w_ << " ; orientation = " << orientation_ << std::endl;
        for (size_t i = 0; i < availability_.size(); i++) {
            std::cout << availability_[i] << " ";
        }
        std::cout << std::endl;
    }

    // Quick way to access the number of letters of the word
    int size() { return w_.size(); }

    // The actual word
    std::string w_;

    // Horizontal (1) or vertical(2)
    int orientation_;

    // Same number of elements as w + 2. Extra space at beggining and end
    // to deal with indexes.
    // 0 is available, 1 is next to intersection, 2 is intersection with another word
    std::vector<int> availability_;

    // The ancor in space (assumed a 2d grid)
    std::vector<int> anchor_;
};

#endif

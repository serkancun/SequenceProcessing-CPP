#include "BPE.h"
#include <sstream>
#include <algorithm>

BPE::BPE() {}

void BPE::addRule(const std::string& left, const std::string& right) {
    merge_rules.push_back({left, right});
}

std::vector<std::string> BPE::tokenize(const std::string& word) {
    std::vector<std::string> tokens;

    for (size_t i = 0; i < word.length(); ) {
        int char_len = 1;
        unsigned char c = word[i];

        if ((c & 0xF8) == 0xF0) char_len = 4;
        else if ((c & 0xF0) == 0xE0) char_len = 3;
        else if ((c & 0xE0) == 0xC0) char_len = 2;

        tokens.push_back(word.substr(i, char_len));
        i += char_len;
    }

    bool is_merged;
    do {
        is_merged = false;

        for (const auto& rule : merge_rules) {
            std::string left = rule.first;
            std::string right = rule.second;

            for (size_t i = 0; i < tokens.size() - 1; ++i) {
                if (tokens[i] == left && tokens[i+1] == right) {
                    tokens[i] = left + right;
                    tokens.erase(tokens.begin() + i + 1);
                    is_merged = true;
                    break;
                }
            }
            if (is_merged) break;
        }
    } while (is_merged);

    return tokens;
}
void BPE::train(const std::string& corpus, int iteration_count) {

    std::map<std::string, int> word_freqs;
    std::stringstream ss(corpus);
    std::string word;
    while (ss >> word) {

        std::string split_word = "";
        for (size_t i = 0; i < word.length(); ) {
            int len = 1;
            unsigned char c = word[i];
            if ((c & 0xE0) == 0xC0) len = 2;
            split_word += word.substr(i, len) + " ";
            i += len;
        }
        word_freqs[split_word]++;
    }

    for (int i = 0; i < iteration_count; ++i) {
        std::map<std::pair<std::string, std::string>, int> pair_freqs;

        for (auto const& [w_split, freq] : word_freqs) {
            std::stringstream w_ss(w_split);
            std::vector<std::string> symbols;
            std::string sym;
            while (w_ss >> sym) symbols.push_back(sym);

            for (size_t j = 0; j < symbols.size() - 1; ++j) {
                pair_freqs[{symbols[j], symbols[j+1]}] += freq;
            }
        }

        if (pair_freqs.empty()) break;

        auto best_pair = std::max_element(pair_freqs.begin(), pair_freqs.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });

        addRule(best_pair->first.first, best_pair->first.second);

        std::map<std::string, int> new_word_freqs;
        std::string target = best_pair->first.first + " " + best_pair->first.second;
        std::string replacement = best_pair->first.first + best_pair->first.second;

        for (auto const& [w_split, freq] : word_freqs) {
            std::string updated_w = w_split;
            size_t pos = 0;
            while ((pos = updated_w.find(target, pos)) != std::string::npos) {
                updated_w.replace(pos, target.length(), replacement);
                pos += replacement.length();
            }
            new_word_freqs[updated_w] = freq;
        }
        word_freqs = new_word_freqs;
    }
}
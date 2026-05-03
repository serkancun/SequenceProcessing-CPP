#ifndef BPE_H
#define BPE_H

#include <string>
#include <vector>
#include <map>
#include <utility>

class BPE {
private:
    std::vector<std::pair<std::string, std::string>> merge_rules;

    std::map<std::string, int> get_word_frequencies(const std::string& corpus);
    std::string merge_pair_in_word(const std::string& word, const std::pair<std::string, std::string>& pair);

public:
    BPE();
    void addRule(const std::string& left, const std::string& right);
    std::vector<std::string> tokenize(const std::string& word);

    void train(const std::string& corpus, int iteration_count);
};

#endif
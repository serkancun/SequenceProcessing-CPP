#ifndef BPETOKENIZER_H
#define BPETOKENIZER_H

#include "Tokenizer.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>

class BPETokenizer : public Tokenizer {
private:
    std::unordered_map<std::string, int> vocabFrequencies;
    std::vector<std::pair<std::string, std::string>> mergeRules;

    std::vector<std::string> splitIntoChars(const std::string& word);
    std::vector<std::string> splitBySpace(const std::string& str);
    std::string joinTokens(const std::vector<std::string>& tokens);

public:
    void train(const std::string& corpusPath, int iterations) override;
    std::vector<std::string> tokenize(const std::string& text) override;
};

#endif
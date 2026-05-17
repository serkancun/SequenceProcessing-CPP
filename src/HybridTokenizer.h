#ifndef HYBRIDTOKENIZER_H
#define HYBRIDTOKENIZER_H

#include "Tokenizer.h"
#include <unordered_map>
#include <string>
#include <vector>

class HybridTokenizer : public Tokenizer {
private:
    Tokenizer* fallbackBPE;
    std::unordered_map<std::string, std::vector<std::string>> morphDictionary;
    std::vector<std::string> splitBySpace(const std::string& str);
    bool isCapitalized(const std::string& word);
    std::string toLowercase(const std::string& word);

public:
    HybridTokenizer(Tokenizer* bpe);

    void train(const std::string& corpusPath, int iterations) override;
    std::vector<std::string> tokenize(const std::string& text) override;
};

#endif
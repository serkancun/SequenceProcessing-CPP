#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

class Tokenizer {
public:
    virtual ~Tokenizer() = default;
    virtual void train(const std::string& corpusPath, int iterations) = 0;
    virtual std::vector<std::string> tokenize(const std::string& text) = 0;
};

#endif
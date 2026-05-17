#include "HybridTokenizer.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <fstream>

// store the fallback BPE tokenizer
HybridTokenizer::HybridTokenizer(Tokenizer* bpe) {
    this->fallbackBPE = bpe;
}

// split a string into a vector of words by space
std::vector<std::string> HybridTokenizer::splitBySpace(const std::string& str) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// check if the first letter of the word is uppercase
bool HybridTokenizer::isCapitalized(const std::string& word) {
    if (word.empty()) return false;
    return std::isupper(static_cast<unsigned char>(word[0]));
}

// convert only the first character of the word to lowercase
std::string HybridTokenizer::toLowercase(const std::string& word) {
    std::string lowerWord = word;
    if (!lowerWord.empty()) {
        lowerWord[0] = std::tolower(static_cast<unsigned char>(lowerWord[0]));
    }
    return lowerWord;
}

// remove spaces and newline characters from lines
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \r\n\t");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \r\n\t");
    return str.substr(first, (last - first + 1));
}

// load morphological data from file to build the internal dictionary
void HybridTokenizer::train(const std::string& corpusPath, int iterations) {
    std::cout << "[Hybrid] Metamorpheme dictionary loading...\n";
    std::ifstream file("metamorpheme-atis.txt");

    if (!file.is_open()) {
        std::cerr << "Error: 'metamorpheme-atis.txt' file not found!\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        // skip empty lines and sentence tags
        if (line == "<S>" || line == "</S>" || line.empty()) continue;

        std::stringstream ss(line);
        std::string surfaceWord, morphBreakdown;

        // parse word and its morph tags
        if (ss >> surfaceWord >> morphBreakdown) {
            std::string lowerSurface = toLowercase(surfaceWord);
            std::vector<std::string> parts;
            std::stringstream breakdownStream(morphBreakdown);
            std::string part;

            // split the morph breakdown by +
            while (std::getline(breakdownStream, part, '+')) {
                if (!part.empty()) {
                    parts.push_back(part);
                }
            }

            // map the lowercase word to its split morphemes
            morphDictionary[lowerSurface] = parts;
        }
    }
    file.close();
    std::cout << " -> " << morphDictionary.size() << " word loaded\n";
}

// tokenize text using the dictionary or falling back to BPE
std::vector<std::string> HybridTokenizer::tokenize(const std::string& text) {
    std::vector<std::string> finalTokens;
    std::vector<std::string> words = splitBySpace(text);

    for (std::string w : words) {
        // Step 1: separate punctuation at the end of a word
        std::string punctuation = "";
        if (!w.empty() && ispunct(w.back()) && w.back() != '\'') {
            punctuation = std::string(1, w.back());
            w.pop_back();
        }

        // Step 2: add uppercase tag and lowercase the word if capitalized
        if (isCapitalized(w)) {
            finalTokens.push_back("<uppercase>");
            w = toLowercase(w);
        }

        // Step 3: dictionary check
        if (morphDictionary.find(w) != morphDictionary.end()) {
            // add the exact morpheme pieces from our dictionary
            for (const std::string& morphToken : morphDictionary[w]) {
                finalTokens.push_back(morphToken);
            }
        }
        // Step 4: fallback BPE
        else {
            // use the BPE tokenizer if the word is missing from the dictionary
            std::vector<std::string> bpeTokens = fallbackBPE->tokenize(w);
            for (const std::string& t : bpeTokens) {
                finalTokens.push_back(t);
            }
        }

        // Step 5: append the punctuation back in the end
        if (!punctuation.empty()) {
            finalTokens.push_back(punctuation);
        }
    }
    return finalTokens;
}
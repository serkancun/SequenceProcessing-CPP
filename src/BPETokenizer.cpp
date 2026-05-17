#include "BPETokenizer.h"
#include <iostream>
#include <fstream>
#include <sstream>

// split string by space
std::vector<std::string> BPETokenizer::splitBySpace(const std::string& str) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// safe character splitter for Turkish characters
std::vector<std::string> BPETokenizer::splitIntoChars(const std::string& word) {
    std::vector<std::string> chars;
    size_t i = 0;
    while (i < word.length()) {
        int len = 1;
        unsigned char c = word[i];

        if (c >= 0xC0) {
            if (c >= 0xFC) len = 6;
            else if (c >= 0xF8) len = 5;
            else if (c >= 0xF0) len = 4;
            else if (c >= 0xE0) len = 3;
            else if (c >= 0xC0) len = 2;
        }

        chars.push_back(word.substr(i, len));
        i += len;
    }
    return chars;
}

// reconstruct tokens with spaces
std::string BPETokenizer::joinTokens(const std::vector<std::string>& tokens) {
    std::string result = "";
    for (size_t i = 0; i < tokens.size(); ++i) {
        result += tokens[i];
        if (i < tokens.size() - 1) result += " ";
    }
    return result;
}

// BPE training method to learn merge rules
void BPETokenizer::train(const std::string& corpusPath, int iterations) {
    std::cout << "[BPE] Training starts... File: " << corpusPath << "\n";
    std::ifstream file(corpusPath);
    if (!file.is_open()) {
        std::cerr << "Error: File not opened -> " << corpusPath << "\n";
        return;
    }

    // Step 1: split words into characters and count their starting frequencies
    std::string word;
    while (file >> word) {
        std::vector<std::string> chars = splitIntoChars(word);
        std::string spacedWord = joinTokens(chars);
        vocabFrequencies[spacedWord]++;
    }
    file.close();

    // Step 2: main BPE training iterations to extract merge rules
    for (int i = 0; i < iterations; ++i) {
        std::unordered_map<std::string, int> pairs;

        // count how often two tokens appear next to each other.
        for (const auto& pair : vocabFrequencies) {
            std::vector<std::string> tokens = splitBySpace(pair.first);
            int freq = pair.second;

            if (tokens.size() < 2) continue;

            for (size_t j = 0; j < tokens.size() - 1; ++j) {
                std::string bigram = tokens[j] + " " + tokens[j + 1];
                pairs[bigram] += freq;
            }
        }

        if (pairs.empty()) break;

        // select the most frequent bigram to merge
        std::string bestPair = "";
        int maxFreq = -1;
        for (const auto& p : pairs) {
            if (p.second > maxFreq) {
                maxFreq = p.second;
                bestPair = p.first;
            }
        }

        // store the learned merge rule
        std::vector<std::string> bestTokens = splitBySpace(bestPair);
        if (bestTokens.size() == 2) {
            mergeRules.push_back({bestTokens[0], bestTokens[1]});
        }

        // apply the new merge rule globally to update the active vocabulary frequencies
        std::unordered_map<std::string, int> newVocab;
        std::string mergedToken = bestTokens[0] + bestTokens[1];

        for (const auto& pair : vocabFrequencies) {
            std::vector<std::string> tokens = splitBySpace(pair.first);
            std::vector<std::string> newTokens;

            for (size_t j = 0; j < tokens.size(); ++j) {
                if (j < tokens.size() - 1 && tokens[j] == bestTokens[0] && tokens[j + 1] == bestTokens[1]) {
                    newTokens.push_back(mergedToken);
                    j++;
                } else {
                    newTokens.push_back(tokens[j]);
                }
            }
            newVocab[joinTokens(newTokens)] = pair.second;
        }
        vocabFrequencies = newVocab;
    }

    std::cout << "[BPE] Training completed. Constructed rule count: " << mergeRules.size() << "\n";
}

// tokenize input text by applying learned merge rules sequentially
std::vector<std::string> BPETokenizer::tokenize(const std::string& text) {
    std::vector<std::string> resultTokens;
    std::vector<std::string> words = splitBySpace(text);

    for (const std::string& w : words) {
        // start segmentation from individual characters
        std::vector<std::string> tokens = splitIntoChars(w);

        // apply each learned merge rule one by one in order.
        for (const auto& rule : mergeRules) {
            std::vector<std::string> newTokens;
            for (size_t i = 0; i < tokens.size(); ++i) {
                if (i < tokens.size() - 1 && tokens[i] == rule.first && tokens[i + 1] == rule.second) {
                    newTokens.push_back(rule.first + rule.second);
                    i++;
                } else {
                    newTokens.push_back(tokens[i]);
                }
            }
            tokens = newTokens;
        }

        // collect final segments
        for (const std::string& t : tokens) {
            resultTokens.push_back(t);
        }
    }

    return resultTokens;
}
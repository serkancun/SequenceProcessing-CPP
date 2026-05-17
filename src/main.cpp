#include <iostream>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif
#include "BPETokenizer.h"
#include "HybridTokenizer.h"

void printTokens(const std::vector<std::string>& tokens) {
    for (const std::string& t : tokens) {
        std::cout << "[" << t << "] ";
    }
    std::cout << "\n";
}

int main() {
    // correct rendering of Turkish characters on Windows
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::string corpusPath = "atis-tr.txt";
    std::string testSentence = "Ucuz uçuşları istiyorum.";

    // --- 1. BPE ALGORITHM ---
    std::cout << "--- 1. BPE ALGORITHM TEST ---\n";
    Tokenizer* bpe = new BPETokenizer();
    bpe->train(corpusPath, 2000); // train BPE with a merge operation budget of 2000
    std::vector<std::string> bpeResult = bpe->tokenize(testSentence);
    std::cout << "[BPE Result]: ";
    printTokens(bpeResult);

    // --- 2. HYBRID ALGORITHM ---
    std::cout << "\n--- 2. HYBRID ALGORITHM TEST ---\n";
    Tokenizer* hybrid = new HybridTokenizer(bpe); //fallback mechanism with bpe algorithm
    hybrid->train(corpusPath, 0);
    std::vector<std::string> hybridResult = hybrid->tokenize(testSentence);
    std::cout << "[Hybrid Result]: ";
    printTokens(hybridResult);

    delete hybrid;
    delete bpe;

    return 0;
}
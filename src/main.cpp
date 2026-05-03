#include <iostream>
#include <fstream>
#include <windows.h>
#include "BPE.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    BPE bpe;

    std::ifstream file("atis-tr.txt");
    std::string corpus((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::cout << "Training starts..." << std::endl;
    bpe.train(corpus, 250);
    std::cout << "Training completed.\n" << std::endl;

    std::string test = "";
    std::vector<std::string> tokens = bpe.tokenize(test);

    std::cout << test << " -> ";
    for (const auto& t : tokens) std::cout << "[" << t << "] ";
    std::cout << std::endl;

    return 0;
}
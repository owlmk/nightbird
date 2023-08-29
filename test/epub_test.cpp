#include "epub.hpp"
#include <codecvt>
#include <string>
#include <locale>
#include <iostream>
#include <Windows.h>

using namespace std;

std::string stringToUnicodeEscape(const std::string &input) {
    std::string result;
    for (char c : input) {
        // Convert each character to its Unicode escape sequence
        result += "\\u" + std::to_string(static_cast<unsigned int>(c));
    }
    return result;
}

int main() {
    std::string s = u8"../temp";
    std::string t = u8"\u7B2C\u4E8C\u7AE0 \u5370\u5EA6\u6559";
    std::string path = "t.epub";
    epub::EpubParser *ep = new epub::EpubParser(s, path);

    std::cout<< ep->GetChapterPath(2) << std::endl;
    std::cout<< ep->GetChapterPath(t) << std::endl;
}
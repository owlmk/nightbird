#include "epub.hpp"

namespace epub{


EpubParser::EpubParser(std::string temp_dir, std::string file_name) {
    ep = new Zip(file_name);
}

EpubParser::~EpubParser() {
    delete ep;
}

}

#ifndef EPUB_HPP
#define EPUB_HPP

#include <iostream>
#include <vector>
#include "libzippp.h"

using StrList = std::vector<std::string>;
using Zip = libzippp::ZipArchive;
using Entry = libzippp::ZipEntry;


namespace epub {

const std::string container = "container.xml";
const std::string container2 = "container.opf";
const std::string container3 = "package.opf";
const std::string container4 = "volume.opf";

class EpubParser {
    public:
        explicit EpubParser(std::string temp_dir, std::string file_name);
        ~EpubParser();
        StrList toc;
        StrList file_list;
        std::string basedir;

    private:
        Zip *ep;
};


}

#endif
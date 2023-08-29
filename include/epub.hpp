#ifndef EPUB_HPP
#define EPUB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "libzippp.h"

using StrList = std::vector<std::string>;
using Zip = libzippp::ZipArchive;
using Entry = libzippp::ZipEntry;
namespace fs = std::filesystem;

namespace epub {

const std::string content = "content.xml";
const std::string content2 = "content.opf";
const std::string content3 = "package.opf";
const std::string content4 = "volume.opf";

class EpubParser {
    public:
        explicit EpubParser(std::string temp_dir, std::string file_name);
        ~EpubParser();
        std::string GetChapterPath(int index);
        std::string GetChapterPath(std::string toc_name);

        fs::path basedir;
        std::string name;
        std::string uuid;
        std::string cover_path;
        xmlDocPtr doc;

    private:
        Zip *ep;
        StrList toc;
        StrList file_list;

        bool Decompress(std::string file_name, Entry &entry);
        bool ReadContent();
        bool ReadToc();
        
};

}

#endif
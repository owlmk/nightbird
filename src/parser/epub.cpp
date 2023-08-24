#include "epub.hpp"

#include <fstream>
#include <libxml/xpath.h>

namespace epub{


EpubParser::EpubParser(std::string temp_dir, std::string file_name) {
    basedir.assign(temp_dir);
    ep = new Zip(file_name);
    Entry content = ep->getEntry(container);
    if (content.isNull()) {
        content = ep->getEntry(container2);
        if (content.isNull()) {
            content = ep->getEntry(container3);
            if (content.isNull()) {
                content = ep->getEntry(container4);
            }
        }
    }
    if (Decompress("content.xml", content)) {
        ReadContent();
    }

}

EpubParser::~EpubParser() {
    ep->close();
    delete ep;
}

bool EpubParser::Decompress(std::string file_name, Entry &entry) {
    if (ep == nullptr) {
        return false;
    }
    fs::path filepath = basedir.append(file_name);
    std::ofstream out(filepath);
    entry.readContent(out);
    return true;
}

bool EpubParser::ReadContent() {
    doc = xmlReadFile(basedir.append("content.xml").string().c_str(), NULL, 0);
    xmlXPathContextPtr context;
    context = xmlXPathNewContext(doc);
    xmlChar *title_path = BAD_CAST "/*/*/*[local-name()='title']";
    xmlChar *uuid_path = BAD_CAST "/package/metadata[1]/identifier[@id='uuid_id']";
    xmlXPathObjectPtr nodeptr;
    nodeptr = xmlXPathEvalExpression(title_path, context);
    if (nodeptr == nullptr && nodeptr->nodesetval == nullptr && nodeptr->nodesetval->nodeNr <= 0) {
        std::cout << "error name" << std::endl;
        name = "Error";
    } else {
        name = xmlNodeGetContent(nodeptr->nodesetval->nodeTab[0]);
    }
    xmlXPathFreeContext(context);
}

}

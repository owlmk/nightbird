#include "epub.hpp"

#include <algorithm>
#include <fstream>
#include <libxml/xpath.h>

namespace epub{

void GetTocLabelAndFile(StrList &toc, StrList &files, xmlNodePtr &root) {
    xmlNodePtr node;
    for (node = root->children; node != nullptr; node = node->next ) {
        if (node->type == XML_ELEMENT_NODE) {
            if (xmlStrEqual(node->name, BAD_CAST "navPoint")){
                GetTocLabelAndFile(toc, files, node);
                
            } else if (xmlStrEqual(node->name, BAD_CAST "navLabel")) {
                xmlNodePtr subnode;
                for (subnode = node->children; subnode != nullptr; subnode = subnode->next) {
                    if (subnode->type == XML_ELEMENT_NODE && xmlStrEqual(subnode->name, BAD_CAST "text")) {
                        xmlChar *text = xmlNodeGetContent(subnode);
                        toc.emplace_back((char *) text);
                        xmlFree(text);
                    }
                }
                xmlFreeNode(subnode);
            } else if (xmlStrEqual(node->name, BAD_CAST "content")) {
                xmlChar * src_path = xmlGetProp(node, BAD_CAST "src");
                // std::cout << src_path << std::endl;
                files.emplace_back((char *) src_path);
                xmlFree(src_path);
            }
        }
    }
    xmlFreeNode(node);
}

EpubParser::EpubParser(std::string temp_dir, std::string file_name) {
    basedir.assign(temp_dir);
    ep = new Zip(file_name);
    ep->open(libzippp::ZipArchive::ReadOnly);
    Entry contentfile = ep->getEntry(content);
    
    if (contentfile.isNull()) {
        std::cout << content << "not find" << std::endl;
        contentfile = ep->getEntry(content2);
        if (contentfile.isNull()) {
            std::cout << content2 << "not find" << std::endl;
            contentfile = ep->getEntry(content3);
            if (contentfile.isNull()) {
                std::cout << content3 << "not find" << std::endl;
                contentfile = ep->getEntry(content4);
            }
        }
    }
    std::cout << "decompress start " << file_name << std::endl;
    std::cout << contentfile.isNull() << std::endl;

    if (Decompress("content.xml", contentfile)) {
        // std::cout << "decompress done" << std::endl;
        ReadContent();
    }
    ReadToc();

}

EpubParser::~EpubParser() {
    ep->close();
    xmlFreeDoc(doc);
    xmlCleanupParser();
    delete ep;
}

bool EpubParser::Decompress(std::string file_name, Entry &entry) {
    if (ep == nullptr) {
        return false;
    }
    std::cout << "decompress start " << file_name << std::endl;
    fs::path filepath = basedir;
    filepath.append(file_name);
    std::string path = filepath.string();
    std::ofstream out(path);
    int res = entry.readContent(out);
    out.close();
    return true;
}

bool EpubParser::ReadContent() {
    fs::path filepath = basedir;
    filepath.append("content.xml");
    doc = xmlReadFile(filepath.string().c_str(), NULL, 0);
    xmlXPathContextPtr context;
    context = xmlXPathNewContext(doc);
    xmlChar *title_xpath = BAD_CAST "/*/*/*[local-name()='title']";
    xmlChar *uuid_xpath = BAD_CAST "/*/*/*[local-name()='identifier']";
    xmlChar *cover_xpath = BAD_CAST "/*[local-name()='package']/*[local-name()='manifest']/*[local-name()='item' and @id='cover']";
    xmlXPathObjectPtr nodeptr = xmlXPathEvalExpression(title_xpath, context);
    if (nodeptr == nullptr && nodeptr->nodesetval == nullptr && nodeptr->nodesetval->nodeNr <= 0) {
        std::cout << "error name" << std::endl;
        name = "Error";
    } else {
        name = (char *)xmlNodeGetContent(nodeptr->nodesetval->nodeTab[0]);
        // std::cout << "name " << name << std::endl;
    }
    nodeptr = xmlXPathEvalExpression(uuid_xpath, context);
    if (nodeptr == nullptr && nodeptr->nodesetval == nullptr && nodeptr->nodesetval->nodeNr <= 0) {
        std::cout << "error uuid" << std::endl;
        uuid = "Error";
    } else {
        uuid = (char *)xmlNodeGetContent(nodeptr->nodesetval->nodeTab[0]);
        // std::cout << "uuid " << uuid << std::endl;
    }
    // xmlFree(title_xpath);
    // xmlFree(uuid_xpath);
    // xmlFree(cover_xpath);
    xmlXPathFreeObject(nodeptr);
    xmlXPathFreeContext(context);
    // std::cout << "read content done" << std::endl;
}

bool EpubParser::ReadToc() {
    Entry toc = ep->getEntry("toc.ncx");
    std::cout << "decompress start " << toc.isNull() << std::endl;
    if (toc.isNull() || !Decompress("toc.ncx", toc)) {
        
        return false;
    }
    std::cout << "decompress done toc.ncx" << toc.isNull() << std::endl;
    fs::path filepath = basedir;
    filepath.append("toc.ncx");
    xmlDocPtr toc_doc = xmlReadFile(filepath.string().c_str(), "UTF-8", XML_PARSE_RECOVER);
    if (toc_doc == NULL) {
        std::cout << "Failed to load XML document\n";
    }
    xmlNodePtr root, node;
    
    root = xmlDocGetRootElement(toc_doc);
    for (node = root->children; node != nullptr; node = node->next ) {
        if ((node->type = XML_ELEMENT_NODE) && xmlStrEqual(node->name, BAD_CAST "navMap")){
            
            // std::cout << node->name << " " << node->type << std::endl;
            GetTocLabelAndFile(this->toc, file_list, node);
            // cout << BAD_CAST xmlNodeGetContent(node) << endl;
        }
    }
    // xmlFreeNode(node);
    // std::cout << "free done" << std::endl;
    // xmlFreeNode(root);
    // std::cout << "free done" << std::endl;
    // xmlFreeDoc(toc_doc);
    // std::cout << "free done" << std::endl;
    return true;
}

std::string EpubParser::GetChapterPath(int index) {
    std::cout << file_list.size() << std::endl;
    return file_list[index];
}

std::string EpubParser::GetChapterPath(std::string toc_name) {
    auto idx = std::find(toc.begin(), toc.end(), toc_name);
    if (idx == toc.end()) {
        return "error";
    } else {
        return file_list[idx - toc.begin()];
    }
}

}

#include "debug_line.h"
#include "antlr4-runtime.h"
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <string>
#include "nljson.h"
#include "nameof.hpp"
#include "wstrutil.h"
#include <regex>

namespace javacommons {
namespace antlr4 {

class TreeUtils
{

    ::antlr4::Parser *parser;
    std::string Indents;
    int level;

public:
    TreeUtils(::antlr4::Parser *parser): parser(parser) {}
    TreeUtils(::antlr4::Parser &parser): parser(&parser) {}

    std::string toPrettyTree(::antlr4::tree::ParseTree *t)
    {
        level = 0;
        //Indents = "  ";
        Indents = " ";
        std::string result = process(t);
        std::regex e1("\\n\\s*\\n");
        result = std::regex_replace(result, e1, "\n");
        return result;
    }

#if 0x0
    std::vector<std::string> getTextList(::antlr4::tree::ParseTree *t) {
        std::vector<std::string> result;
        if (t->children.size() == 0) {
            result.push_back(t->getText());
            return result;
        }
        for (size_t i = 0; i < t->children.size(); i++) {
            std::vector<std::string> result2 = getTextList(t->children[i]);
            for(auto s: result2)
                result.push_back(s);
        }
        return result;
    }
#endif

    nljson toJson(::antlr4::tree::ParseTree *t, bool optimize, const std::vector<std::string> &keep_list = {}, int level = 0)
    {
        ::antlr4::tree::ParseTreeType type = t->getTreeType();
        //std::cout << NAMEOF_ENUM(type) << std::endl;
        switch(type)
        {
        case ::antlr4::tree::ParseTreeType::RULE:
        {
            //debug_line1(t->getText());
            nljson result = nljson::object();
            std::string s = ::antlr4::tree::Trees::getNodeText(t, parser->getRuleNames());
            result["!"] = s;
            std::cerr << lead2(level) + s << std::endl;
            nljson children = nljson::array();
            for(int i=0; i<t->children.size(); i++)
            {
                nljson child = this->toJson(t->children[i], optimize, keep_list, level+1);
                if (child.is_object() && child["*"].size() == 0) continue;
                children.push_back(child);
            }
            if (children.size() > 0)
            {
                if (optimize && children.size() == 1 && children[0].is_object())
                {
                    if (std::find(keep_list.begin(), keep_list.end(), s) == keep_list.end())
                    //if  (*find(keep_list.begin(), keep_list.end(), s) != s)
                        return children[0];
                }
                else if (children.size() == 1 && children[0].is_string())
                {
                    result["*"] = children[0];
                }
                else
                {
                    result["*"] = children;
                }
            }
            return result;
        }
            break;
        case ::antlr4::tree::ParseTreeType::TERMINAL:
        {
            std::string s = ::antlr4::tree::Trees::getNodeText(t, parser->getRuleNames());
            std::string s_print = s;
            strutil::replace_all(s_print, "\n", "\\n");
            std::cerr << lead2(level) + "|" + s_print  + "|" << std::endl;
            return s;
        }
            break;
        default:
            break;
        }

        return nullptr;
    }

protected:

    std::string process(::antlr4::tree::ParseTree *t)
    {
        if (t->children.size() == 0)
            return ::antlr4::tree::Trees::getNodeText(t, this->parser);
        std::stringstream ss;
        ss << lead(level);
        level++;
        std::string s = ::antlr4::tree::Trees::getNodeText(t, this->parser);
        ss << s << ' ';
        for (size_t i = 0; i < t->children.size(); i++)
        {
            ss << process(t->children[i]);
        }
        level--;
        ss << lead(level);
        return ss.str();
    }

    std::string lead(int level)
    {
        std::stringstream ss;
        if (level > 0)
        {
            ss << '\n';
            for (int cnt = 0; cnt < level; cnt++)
            {
                ss << Indents;
            }
        }
        return ss.str();
    }

    std::string lead2(int level)
    {
        std::stringstream ss;
        if (level > 0)
        {
            for (int cnt = 0; cnt < level; cnt++)
            {
                ss << Indents;
            }
        }
        return ss.str();
    }

};

}
}

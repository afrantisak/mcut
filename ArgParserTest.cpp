#include "ArgParser.h"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[])
{
    try
    {
        // declare and set defaults
        ArgParser::Type::B    b    = 0;
        ArgParser::Type::C    c    = 0;
        ArgParser::Type::UC   uc   = 0;
        ArgParser::Type::S    s    = 0;
        ArgParser::Type::US   us   = 0;
        ArgParser::Type::N    n    = 0;
        ArgParser::Type::UN   un   = 0;
        ArgParser::Type::L    l    = 0;
        ArgParser::Type::UL   ul   = 0;
        ArgParser::Type::LL   ll   = 0;
        ArgParser::Type::ULL  ull  = 0;
        ArgParser::Type::Size size = 0;
        ArgParser::Type::Str  str  = "";

        // define arguments
        ArgParser args("ArgParserTest");
        args.add("--b",      b,    "bool");
        args.add("--c",      c,    "char");
        args.add("--uc",     uc,   "unsigned char");
        args.add("--s",      s,    "short");
        args.add("--us",     us,   "unsigned short");
        args.add("--n",      n,    "int");
        args.add("--un",     un,   "unsigned int");
        args.add("--l",      l,    "long");
        args.add("--ul",     ul,   "unsigned long");
        args.add("--ll",     ll,   "long long");
        args.add("--ull",    ull,  "unsigned long long");
        args.add("--size",   size, "size_t");
        args.add("--str",    str,  "std::string");

        // parse
        args.parse(argc, argv);

        // output
        std::cout << "b:      " << b       << std::endl;
        std::cout << "c:      " << (int)c  << std::endl;
        std::cout << "uc:     " << (int)uc << std::endl;
        std::cout << "s:      " << s       << std::endl;
        std::cout << "us:     " << us      << std::endl;
        std::cout << "n:      " << n       << std::endl;
        std::cout << "un:     " << un      << std::endl;
        std::cout << "l:      " << l       << std::endl;
        std::cout << "ul:     " << ul      << std::endl;
        std::cout << "ll:     " << ll      << std::endl;
        std::cout << "ull:    " << ull     << std::endl;
        std::cout << "size:   " << size    << std::endl;
        std::cout << "str:    " << str     << std::endl;
    }
    catch (int n)
    {
        return n;
    }
    catch (std::exception& e)
    {
        if (e.what())
            std::cerr << "exception: " << e.what() << "\n";
        return 127;
    }

    return 0;
}

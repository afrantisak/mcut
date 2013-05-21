#include "ConsoleInterrupt.h"
#include "ArgParser.h"
#include "Packet.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
#include "BigSink.h"
#include <boost/iostreams/stream.hpp>

struct Options
{
    bool b;
    char c;
    unsigned char uc;
    short s;
    unsigned short us;
    int n;
    unsigned int un;
    long l;
    unsigned long ul;
    long long ll;
    unsigned long long ull;
    std::string string;
    size_t size;

    Options(int argc, char* argv[])
    {
        ArgParser args("ArgParserTest");
        args.add("--b",      b);
        args.add("--c",      c);
        args.add("--uc",     uc);
        args.add("--s",      s);
        args.add("--us",     us);
        args.add("--n",      n);
        args.add("--un",     un);
        args.add("--l",      l);
        args.add("--ul",     ul);
        args.add("--ll",     ll);
        args.add("--ull",    ull);
        args.add("--string", string);
        args.add("--size_t", size);
        args.parse(argc, argv);
    }
};

int main(int argc, char* argv[])
{
    try
    {
        // get command line / config file options
        auto options = Options(argc, argv);
        
        std::cout << "b: "      << options.b      << std::endl;
        std::cout << "c: "      << options.c      << std::endl;
        std::cout << "uc: "     << options.uc     << std::endl;
        std::cout << "s: "      << options.s      << std::endl;
        std::cout << "us: "     << options.us     << std::endl;
        std::cout << "n: "      << options.n      << std::endl;
        std::cout << "un: "     << options.un     << std::endl;
        std::cout << "l: "      << options.l      << std::endl;
        std::cout << "ul: "     << options.ul     << std::endl;
        std::cout << "ll: "     << options.ll     << std::endl;
        std::cout << "ull: "    << options.ull    << std::endl;
        std::cout << "string: " << options.string << std::endl;
        std::cout << "size_t: " << options.size   << std::endl;
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

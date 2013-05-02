#include <algorithm>                       // copy, min
#include <iosfwd>                          // streamsize
#include <cassert>
#include <string>
#include <memory>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/categories.hpp>  // sink_tag

class BigSink : public boost::iostreams::sink {
public:
    typedef std::string Container;
    
    BigSink(Container& container, std::string sFileName, size_t nChunkSize)
    :   container_(container),
        m_pFileName(new std::string(sFileName)),
        m_nChunkSize(nChunkSize)
    {
    }
    
    ~BigSink()
    {
    }
    
    std::streamsize write(const char_type* s, std::streamsize n)
    {
        container_.insert(container_.end(), s, s + n);
        return n;
    }
    
private:
    Container& container_;
    std::shared_ptr<std::string> m_pFileName;
    size_t m_nChunkSize;
    
    
    //BigSink(const BigSink&);
};


int main()
{
    using namespace std;

    string result;
    boost::iostreams::stream<BigSink> out(result, "adsf", 1);
    out << "Hello World!";
    out.flush();
    assert(result == "Hello World!");
}
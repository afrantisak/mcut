#pragma once
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

class ArgParser
{
public:  
    typedef std::string Name;
    
    ArgParser(Name sDescription);
    
    void add_option(Name sLong, Name sDescription);

    // actually process the arguments and check for errors
    void parse_args(int argc, char* argv[]);
    
    // get the value of an argument
    template <typename T>
    void get(const Name& sName, T& t) const
    {
        t = boost::lexical_cast<T>(value(sName).as<std::string>());
    }
        
private:

    const boost::program_options::variable_value& value(const Name& sName) const;
    
    // if this is an optional argument (i.e. the name begins with "-" or "--")
    // then return the name WITHOUT the dashes.  If it is NOT an optional argument,
    // return the empty string.
    Name getOptional(const Name& sLong);
        
    struct Option
    {
        Name sLong;
        Name sShort;
        Name sDescription;
    };
    
    Name m_sDescription;
    
    typedef std::vector<Option> Options;
    Options m_options;
    
    boost::program_options::options_description m_po_optional;
    boost::program_options::positional_options_description m_po_positional;
    boost::program_options::variables_map m_po_map;
};


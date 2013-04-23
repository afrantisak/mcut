#pragma once
#include <string>
#include <boost/program_options.hpp>

class ArgParser
{
public:  
    typedef std::string Name;
    
    ArgParser(Name sDescription);
    
    void add_option(Name sLong, Name sDescription);
    
    void parse_args(int argc, char* argv[]);
    
    int count(const Name& sName) const;

    const boost::program_options::variable_value& value(const Name& sName) const;
        
private:
        
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


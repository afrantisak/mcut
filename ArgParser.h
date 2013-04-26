#pragma once
#include <string>
#include <typeinfo>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

class ArgParser
{
public:  
    typedef std::string Name;
    
    ArgParser(Name sDescription);
    
    template<typename T>
    void add(Name sLong, T& value, Name sDescription)
    {
        m_options.push_back(Option(sLong, "", &value, &typeid(T), sDescription));
    }

    // actually process the arguments and check for errors
    // TODO: load from a config file first then override options from the command line
    void parse(int argc, char* argv[]);
    
    struct Option
    {
        Option(Name sLong, Name sShort, void* pValue, const std::type_info* pInfo, Name sDescription)
        :   m_sLong(sLong), m_sShort(sShort), m_pValue(pValue), m_pInfo(pInfo), m_sDescription(sDescription)
        {
        }
        
        Name m_sLong;
        Name m_sShort;
        void* m_pValue;
        const std::type_info* m_pInfo;
        Name m_sDescription;
    };
    
private:

    const boost::program_options::variable_value& value(const Name& sName) const;
    
    // if this is an optional argument (i.e. the name begins with "-" or "--")
    // then return the name WITHOUT the dashes.  If it is NOT an optional argument,
    // return the empty string.
    Name getOptional(const Name& sLong);
        
    void add_option(boost::program_options::options_description& desc, const Option& option, const Name& sName);
    
    Name m_sDescription;
    
    typedef std::vector<Option> Options;
    Options m_options;
    
    boost::program_options::options_description m_po_visible;
    boost::program_options::options_description m_po_all;
    boost::program_options::positional_options_description m_po_positional;
    boost::program_options::variables_map m_po_map;
};


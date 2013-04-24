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
    void add_option(Name sLong, Name sDescription, T& value)
    {
        Option option;
        option.sLong = sLong;
        option.sShort = "";
        option.sDescription = sDescription;
        option.pValue = &value;
        option.pInfo = &typeid(T);
        m_options.push_back(option);
    }

    // actually process the arguments and check for errors
    void parse_args(int argc, char* argv[]);
    
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
        void* pValue;
        const std::type_info* pInfo;
        
        template<typename T>
        bool assign(const boost::program_options::variable_value& value)
        {
            if (*pInfo == typeid(T))
            {
                *reinterpret_cast<T*>(pValue) = boost::lexical_cast<T>(value.as<std::string>());
                return true;
            }
            return false;
        }
        
    };
    
    Name m_sDescription;
    
    typedef std::vector<Option> Options;
    Options m_options;
    
    boost::program_options::options_description m_po_visible;
    boost::program_options::options_description m_po_all;
    boost::program_options::positional_options_description m_po_positional;
    boost::program_options::variables_map m_po_map;
};


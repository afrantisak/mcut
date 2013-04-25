#include "ArgParser.h"

namespace po = boost::program_options;

template<typename T>
bool option_add(const ArgParser::Option& option, po::options_description& desc, const std::string& sName)
{
    if (*option.m_pInfo == typeid(T))
    {
        desc.add_options()(sName.c_str(), po::value<T>(), option.m_sDescription.c_str());    
        return true;
    }
    return false;
}

template<>
bool option_add<bool>(const ArgParser::Option& option, po::options_description& desc, const std::string& sName)
{
    if (*option.m_pInfo == typeid(bool))
    {
        desc.add_options()(sName.c_str(), option.m_sDescription.c_str());    
        return true;
    }
    return false;
}

template<typename T>
bool option_assign(const ArgParser::Option& option, const po::variable_value& value)
{
    if (*option.m_pInfo == typeid(T))
    {
        if (!value.empty())
            *reinterpret_cast<T*>(option.m_pValue) = boost::lexical_cast<T>(value.as<std::string>());
        return true;
    }
    return false;
}

ArgParser::ArgParser(Name sDescription)
:   m_sDescription(sDescription),
    m_options(),
    m_po_visible("Allowed options"),
    m_po_all("All options"),
    m_po_positional(),
    m_po_map()
{
}

void ArgParser::add_option(boost::program_options::options_description& desc, const Option& option, const Name& sName)
{
    if (option_add<bool>(option, desc, sName)) {}
    else if (option_add<std::string>(option, desc, sName)) {}
    else if (option_add<int>(option, desc, sName)) {}
    else if (option_add<short>(option, desc, sName)) {}
    else
    {
        std::cout << "ERROR: ArgParser unsupported type (" << option.m_pInfo->name() << ") conversion for option " << option.m_sLong << std::endl;
        throw 1;
    }
}

void ArgParser::parse_args(int argc, char* argv[])
{
    m_po_all.add_options()("help", "show this help message");
    m_po_visible.add_options()("help", "show this help message");
    
    // Declare the supported options.
    for (auto option: m_options)
    {
        Name sName = getOptional(option.m_sLong);
        if (sName.size())
        {
            add_option(m_po_all, option, sName);
            add_option(m_po_visible, option, sName);
        }
        else
        {
            m_po_all.add_options()(option.m_sLong.c_str(), option.m_sDescription.c_str());    
            m_po_positional.add(option.m_sLong.c_str(), 1);
        }
    }
    
    po::store(po::command_line_parser(argc, argv).options(m_po_all).positional(m_po_positional).run(), m_po_map);
    po::notify(m_po_map);    
    
    if (m_po_map.count("help")) {
        std::cout << "Usage: " << m_sDescription; 
        for (auto option: m_options)
        {
            Name sName = getOptional(option.m_sLong);
            if (sName.size())
            {
            }
            else
            {
                std::cout << " <" << option.m_sLong << ">";
            }
        }
        
        std::cout << " [options]" << std::endl;
        std::cout << m_po_visible << std::endl;
        throw 0;
    }
    
    // check for required args
    for (auto option: m_options)
    {
        Name sName = getOptional(option.m_sLong);
        if (sName.size())
        {
        }
        else
        {
            if (m_po_map.count(option.m_sLong.c_str()) == 0)
            {
                std::cout << "ERROR: " << option.m_sLong << " is required" << std::endl;
                throw 1;
            }
        }
    }

    // do the conversions
    for (auto option: m_options)
    {
        if (*option.m_pInfo == typeid(bool))
        {
            *reinterpret_cast<bool*>(option.m_pValue) = m_po_map.count(getOptional(option.m_sLong).c_str()) ? true : false;
        }
        else if (option_assign<std::string>(option, value(option.m_sLong))) {}
        else if (option_assign<int>(option, value(option.m_sLong))) {}
        else if (option_assign<short>(option, value(option.m_sLong))) {}
        else
        {
            std::cout << "ERROR: ArgParser unsupported type (" << option.m_pInfo->name() << ") conversion for option " << option.m_sLong << std::endl;
            throw 1;
        }
    }
}

const po::variable_value& ArgParser::value(const Name& sName) const 
{
    return m_po_map.operator[](sName.c_str());
}
    
ArgParser::Name ArgParser::getOptional(const Name& sLong)
{
    // TODO: convert spaces/underscores to dashes
    if (sLong.size() && sLong[0] == '-')
    {
        Name sNice(sLong.substr(1));
        if (sNice.size() && sNice[0] == '-')
            sNice = sNice.substr(1);
        return sNice;
    }
    
    return Name();
}

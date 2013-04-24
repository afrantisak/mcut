#include "ArgParser.h"

namespace po = boost::program_options;

ArgParser::ArgParser(Name sDescription)
:   m_sDescription(sDescription),
    m_options(),
    m_po_visible("Allowed options"),
    m_po_all("All options"),
    m_po_positional(),
    m_po_map()
{
}

void ArgParser::parse_args(int argc, char* argv[])
{
    m_po_visible.add_options()("help", "show this help message");
    m_po_all.add_options()("help", "show this help message");
    
    // Declare the supported options.
    for (auto option: m_options)
    {
        Name sName = getOptional(option.sLong);
        if (sName.size())
        {
            m_po_all.add_options()(sName.c_str(), option.sDescription.c_str());    
            m_po_visible.add_options()(sName.c_str(), option.sDescription.c_str());    
        }
        else
        {
            m_po_all.add_options()(option.sLong.c_str(), option.sDescription.c_str());    
            m_po_positional.add(option.sLong.c_str(), 1);
        }
    }
    
    po::store(po::command_line_parser(argc, argv).options(m_po_all).positional(m_po_positional).run(), m_po_map);
    po::notify(m_po_map);    
    
    if (m_po_map.count("help")) {
        std::cout << "Usage: " << m_sDescription; 
        for (auto option: m_options)
        {
            Name sName = getOptional(option.sLong);
            if (sName.size())
            {
            }
            else
            {
                std::cout << " <" << option.sLong << ">";
            }
        }
        
        std::cout << " [options]" << std::endl;
        std::cout << m_po_visible << std::endl;
        throw 0;
    }
    
    // check for required args
    for (auto option: m_options)
    {
        Name sName = getOptional(option.sLong);
        if (sName.size())
        {
        }
        else
        {
            if (m_po_map.count(option.sLong.c_str()) == 0)
            {
                std::cout << "ERROR: " << option.sLong << " is required" << std::endl;
                throw 1;
            }
        }
    }

    // do the conversions
    for (auto option: m_options)
    {
        if (*option.pInfo == typeid(bool))
        {
            *reinterpret_cast<bool*>(option.pValue) = m_po_map.count(getOptional(option.sLong).c_str()) ? true : false;
        }
        else if (option.assign<std::string>(value(option.sLong))) {}
        else if (option.assign<int>(value(option.sLong))) {}
        else if (option.assign<short>(value(option.sLong))) {}
        else
        {
            std::cout << "ERROR: ArgParser unsupported type (" << option.pInfo->name() << ") conversion for option " << option.sLong << std::endl;
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

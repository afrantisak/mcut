#include "ArgParser.h"

namespace po = boost::program_options;

ArgParser::ArgParser(Name sDescription)
:   m_sDescription(sDescription),
    m_options(),
    m_po_optional("Allowed options"),
    m_po_positional(),
    m_po_map()
{
}

void ArgParser::add_option(Name sLong, Name sDescription)
{
    Option option = { sLong, "", sDescription };
    m_options.push_back(option);
};

void ArgParser::parse_args(int argc, char* argv[])
{
    m_po_optional.add_options()("help", "show this help message");
    // Declare the supported options.
    for (auto option: m_options)
    {
        Name sName = getOptional(option.sLong);
        if (sName.size())
        {
            // TODO: remove any leading dashes and convert spaces/underscores to dashes
            m_po_optional.add_options()(sName.c_str(), option.sDescription.c_str());    
        }
        else
        {
            m_po_optional.add_options()(option.sLong.c_str(), option.sDescription.c_str());    
            m_po_positional.add(option.sLong.c_str(), 1);
        }
    }
    
    po::store(po::command_line_parser(argc, argv).options(m_po_optional).positional(m_po_positional).run(), m_po_map);
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
        std::cout << m_po_optional << std::endl;
        throw 1;
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
}

int ArgParser::count(const Name& sName) const
{
    return m_po_map.count(sName.c_str());
}

const po::variable_value& ArgParser::value(const Name& sName) const 
{
    return m_po_map.operator[](sName.c_str());
}
    
ArgParser::Name ArgParser::getOptional(const Name& sLong)
{
    if (sLong.size() && sLong[0] == '-')
    {
        Name sNice(sLong.substr(1));
        if (sNice.size() && sNice[0] == '-')
            sNice = sNice.substr(1);
        return sNice;
    }
    
    return Name();
}

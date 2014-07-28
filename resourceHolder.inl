#include "ResourcePath.hpp"
#include <fstream>

template <typename Resource, typename ID>
void cResourceHolder<Resource, ID>::loadAllFromList(const std::string& filename, const std::string& extension)
{
    std::vector<std::string>    files;
    std::string                 stmp;
    std::ifstream               inFile(resourcePath() + filename + ".lst");
    
    while ( inFile >> stmp )
    {
        files.push_back(stmp);
    }
    
    inFile.close();
    
    for ( const auto& i : files )
    {
        load(static_cast<ID>(i), i + extension);
    }
    
}

template <typename Resource, typename ID>
void cResourceHolder<Resource, ID>::load(ID id, const std::string& filename)
{
    std::unique_ptr<Resource> p { new Resource() };
    if ( !p->loadFromFile(resourcePath() + filename) )
        throw std::runtime_error("Couldn't load resource: " + filename);
    
    mMap[id] = std::move(p);
}

template <typename Resource, typename ID>
const Resource& cResourceHolder<Resource, ID>::get(ID id) const
{
    auto found = mMap.find(id);
    if ( found == std::end(mMap) )
        throw std::runtime_error("Requested resource not found.");
    return *found->second;
}

template <typename Resource, typename ID>
Resource& cResourceHolder<Resource, ID>::get(ID id)
{
    auto found = mMap.find(id);
    if ( found == std::end(mMap) )
        throw std::runtime_error("Requested resource not found.");
    return *found->second;
}
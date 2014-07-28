#ifndef __sstc__resourceholder__
#define __sstc__resourceholder__

#include <iostream>
#include <memory>
#include <map>

template <typename Resource, typename ID>
class cResourceHolder {
public:
    void                load(ID, const std::string&);   // load resource from file
    Resource&           get(ID);
    const Resource&     get(ID) const;
    
private:
    std::map<ID, std::unique_ptr<Resource>> mMap;
};

#include "resourceHolder.inl"

#endif /* defined(__sstc__resourceholder__) */

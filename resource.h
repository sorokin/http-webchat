#ifndef HTTPWEBCHAT_RESOURCE_H
#define HTTPWEBCHAT_RESOURCE_H


#include <map>

class Resource {
    const char* _data;
    size_t _size;

    static std::map<std::string, Resource> _resources;
public:
    Resource(const char*, const char*);

    const char* const& data() const;
    const size_t& size() const;

    static const Resource& getResource(const std::string& name);
};

#define LOAD_RESOURCE(x) ([]() {\
    extern const char _binary_##x##_start, _binary_##x##_end;\
    return Resource(&_binary_##x##_start, &_binary_##x##_end);\
}())


#endif //HTTPWEBCHAT_RESOURCE_H

#include "resource.h"

std::map<std::string, Resource> Resource::_resources = {{"index.html", LOAD_RESOURCE(index_html)},
                                                        {"chat.css", LOAD_RESOURCE(chat_css)},
                                                        {"favicon.ico", LOAD_RESOURCE(favicon_ico)},
                                                        {"green_light.png", LOAD_RESOURCE(green_light_png)},
                                                        {"red_light.png", LOAD_RESOURCE(red_light_png)},
                                                        {"chat.js", LOAD_RESOURCE(chat_js)},
                                                        {"jquery.js", LOAD_RESOURCE(jquery_js)}};

Resource::Resource(const char* begin, const char* end): _data(begin), _size(end - begin) {}

const char* const& Resource::data() const {
    return _data;
}

const size_t& Resource::size() const {
    return _size;
}

const Resource& Resource::getResource(const std::string& name) {
    return _resources.at(name);
}

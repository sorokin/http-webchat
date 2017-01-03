#ifndef HTTPWEBCHAT_JSON_H
#define HTTPWEBCHAT_JSON_H


#include <map>
#include <vector>

#include "../common.h"

class JSON {
public:
    enum Type {NULL_VALUE, BOOLEAN, INTEGER, DOUBLE, STRING, ARRAY, OBJECT};
private:
    Type type;

    union {
        bool booleanValue;
        long integerValue;
        double doubleValue;
        std::string stringValue;
        std::vector<JSON> arrayElements;
        std::map<std::string, JSON> objectFields;
    };

    static JSON _parseJSON(const std::string&, size_t&);
public:
    JSON();
    JSON(bool);
    JSON(long);
    JSON(double);
    JSON(const std::string&);
    JSON(const std::vector<JSON>&);
    JSON(const std::map<std::string, JSON>&);

    Type getType();

    static JSON parseJSON(const std::string&);
    std::string toString() const;
};


#endif //HTTPWEBCHAT_JSON_H

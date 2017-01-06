#include "json.h"

JSON::JSON(): type(NULL_VALUE) {}

JSON::JSON(bool booleanValue): type(BOOLEAN), booleanValue(booleanValue) {}

JSON::JSON(long integerValue): type(INTEGER), integerValue(integerValue) {}

JSON::JSON(double doubleValue): type(DOUBLE), doubleValue(doubleValue) {}

JSON::JSON(const std::string& stringValue): type(STRING), stringValue(stringValue) {}

JSON::JSON(const std::vector<JSON>& arrayElements): type(ARRAY), arrayValue(arrayElements) {}

JSON::JSON(const std::map<std::string, JSON>& objectFields): type(OBJECT), objectValue(objectFields) {}

JSON JSON::_parseJSON(const std::string& data, size_t& cur) {
    for (; cur < data.size()
           && (data[cur] == ' ' || data[cur] == '\t' || data[cur] == '\r' || data[cur] == '\n'); ++cur);
    if (cur == data.size()) {
        throw std::runtime_error("Wrong JSON (unfinished): " + data);
    }
    if (data[cur] == 'n') {
        if (data[cur + 1] == 'u' && data[cur + 2] == 'l' && data[cur + 3] == 'l') {
            cur += 4;
            return JSON();
        } else {
            throw std::runtime_error("Wrong JSON (couln't parse null) at symbol " + std::to_string(cur) + ": \""
                                     + data + "\"");
        }
    } else if (data[cur] == 't') {
        if (data[cur + 1] == 'r' && data[cur + 2] == 'u' && data[cur + 3] == 'e') {
            cur += 4;
            return JSON(true);
        } else {
            throw std::runtime_error("Wrong JSON (couldn't parse true) at symbol " + std::to_string(cur) + ": \""
                                     + data + "\"");
        }
    } else if (data[cur] == 'f') {
        if (data[cur + 1] == 'a' && data[cur + 2] == 'l' && data[cur + 3] == 's' && data[cur + 4] == 'e') {
            cur += 5;
            return JSON(false);
        } else {
            throw std::runtime_error("Wrong JSON (couldn't parse false) at symbol " + std::to_string(cur) + ": \""
                                     + data + "\"");
        }
    } else if (data[cur] == '-') {
        std::string numberAsString = "" + data[cur++];
        for (; cur < data.size() && data[cur] >= '0' && data[cur] <= '9'; ++cur) {
            numberAsString += data[cur];
        }
        if (data[cur] != '.') {
            return JSON(std::stol(numberAsString));
        } else {
            numberAsString += data[cur++];
            for (; cur < data.size() && data[cur] >= '0' && data[cur] <= '9'; ++cur) {
                numberAsString += data[cur];
            }
            return JSON(std::stod(numberAsString));
        }
    } else if (data[cur] >= '0' && data[cur] <= '9') {
        std::string numberAsString = "" + data[cur];
        for (; cur < data.size() && data[cur] >= '0' && data[cur] <= '9'; ++cur) {
            numberAsString += data[cur];
        }
        if (data[cur] != '.') {
            return JSON(std::stol(numberAsString));
        } else {
            numberAsString += data[cur++];
            for (; cur < data.size() && data[cur] >= '0' && data[cur] <= '9'; ++cur) {
                numberAsString += data[cur];
            }
            return JSON(std::stod(numberAsString));
        }
    } else if (data[cur] == '\"') {
        ++cur;

        std::string string;
        size_t fnes;
        for (fnes = cur; cur < data.size() && data[cur] != '\"'; ++cur) {
            if (data[cur] == '\\') {
                if (fnes < cur) {
                    string += data.substr(fnes, cur - fnes);
                }
                if (data[cur + 1] == '"') {
                    string += '"';
                } else if (data[cur + 1] == 'n') {
                    string += '\n';
                } else if (data[cur + 1] == '\\') {
                    string += '\\';
                } else {
                    throw std::runtime_error("Wrong JSON (escaping symbol other than '\"', '\\' or \\n) at symbol "
                                             + std::to_string(cur + 1) + ": \"" + data + "\"");
                }
                fnes = cur + 2;
                ++cur;
            }
        }
        if (fnes < cur) {
            string += data.substr(fnes, cur - fnes);
        }

        if (cur == data.size()) {
            throw std::runtime_error("Wrong JSON (unfinished): " + data);
        } else {
            ++cur;
            return JSON(string);
        }
    } else if (data[cur] == '[') {
        ++cur;

        std::vector<JSON> arrayElements;
        while (cur < data.size() && data[cur] != ']') {
            for (; cur < data.size()
                   && (data[cur] == ' ' || data[cur] == '\t' || data[cur] == '\r' || data[cur] == '\n'); ++cur);
            if (cur == data.size()) {
                throw std::runtime_error("Wrong JSON (unfinished): " + data);
            }

            arrayElements.push_back(_parseJSON(data, cur));

            for (; cur < data.size()
                   && (data[cur] == ' ' || data[cur] == '\t' || data[cur] == '\r' || data[cur] == '\n'); ++cur);
            if (cur == data.size()) {
                throw std::runtime_error("Wrong JSON (unfinished): " + data);
            }

            if (data[cur] == ']') {
                break;
            } else if (data[cur++] != ',') {
                throw std::runtime_error("Wrong JSON (should be comma in array) at symbol " + std::to_string(cur)
                                         + ": \"" + data + "\"");
            }
        }

        if (cur == data.size()) {
            throw std::runtime_error("Wrong JSON (unfinished): " + data);
        } else {
            ++cur;
            return JSON(arrayElements);
        }
    } else if (data[cur] == '{') {
        ++cur;

        std::map<std::string, JSON> objectValues;
        while (cur < data.size() && data[cur] != ']') {
            for (; cur < data.size()
                   && (data[cur] == ' ' || data[cur] == '\t' || data[cur] == '\r' || data[cur] == '\n'); ++cur);
            if (cur == data.size()) {
                throw std::runtime_error("Wrong JSON (unfinished): " + data);
            }

            JSON key = _parseJSON(data, cur);
            if (key.getType() != STRING) {
                throw std::runtime_error("Wrong JSON (key type isn't a string) at symbol " + std::to_string(cur)
                                         + ": \"" + data + "\"");
            }

            for (; cur < data.size()
                   && (data[cur] == ' ' || data[cur] == '\t' || data[cur] == '\r' || data[cur] == '\n'); ++cur);
            if (cur == data.size()) {
                throw std::runtime_error("Wrong JSON (unfinished): " + data);
            } else if (data[cur++] != ':') {
                throw std::runtime_error("Wrong JSON (should be colon between key and value in object) at symbol "
                                         + std::to_string(cur) + ": \"" + data + "\"");
            }
            for (; cur < data.size()
                   && (data[cur] == ' ' || data[cur] == '\t' || data[cur] == '\r' || data[cur] == '\n'); ++cur);
            if (cur == data.size()) {
                throw std::runtime_error("Wrong JSON (unfinished): " + data);
            }

            JSON value = _parseJSON(data, cur);
            objectValues[key.stringValue] = value;

            for (; cur < data.size()
                   && (data[cur] == ' ' || data[cur] == '\t' || data[cur] == '\r' || data[cur] == '\n'); ++cur);
            if (cur == data.size()) {
                throw std::runtime_error("Wrong JSON (unfinished): " + data);
            }

            if (cur == data.size()) {
                throw std::runtime_error("Wrong JSON (unfinished): " + data);
            } else if (data[cur] == '}') {
                break;
            } else if (data[cur++] != ',') {
                throw std::runtime_error("Wrong JSON (should be comma in object) at symbol " + std::to_string(cur)
                                         + ": \"" + data + "\"");
            }
        }

        if (cur == data.size()) {
            throw std::runtime_error("Wrong JSON (unfinished): " + data);
        } else {
            ++cur;
            return JSON(objectValues);
        }
    } else {
        throw std::runtime_error("Wrong JSON (new entity isn't parsable) at symbol " + std::to_string(cur) + ": \""
                                 + data + "\"");
    }
}

JSON::Type JSON::getType() const {
    return type;
}

bool JSON::getBooleanValue() const {
    if (type != BOOLEAN) {
        throw std::runtime_error("Taking boolean value from JSON type " + type);
    }
    return booleanValue;
}

long JSON::getIntegerValue() const {
    if (type != INTEGER) {
        throw std::runtime_error("Taking integer value from JSON type " + type);
    }
    return integerValue;
}

double JSON::getDoubleValue() const {
    if (type != DOUBLE) {
        throw std::runtime_error("Taking double value from JSON type " + type);
    }
    return doubleValue;
}

std::string JSON::getStringValue() const {
    if (type != STRING) {
        throw std::runtime_error("Taking string value from JSON type " + type);
    }
    return stringValue;
}

std::vector<JSON> JSON::getArrayValue() const {
    if (type != ARRAY) {
        throw std::runtime_error("Taking array value from JSON type " + type);
    }
    return arrayValue;
}

std::map<std::string, JSON> JSON::getObjectValue() const {
    if (type != OBJECT) {
        throw std::runtime_error("Taking boolean value from JSON type " + type);
    }
    return objectValue;
}

JSON JSON::parseJSON(const std::string& data) {
    size_t cur = 0;
    JSON object = _parseJSON(data, cur);
    for (; cur < data.size()
           && (data[cur] == ' ' || data[cur] == '\t' || data[cur] == '\r' || data[cur] == '\n'); ++cur);
    if (cur != data.size()) {
        throw std::runtime_error("Wrong JSON (unsuspected end of JSON) at symbol " + std::to_string(cur) + ": \""
                                 + data + "\"");
    } else {
        return object;
    }
}

std::string JSON::toString() const {
    switch (type) {
        case NULL_VALUE: {
            return "null";
        } case BOOLEAN: {
            return booleanValue ? "true" : "false";
        } case INTEGER: {
            return std::to_string(integerValue);
        } case DOUBLE: {
            return std::to_string(doubleValue);
        } case STRING: {
            std::string encodedValue;
            for (size_t i = 0; i < stringValue.size(); ++i) {
                if (stringValue[i] == '"') {
                    encodedValue += "\\\"";
                } else if (stringValue[i] == '\n') {
                    encodedValue += "\\n";
                } else if (stringValue[i] == '\\') {
                    encodedValue += "\\\\";
                } else {
                    encodedValue += stringValue[i];
                }
            }

            return "\"" + encodedValue + "\"";
        } case ARRAY: {
            std::string result = "[";
            for (std::vector<JSON>::const_iterator it = arrayValue.begin();
                    it != arrayValue.end(); ++it) {
                if (it != arrayValue.begin()) {
                    result += ", ";
                }
                result += it->toString();
            }
            result += "]";
            return result;
        } case OBJECT: {
            std::string result = "{";
            for (std::map<std::string, JSON>::const_iterator it = objectValue.begin();
                 it != objectValue.end(); ++it) {
                if (it != objectValue.begin()) {
                    result += ", ";
                }
                result += "\"" + it->first + "\": " + it->second.toString();
            }
            result += "}";
            return result;
        } default: {
            throw std::runtime_error("Impossible JSON parse exception");
        }
    }
}

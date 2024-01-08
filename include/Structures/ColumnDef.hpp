#pragma once
#include <string>
#include <sstream>
#include "Utils.hpp"

namespace DBaseTools
{

struct ColumnDef
{
    void from_binary(std::string& data)
    {
        field_name = data.substr(0, 10);
        field_name = std::string(field_name.data()); // remove trailing '\0'
        field_name = trim(field_name);
        field_length = (uint8_t)data.at(16);
    }

    std::string to_binary()
    {
        if(field_name.size() > 10)
            throw std::runtime_error(
                "field_name too long, max length is 10. "
                "But got length: " + std::to_string(field_name.size()) + ". "
                "field_name: " + field_name
            );

        std::string ret(32, '\0');
        ret.replace(0, field_name.size(), field_name);
        ret.at(11) = field_type;
        ret.at(16) = uint8_t(field_length);

        return ret;
    }

    std::string to_debug_string()
    {
        std::stringstream ss;
        ss << "field_name: \"" << field_name << "\"" << std::endl;
        ss << "field_type: Characters" << std::endl;
        ss << "field_length: " << field_length << std::endl;

        return ss.str();
    }

    std::string field_name = "";   // 0~10: Name of field
    char field_type = 'C';         // 11: 'C stands for String type, we only support string type
    // 12~15 : not used, fill with zeros
    std::size_t field_length = 0;       // 16: length of field
    // 17~31 : not used, fill with zero
};

};


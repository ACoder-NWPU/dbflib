#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Utils.hpp"
#include "ColumnDef.hpp"

namespace DBaseTools
{

struct Record
{
    void from_binary(const std::string& data, const std::vector<std::shared_ptr<ColumnDef>>& col_defs)
    {
        std::size_t curPos = 1; // first byte is deleted flag, 0x20 means not deleted, we ignore this flag
        for (auto column : col_defs)
        {
            std::string str = data.substr(curPos, column->field_length);
            str = trim(str);
            contents[column->field_name] = str;
            curPos += column->field_length;
        }
    }

    std::string to_binary(
        std::shared_ptr<const Header> header,
        const std::vector<std::shared_ptr<ColumnDef>>& col_defs
        ) const
    {
        std::string ret(header->bytes_per_record, '\0');
        std::size_t curPos = 0;
        for (std::size_t i = 0; i < col_defs.size(); ++i)
        {
            std::string k = col_defs[i]->field_name;
            std::string v = contents.at(k);
            std::size_t field_length = col_defs[i]->field_length;
            ret.at(curPos) = 0x20; // deleted flag, 0x20 means not deleted
            ret.replace(curPos+1, std::min(field_length-1, v.size()), v); // +1 for deleted flag, 0x20 means not deleted
            curPos += field_length;
        }
        return ret;
    }

    std::string to_debug_string(char sep='\n') const
    {
        std::stringstream ss;
        for (const auto& pr : contents)
        {
            std::string k = pr.first;
            std::string v = pr.second;
            ss << "\"" << k << "\" = \"" <<  v << "\"" << sep;
        }
        return ss.str();
    }

    std::map<std::string, std::string> contents;
};

}
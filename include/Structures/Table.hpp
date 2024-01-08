#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Header.hpp"
#include "ColumnDef.hpp"
#include "Record.hpp"

namespace DBaseTools
{

struct Table
{
    std::string to_debug_string()
    {
        std::stringstream ss;

        ss << " --- Header --- " << std::endl << std::endl;
        ss << header->to_debug_string() << std::endl;

        ss << " --- Column Definitions --- " << std::endl << std::endl;
        for(std::size_t i=0; i<col_defs.size(); ++i)
        {
            ss << "column " << i << std::endl;
            ss << col_defs[i]->to_debug_string() << std::endl;
        }

        ss << " --- Records --- " << std::endl << std::endl;
        for(std::size_t i=0; i<records.size(); ++i)
        {
            ss << "record " << i << std::endl;
            ss  << records[i]->to_debug_string() << std::endl;
        }
        return ss.str();
    }

    std::shared_ptr<Header> header;
    std::vector<std::shared_ptr<ColumnDef>> col_defs;
    std::vector<std::shared_ptr<Record>> records;
};

}
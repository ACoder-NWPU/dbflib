#pragma once

#include <memory>
#include <chrono>
#include <Structures/Table.hpp>

namespace DBaseTools
{

// This class helps you to manipulate a table.
// If you want to create a new table, you can do like this:
//      TableBuilder(std::make_shared<Table>());
//      builder.set_columns({{"name", 10}, {"age", 3}});
//      builder.append_record({{"name", "John"}, {"age", "20"}});
//      builder.append_record({{"name", "Mary"}, {"age", "21"}});
// If you want to modify an existing table, you can do like this:
//      TableBuilder(table);
//      builder.append_record({{"name", "John"}, {"age", "20"}});
// Notice that the header will automatically be updated.
struct TableBuilder
{
    TableBuilder(std::shared_ptr<Table> table) : table(table)
    {
    }

    void set_columns(std::vector<std::tuple<std::string,std::size_t>> name_length_tuples)
    {
        auto col_defs = std::vector<std::shared_ptr<ColumnDef>>();
        for (auto name_length_tuple : name_length_tuples)
        {
            auto col_def = std::make_shared<ColumnDef>();
            col_def->field_name = std::get<0>(name_length_tuple);
            col_def->field_length = std::get<1>(name_length_tuple) + 1; // +1 for deleted flag
            col_defs.emplace_back(std::move(col_def));
        }

        table->col_defs = std::move(col_defs);
        regenerate_header();
    }

    void append_record(std::map<std::string, std::string> contents)
    {
        // check if contents has exactly the same keys as col_defs
        {
            if (contents.size() != table->col_defs.size())
                throw std::runtime_error(
                    "contents.size() != table->col_defs.size(), "
                    "contents.size() = " + std::to_string(contents.size()) + ", " +
                    "table->col_defs.size() = " + std::to_string(table->col_defs.size())
                );

            for (auto col_def : table->col_defs)
                if (contents.find(col_def->field_name) == contents.end())
                    throw std::runtime_error("Cannot find column " + col_def->field_name + " in contents");
        }

        // check if values of contents has valid length
        {
            for(const auto& col_def : table->col_defs)
            {
                auto field_name = col_def->field_name;
                auto field_length = col_def->field_length;
                auto field_value = contents[field_name];
                if (field_value.size() > field_length - 1) // -1 for deleted flag
                    throw std::runtime_error(
                        "Value of field [" + field_name + "] is too long! " +
                        "Limit = " + std::to_string(field_length - 1) + ", " +
                        "Actual = " + std::to_string(field_value.size())
                    );
            }
        }

        auto record = std::make_shared<Record>();
        record->contents = std::move(contents);
        table->records.emplace_back(std::move(record));
        regenerate_header();
    }

    std::shared_ptr<Table> table;

private:

    void regenerate_header()
    {
        auto header = std::make_shared<Header>();
        header->records_cnt = table->records.size();
        header->header_total_bytes = 32 + table->col_defs.size() * 32 + 1; // header(32) + column_defs(32 * n) + terminator(1)

        header->bytes_per_record = 0;
        for (auto col_def : table->col_defs)
            header->bytes_per_record += col_def->field_length;

        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        auto now_tm = std::localtime(&now_time_t);
        header->last_updated = (now_tm->tm_year) | (now_tm->tm_mon + 1 << 8) | (now_tm->tm_mday << 16);

        table->header = std::move(header);
    }

};



}
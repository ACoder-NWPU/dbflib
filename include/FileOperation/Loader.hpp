#pragma once

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <tuple>
#include "Structures/Table.hpp"

namespace DBaseTools
{

// This class is used to load a table from *.dbf file.
// If you want to parse a dbf file into a new table, you can do like this:
//      Loader loader("test.dbf");
//      auto table = loader.load_table();
// If you want to update an existing table, you can do like this:
//      Loader loader("test.dbf");
//      loader.update_table(table);
struct Loader
{
    Loader(const std::string& filename) : fin(filename, std::ios::binary)
    {
        if (!fin)
            throw std::runtime_error("Cannot open file " + filename);
    }

    // Load a table from file
    std::shared_ptr<Table> load_table()
    {
        std::size_t file_size = get_file_size();

        auto header = load_header();

        // header->header_total_bytes = 32 + columns_count * column_def_size(32) + terminator(1)
        std::size_t columns_cnt = (header->header_total_bytes - 32 - 1) / 32;
        std::vector<std::shared_ptr<ColumnDef>> col_defs;
        for (std::size_t i = 0; i < columns_cnt; ++i)
            col_defs.push_back(load_column_def(i));

        // record_size = deleted_flag(1) + sum(column_def->field_length)
        std::vector<std::shared_ptr<Record>> records;
        for (std::size_t i = 0; i < header->records_cnt; ++i)
            records.push_back(load_record(col_defs, header->bytes_per_record, i));

        auto table = std::make_shared<Table>();
        table->header = std::move(header);
        table->col_defs = std::move(col_defs);
        table->records = std::move(records);
        return table;
    }

    // Incrementally update a table from file, return old and new records count
    std::tuple<std::size_t, std::size_t> update_table(std::shared_ptr<Table> table)
    {
        std::size_t file_size = get_file_size();

        auto header = load_header();
        std::size_t old_records_cnt = table->header->records_cnt;
        std::size_t new_records_cnt = header->records_cnt;

        if (new_records_cnt <= old_records_cnt) // if new<old, there must be something wrong, if new=old, no need to update
            return std::make_tuple(old_records_cnt, new_records_cnt);

        std::vector<std::shared_ptr<Record>> new_records;
        for (std::size_t i = old_records_cnt; i < new_records_cnt; ++i)
            new_records.push_back(load_record(table->col_defs, table->records[0]->contents.size(), i));

        table->header = std::move(header);
        table->records.insert(table->records.end(), new_records.begin(), new_records.end());

        return std::make_tuple(old_records_cnt, new_records_cnt);
    }

    std::ifstream fin;

private:
    std::size_t get_file_size()
    {
        fin.seekg(0, fin.end);
        return fin.tellg();
    }

    std::shared_ptr<Header> load_header()
    {
        if (32 > get_file_size())
            throw std::runtime_error(
                "File is too small to contain header, need = 32, file_size = " + std::to_string(get_file_size())
            );

        auto header = std::make_shared<Header>();
        std::string buf(32, '\0');
        fin.seekg(0, fin.beg); // go to beginning of file
        fin.read(&buf.at(0), 32);
        header->from_binary(buf);

        return header;
    }

    std::shared_ptr<ColumnDef> load_column_def(std::size_t column_index)
    {
        std::size_t offset = 32 + column_index * 32; // offset = header(32) + column_index * column_def_size(32)
        if (offset + 32 > get_file_size())
            throw std::runtime_error(
                "File is too small to contain column definition, need = " + std::to_string(offset + 32) +
                    ", file_size = " + std::to_string(get_file_size())
            );

        std::string buf(32, '\0');
        fin.seekg(offset, fin.beg);
        fin.read(&buf.at(0), 32);
        auto column_def = std::make_shared<ColumnDef>();
        column_def->from_binary(buf);

        return column_def;
    }

    std::shared_ptr<Record> load_record(std::vector<std::shared_ptr<ColumnDef>> col_defs,
        size_t record_size,
        std::size_t record_index)
    {
        // offset = header(32) + columns_count * column_def_size(32) + record_index * record_size
        std::size_t offset = 32 + col_defs.size() * 32 + record_index * record_size;
        if (offset + record_size > get_file_size())
            throw std::runtime_error(
                "File is too small to contain record, need = " + std::to_string(offset + record_size) +
                    ", file_size = " + std::to_string(get_file_size())
            );

        std::string buf(record_size, '\0');
        fin.seekg(offset, fin.beg);
        fin.read(&buf.at(0), record_size);
        auto record = std::make_shared<Record>();
        record->from_binary(buf, col_defs);

        return record;
    }
};

}
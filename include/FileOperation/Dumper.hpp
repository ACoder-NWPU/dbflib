#pragma once

#include <string>
#include <fstream>
#include "Utils.hpp"
#include "Structures/Table.hpp"

namespace DBaseTools
{

struct Dumper
{
    Dumper(const std::string& filename) : fout(filename, std::ios::binary | std::ios::ate)
    {
        if (!fout)
            throw std::runtime_error("Cannot open file " + filename);
    }

    void dump_all(std::shared_ptr<const Table> table)
    {
        dump_header(table->header);

        for (std::size_t i = 0; i < table->col_defs.size(); ++i)
            dump_column_def(table->col_defs[i], 32 + i * 32);
        fout.seekp(32 + table->col_defs.size() * 32, std::ios::beg);
        fout.write("\x0D", 1); // terminator

        for (std::size_t i = 0; i < table->records.size(); ++i)
            dump_record(
                32 + table->col_defs.size() * 32 + 1 + i * table->header->bytes_per_record,
                table->records[i], table->header, table->col_defs
            );
        dump_file_terminator();
    }

    void dump_part(std::shared_ptr<const Table> table, std::size_t row_begin, std::size_t row_end)
    {
        if (row_end > table->records.size() || row_begin > row_end)
            throw std::runtime_error(
                "Invalid row_begin or row_end, row_begin = " + std::to_string(row_begin) +
                    ", row_end = " + std::to_string(row_end) + ", records.size() = "
                    + std::to_string(table->records.size())
            );

        std::size_t begin_pos = 32 + table->col_defs.size() * 32 + 1 + row_begin * table->header->bytes_per_record;
        if (begin_pos > get_file_size())
            throw std::runtime_error(
                "Invalid row_begin, begin_pos = " + std::to_string(begin_pos) + ", file_size = "
                    + std::to_string(get_file_size())
            );

        for (std::size_t i = row_begin; i < row_end; ++i)
            dump_record(
                32 + table->col_defs.size() * 32 + 1 + i * table->header->bytes_per_record,
                table->records[i], table->header, table->col_defs
            );
        dump_header(table->header);
        dump_file_terminator();
    }

    void flush()
    {
        fout.flush();
    }

    std::ofstream fout;

private:

    std::size_t get_file_size()
    {
        fout.seekp(0, fout.end);
        return fout.tellp();
    }

    void dump_header(std::shared_ptr<Header> header)
    {
        fout.seekp(0, std::ios::beg);
        std::string header_data = header->to_binary();
        fout.write(header_data.data(), header_data.size());
    }

    void dump_column_def(std::shared_ptr<ColumnDef> col_def, std::size_t begin_pos)
    {
        fout.seekp(begin_pos, std::ios::beg);
        std::string col_def_data = col_def->to_binary();
        fout.write(col_def_data.data(), col_def_data.size());
    }

    void dump_record(
        std::size_t begin_pos, std::shared_ptr<Record> record,
        std::shared_ptr<const Header> header,
        const std::vector<std::shared_ptr<ColumnDef>>& col_def)
    {
        fout.seekp(begin_pos, std::ios::beg);
        std::string record_data = record->to_binary(header, col_def);
        fout.write(record_data.data(), record_data.size());
    }

    void dump_file_terminator()
    {
        fout.seekp(0, std::ios::end);
        fout.write("\x1A", 1);
    }
};

}
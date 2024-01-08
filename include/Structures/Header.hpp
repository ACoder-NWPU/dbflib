#pragma once

#include <string>
#include <ctime>
#include <sstream>
#include <bitset>

namespace DBaseTools
{

struct Header
{
    void from_binary(const std::string& data)
    {
        uint8_t current_byte;

        if (data.size() != 32)
        {
            throw std::runtime_error("DBaseHeader::from_binary: data.size() != 32");
        }

        // read file into header_data
        for (std::size_t i = 0; i < 12ull; i++)
        {
            current_byte = data.at(i);

            switch (i)
            {
            case 0:
            {
                // File type
                break;
            }
            case 1:
            {   // years since 1900
                last_updated |= current_byte;
                break;
            }
            case 2:
            {   //month: 0-11
                last_updated |= (current_byte << 8);
                break;
            }
            case 3:
            {   //day: 1-31
                last_updated |= (current_byte << 16);
                break;
            }
            case 4:
            case 5:
            case 6:
            case 7:
            { //Number of records in table
                records_cnt += (current_byte << (8 * (i - 4)));
                break;
            }
            case 8:
            case 9:
            { //Position of first data record
                header_total_bytes += (current_byte << (8 * (i - 8)));
                break;
            }
            case 10:
            case 11:
            { //Length of one data record incl. deleting flag
                bytes_per_record += (current_byte << (8 * (i - 10)));
                break;
            }
            case 14:
            {
                break;
            }
            case 15:
            {
                // encrypted
                break;
            }
            case 28:
            {
                break;
            }
            case 29:
            {
                // code_page_mark
                break;
            }

            }
        } //end for loop read header
    }

    std::string to_binary() const
    {
        std::string ret(32, '\0');

        ret[0] = 0x03; // File type

        ret[1] = (last_updated & 0xFF); // years since 1900
        ret[2] = ((last_updated >> 8) & 0xFF); //month: 0-11
        ret[3] = ((last_updated >> 16) & 0xFF); //day: 1-31

        // Number of records in table, 32-bit, little endian
        ret[4] = (records_cnt & 0xFF);
        ret[5] = ((records_cnt >> 8) & 0xFF);
        ret[6] = ((records_cnt >> 16) & 0xFF);
        ret[7] = ((records_cnt >> 24) & 0xFF);

        // Number of bytes in header, 16-bit, little endian
        ret[8] = (header_total_bytes & 0xFF);
        ret[9] = ((header_total_bytes >> 8) & 0xFF);

        // Number of bytes in record, 16-bit, little endian
        ret[10] = (bytes_per_record & 0xFF);
        ret[11] = ((bytes_per_record >> 8) & 0xFF);

        return ret;
    }

    std::string to_debug_string() const
    {
        std::stringstream ss;
        ss << "Header length:\t\t\t\t" << header_total_bytes << " bytes" << std::endl;
        ss << "Last updated:\t\t\t\t" << (last_updated & 0xFF) + 1900 << "-" << ((last_updated >> 8) & 0xFF) << "-"
           << ((last_updated >> 16) & 0xFF) << std::endl;
        ss << "Number of records:\t\t\t" << (int)records_cnt << std::endl;
        ss << "Number of bytes in header:\t\t" << (int)header_total_bytes << std::endl;
        ss << "Number of bytes per record:\t\t" << (int)bytes_per_record << std::endl;

        return ss.str();
    }

    /// BYTE 0, always let it to be 0x03
    uint32_t last_updated = 0;                 /// BYTE 1-3, YYMMDD format, year=(BYTE1)+1900, month=(BYTE2)+1, day=BYTE3
    uint32_t records_cnt = 0;          /// BYTE 4-7, 32-bit, number of records
    uint64_t header_total_bytes = 0;   /// BYTE 8-9, 16 bit, number of bytes in header(including column definitions)
    uint16_t bytes_per_record = 0;    /// BYTE 10-11, 16 bit, number of bytes in one record
    /// BYTE 12-31 are zeros
};

}
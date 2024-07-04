#include <iostream>
#include "DBaseTools.hpp"

void load_demo(std::string filename)
{
    try
    {
        DBaseTools::Loader loader(filename);
        auto table = loader.load_table();
        std::cout << table->to_debug_string() << std::endl;

        /*

         after loading, you can access the table like this:

        table->header->last_updated;
        table->header->records_cnt;
        table->col_defs[0]->field_name;
        table->records[0]->contents["STOCK_CODE"];

         */
    }
    catch (std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void dump_demo(std::string filename)
{
    try
    {
        DBaseTools::TableBuilder builder(std::make_shared<DBaseTools::Table>());

        builder.set_columns({{"name", 10}, {"age", 3}});
        builder.append_record({{"name", "John"}, {"age", "20"}});
        builder.append_record({{"name", "Mary"}, {"age", "21"}});
        builder.append_record({{"name", "Tom"}, {"age", "22"}});

        auto table = builder.table;
        std::cout << table->to_debug_string() << std::endl;

        DBaseTools::Dumper dumper(filename);
        dumper.dump_all(builder.table);
        dumper.flush();
    }
    catch (std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main()
{
    load_demo("D:/WeChat/WeChat Files/wxid_cn79p3oem8dl22/FileStorage/File/2024-07/trade(1).dbf");
//    dump_demo();
//    load_demo("C:/Users/wyw15/Desktop/pingan_pb/export_data/XT_DBF_ORDER.dbf");

    return 0;
}

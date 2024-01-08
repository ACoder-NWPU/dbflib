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

void xt_place_order_demo(std::string filename)
{
    try
    {
        DBaseTools::Loader loader(filename);
        auto table = loader.load_table();

        DBaseTools::TableBuilder builder(table);

        builder.append_record(
            {
                {"ORDER_TYPE", "23"}, // 证券买入
                {"PRICE_TYPE", "3"}, // 限价委托
                {"MODE_PRICE", "10.5"},
                {"STOCK_CODE", "000001"},
                {"VOLUME", "500"},
                {"ACCOUNT_ID", "301300110012"},
                {"ACT_TYPE", "49"},
                {"BROKERTYPE", "2"},
                {"STRATEGY", ""},
                {"NOTE", "博普bopu投资备注测试12345-_+=*&^%$#@!`~"},
                {"NOTE1", ""},
                {"COMPO_PATH", ""},
                {"TRADEPARAM", ""},
                {"COMMAND_ID", ""},
                {"BASKETPATH", ""},
                {"INSERTTIME", ""},
                {"EXTRAPARAM", ""}
            }
        );

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
//    load_demo("D:/projects/DBaseTools/XT_ZJCX_Stock_20240102.dbf");
//    dump_demo();
//    load_demo("C:/Users/wyw15/Desktop/pingan_pb/export_data/XT_DBF_ORDER.dbf");

    xt_place_order_demo(R"(C:\Users\wyw15\Desktop\pingan_pb\export_data\XT_DBF_ORDER.dbf)");

    return 0;
}

#include "stdafx.h"
#include <network/orm.hpp>
using namespace std;
using namespace std::string_literals;
using namespace cytx;
using namespace cytx::orm;

struct user_bean
{
    int id;
    std::string name;
    DB_META(user_bean, id, name);
};

struct t_server
{
    int id;
    string name;
    string ip;
    int port;
    int valid;
    DB_META(t_server, id, name, ip, port, valid);
};

//struct t_server1
//{
//    int id;
//    string name;
//    string ip;
//    int port;
//    int valid;
//};

int main(int argc, char **argv)
{
    /*auto t_server_size = sizeof(t_server);
    auto t_server_size1 = sizeof(t_server1);
    auto str_size = sizeof(string);
    cout << fmt::format("{}, {}, {}", t_server_size, t_server_size1, str_size);*/

    std::string sql = R"--(CREATE TABLE IF NOT EXISTS `t_server` (
        `id` int(11) NOT NULL,
        `name` varchar(255) DEFAULT NULL,
        `ip` varchar(255) NOT NULL,
        `port` int(11) NOT NULL,
        `valid` tinyint(4) NOT NULL
        ) ENGINE = InnoDB DEFAULT CHARSET = utf8; )--";

    table_factory::instance().register_table<t_server>(sql);

    smart_db db("ip=localhost;port=3306;user=root;pwd=123456");
    try
    {
        db.connect();
        db.create_db("test");
        db.create_table<t_server>();
        auto row_count = db.execute_scalar("select count(*) from t_server");
        cout << row_count << endl;

        t_server t;
        t._id = 1, t._ip = "localhost", t._name = "test", t._port = 12345, t._valid = 1;
        db.insert(t);
        db.insert(t._id = 9, t._ip = "localhost", t._name = "test", t._port = 12345, t._valid = 1);
        t_server t1;
        auto r = db.update(t1._name = "test_update aadd").where(t._id == 9).where(t._id == 1).to_value();
        cout << "update " << r << endl;

        auto vr = db.query<t_server>().to_vector();
        auto vr1 = db.query<t_server>().where(t._id == 9).to_vector();
        auto vr2 = db.query<t_server>().where(t._id == 9).take(2).to_vector();

        auto dr = db.delete_item<t_server>().where(t._id == 1).to_value();
        cout << "delete " << dr << endl;

        auto dr1 = db.delete_item<t_server>().where(t._id == 9).to_value();
        cout << "delete " << dr1 << endl;
    }
    catch (mysql_exception& e)
    {
        cout << e.message() << endl;
    }
    return 0;
}
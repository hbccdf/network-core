#include <iostream>
#include <network/service/service_meta.hpp>
#include <network/service/service_manager.hpp>

using namespace cytx;

class base_service
{
public:
    virtual void show()
    {
        std::cout << "base" << std::endl;
    }
};

class derived_service : public base_service
{
public:
    void show() override
    {
        std::cout << "derived" << std::endl;
    }
};
REG_SERVICE(derived_service, base_service);

class derived_test_service : public base_service
{
public:
    void show() override
    {
        std::cout << "derived test" << std::endl;
    }
};
REG_SERVICE(derived_test_service, base_service);

int main(int argc, char* argv[])
{
    std::vector<base_service*> list = service_factory::ins().get_all_service<base_service>();
    for (base_service* base : list)
    {
        base->show();
    }


    base_service* base = service_factory::ins().get_service<base_service>();
    base->show();


    std::cout << std::endl;
    service_manager smgr;
    base_service* base1 = smgr.get_service<base_service>();
    smgr.register_service("derived_service");
    smgr.register_service("base_service");
    smgr.register_service("derived_test_service");
    base1 = smgr.get_service<base_service>();
    base1->show();

    std::vector<base_service*> list1 = smgr.get_all_service<base_service>();
    for (base_service* base : list1)
    {
        base->show();
    }
    int i = 0;
}
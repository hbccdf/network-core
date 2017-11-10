#include <iostream>
#include <network/service/service_meta.hpp>

using namespace cytx;

class base_service
{
public:
    virtual void show() = 0;
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

    int i = 0;
}
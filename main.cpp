#include <iostream>
#include <stdexcept>
#include <clxx/exception.hpp>
#include "exception_base.hpp"
struct A {};
struct exception3
    : public hybrid::exception_base<std::exception, A>
{
    typedef hybrid::exception_base<std::exception, A> Base;
    exception3() noexcept : Base("exception3") { }
};

int main()
{
    try { throw exception3(); }
    catch (A& e) { }
}

#ifndef HYBRID_EXCEPTION_BASE_HPP_INCLUDED
#define HYBRID_EXCEPTION_BASE_HPP_INCLUDED

#include <clxx/util/std_except_ctor_arg.hpp>
#include <type_traits>
#include <utility>
#include <tuple>

namespace hybrid { namespace detail {
    namespace has_standard_base_impl
    {
        template <class Tuple>
        struct has_standard_base;

        template <class... Ts>
        struct has_standard_base<std::tuple<Ts...>>
        {
            struct null;

            static std::false_type
            check(typename std::conditional<std::is_base_of<std::exception, Ts>::value, null, Ts>::type&...);

            static std::true_type check(...);

            using result = decltype(check(std::declval<Ts&>()...));
        };
    }

    template <class Tuple>
    struct has_standard_exception_base : has_standard_base_impl::has_standard_base<Tuple>::result { };
}}

namespace hybrid
{
    template <class Tuple, bool = detail::has_standard_exception_base<Tuple>::value>
    struct exception_base_impl;

    namespace detail
    {
        // primary std_except_ctor_arg has std_except_no_ctor_arg_tag as ::type member typedef
        template <class Exception, class = typename clxx::std_except_ctor_arg<Exception>::type>
        struct ctor_helper;
    }
}

namespace hybrid { namespace detail {
    template <class Exception, class>
    struct ctor_helper : Exception
    {
    protected:
        template <class... Args>
        explicit ctor_helper(Args&&... args) : Exception(std::forward<Args>(args)...) { }
    };

    template <class Exception>
    struct ctor_helper<Exception, clxx::std_except_no_ctor_arg_tag> : Exception
    {
    protected:
        template <class... Args>
        explicit ctor_helper(Args&&...) { }
    };
}} // end namespace hybrid, detail

namespace hybrid
{
    template <class... Ts>
    struct exception_base_impl<std::tuple<Ts...>, false> : detail::ctor_helper<Ts>...
    {
    public:
        // GCC says "invalid pack expansion" when unpacking a variadic args... into ctor_helper. Keeping it at one argument for now
        template <class Args>
        exception_base_impl(Args&& args)
            : detail::ctor_helper<Ts>(std::forward<Args>(args))...
        { }

        // internal compiler error on CodeBlocks GCC 4.8 with = default
        virtual ~exception_base_impl() noexcept { }
    };

    template <class... Ts>
    struct exception_base_impl<std::tuple<Ts...>, true> : exception_base_impl<std::tuple<Ts...>, false>
    {
        using exception_base_impl<std::tuple<Ts...>, false>::exception_base_impl;
        virtual std::exception const& hybrid2std() const noexcept { return *this; }
    };

    template <class... Exceptions>
    using exception_base = exception_base_impl<std::tuple<Exceptions...>>;
} // end namespace hybrid

#endif // HYBRID_EXCEPTION_BASE_HPP_INCLUDED

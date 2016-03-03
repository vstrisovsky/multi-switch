#include <iostream>
#include <tuple>
#include <functional>

namespace multiswitch
{

struct _Placeholder {} _;

template<std::size_t ... _Is>
struct _Indices {};

template<std::size_t _N, std::size_t... _Is>
struct _BuildIndices : _BuildIndices<_N-1, _N-1, _Is ...>
{};

template<std::size_t... _Is>
struct _BuildIndices<0, _Is ...> : _Indices<_Is ...>
{};

template<typename _Fnc, typename _Args, std::size_t ... _Is>
inline constexpr
auto _call(_Fnc &&f, _Args &&args, const _Indices<_Is...> &) -> decltype(f(std::get<_Is>(args)...))
{
   return f(std::get<_Is>(args)...);
}

template<typename _Fnc, typename _Args>
inline constexpr
auto call(_Fnc &&f, const _Args &args) -> decltype(_call(f, args, _BuildIndices< std::tuple_size<_Args>::value>()))
{
    return _call(f, args, _BuildIndices< std::tuple_size<_Args>::value>());
}

template<size_t _I>
struct _Index
{
    using _Next = _Index<_I+1>;
    static const constexpr size_t value = _I;
};

template<typename _Cs, typename _I>
struct _Evaluate
{
    template<typename _Values>
    inline constexpr
    static void _evaluate(const _Cs &cs, const _Values& values) {
        if (!std::get<_I::value>(cs)(values)) {
            _Evaluate<_Cs, typename _I::_Next>::_evaluate(cs, values);
        }
    };
};

template<typename _Cs>
struct _Evaluate<_Cs, _Index<std::tuple_size<_Cs>::value>>
{
    template<typename _Values>
    inline constexpr
    static void _evaluate(const _Cs &cs, const _Values& values) {
    };
};

template<typename _Cs, typename ... _T>
struct _Switch
{
    _Cs mCases;
    _Switch(_Cs && cases)
    : mCases(cases)
    {
    }

    void operator()(const _T& ... values)
    {
        _Evaluate<_Cs, _Index<0>>::_evaluate(mCases, std::make_tuple(std::cref(values)...));
    }
};

template<typename ... _T>
struct _Switch<void, _T ...>
{
    _Switch()
    {
    }
};

template<typename ... _T>
inline constexpr
_Switch<void, _T...> _switch()
{
    return _Switch<void, _T...>();
}

template<typename _Fnc, typename ... _T>
struct _Case
{
    _Fnc mFnc;
    std::tuple<_T ...> mValues;
    _Case(_Fnc&& fnc, std::tuple<_T ...>&& values)
    : mFnc(fnc),
      mValues(values)
    {}

    bool operator()(const std::tuple<const _T& ...>& values) const
    {
        if(values  == mValues) {
            call(mFnc, values);
            return true;
        }
        return false;
    }
};

template<typename ... _T>
struct _Case<void, _T...>
{
    std::tuple<_T ...> mValues;
    _Case(const _T & ... values)
    : mValues(values ...)
    {}

    template<typename _Fnc>
    _Case<_Fnc, _T ...> operator()(_Fnc&& fnc)
    {
        return _Case<_Fnc, _T ...>(std::forward<_Fnc>(fnc), std::move(mValues));
    }
};

template<typename ... _T>
inline constexpr
_Case<void, _T...> _case(_T && ... values)
{
    return _Case<void, _T...>(values ...);
}

template<typename _Cs, typename _Fnc, typename ... _T>
inline constexpr
auto operator <= (_Switch<_Cs, _T ...>&& s, _Case<_Fnc, _T ...>&& c) ->
    _Switch<decltype(std::tuple_cat(std::move(s.mCases), std::tuple<_Case<_Fnc, _T ...>>(std::forward<_Case<_Fnc, _T ...>>(c)))), _T ...>
{
    using _Result =  _Switch<decltype(std::tuple_cat(std::move(s.mCases), std::tuple<_Case<_Fnc, _T ...>>(std::forward<_Case<_Fnc, _T ...>>(c)))), _T ...>;
    return _Result(std::tuple_cat(std::move(s.mCases), std::tuple<_Case<_Fnc, _T ...>>(std::forward<_Case<_Fnc, _T ...>>(c))));
}

template<typename _Fnc, typename ... _T>
inline constexpr
_Switch<std::tuple<_Case<_Fnc, _T ...>>, _T ...> operator <= (_Switch<void, _T ...>&& s, _Case<_Fnc, _T ...>&& c)
{
    return _Switch<std::tuple<_Case<_Fnc, _T ...>>, _T ...>(std::tuple<_Case<_Fnc, _T ...>>(std::forward<_Case<_Fnc, _T ...>>(c)));
}

}

int main()
{
    using namespace multiswitch;
    int x = 1, y = 3;
    auto a = _switch<int, int>()
            <= _case(1,0)([](int, int){std::cout << "Hello, World 1-0!" << std::endl;})
            <= _case(1,1)([](int, int){std::cout << "Hello, World 1-1" << std::endl;})
            <= _case(1,3)([](int, int){std::cout << "Bingo 1-3" << std::endl;});

    a(x,y);

    std::string s1 = "test", s2 = "case";
    auto b = _switch<std::string, std::string>()
            <= _case(std::string("a"), std::string("b"))([](std::string, std::string){std::cout << "a-b" << std::endl;})
            <= _case(std::string("test"), std::string("x"))([](std::string, std::string){std::cout << "test-x" << std::endl;})
            <= _case(std::string("test"), std::string("case"))([](std::string, std::string){std::cout << "test-case" << std::endl;});

    b(s1, s2);

    return 0;
}

#include <iostream>
#include <tuple>
#include <functional>

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
    std::tuple<const _T& ...> mValues;
    _Case(_Fnc&& fnc, std::tuple<const _T& ...>&& values)
    : mFnc(fnc),
      mValues(values)
    {}

    bool operator()(_T&& ... values)
    {
        return true;
    }
};

template<typename ... _T>
struct _Case<void, _T...>
{
    std::tuple<const _T& ...> mValues;
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

int main()
{
    int x = 1, y = 3;
    auto a = _switch<int, int>()
            <= _case(1,0)([](int, int){std::cout << "Hello, World 1-0!" << std::endl;})
            <= _case(1,1)([](int, int){std::cout << "Hello, World 1-1" << std::endl;});

    a(x,y);
    return 0;
}
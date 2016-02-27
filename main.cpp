#include <iostream>
#include <tuple>

template<typename ... _T>
struct _Switch
{
    std::tuple<const _T& ...> mValues;
    _Switch(const _T& ... values)
    : mValues(values ...)
    {

    }
};

template<typename ... _T>
_Switch<_T...> _switch(const _T& ... values)
{
    return _Switch<_T...>(values ...);
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
_Case<void, _T...> _case(_T && ... values)
{
    return _Case<void, _T...>(values ...);
}

template<typename _Fnc, typename ... _T>
_Switch<_T ...> operator <= (_Switch<_T ...>&& s, _Case<_Fnc, _T ...>&& c)
{
    return std::move(s);
}


int main()
{
    std::cout << "Hello, World!" << std::endl;

    int x = 1, y = 3;
    _switch(x,y)
            <= _case(1,0)([](int, int){})
            <= _case(1,1)([](int, int){});

    return 0;
}
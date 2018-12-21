#ifndef _MULTISWITCH_HPP_
#define _MULTISWITCH_HPP_

#include <tuple>
#include <functional>

namespace multiswitch
{

struct _Placeholder {} _;

template<typename _T>
bool operator==(const _T&, const _Placeholder&)
{
  return true;
}

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

template<typename ... _T>
struct _Switch
{
    using _ValuePack = std::tuple<const _T& ...>;
    _ValuePack values;
    bool eval;
    _Switch(const _T& ... _values)
    : values(_values ...)
    , eval(true)
    {
    }

    template<typename _C>
    _Switch<_T ...>& evaluate(_C&& c)
    {
        if (eval && c(values))
        {
            eval = false;
        }
        return *this;
    }
};

template<typename ... _T>
inline constexpr
_Switch<_T...> _switch(const _T& ... values)
{
    return _Switch<_T...>(values...);
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

    template<typename ... _V>
    bool operator()(const std::tuple<const _V& ...>& values) const
    {
        if(values  == mValues) {
            mFnc();
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

template<typename _C, typename ... _T>
inline constexpr
_Switch<_T ...> operator <= (_Switch<_T ...>&& s, _C&& c)
{
    return s.evaluate(std::move(c));
}

}

#endif // _MULTISWITCH_HPP_

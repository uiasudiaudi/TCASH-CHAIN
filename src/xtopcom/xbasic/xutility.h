// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "xbase/xlog.h"
#include "xbasic/xbyte_buffer.h"

#include <string>
#include <utility>
#include <stdexcept>

NS_BEG1(tcash)

template<class...> struct conjunction : std::true_type { };
template<class B1> struct conjunction<B1> : B1 { };
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
    : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

template <typename T, typename... Ts>
typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value && conjunction<std::is_same<Ts, T>...>::value ,
                        T>::type
add(T i, T j, Ts... all) {
  T values[] = { j, all... };
  T r = i;
  for (auto v : values) {
    r += v;
    if (r < v) {
        throw std::out_of_range{"add overflow"};
    }
  }
  return r;
}



template <class T>
typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value,
                        T>::type
sub(T a, T b) {
    T x = a - b;
    if (a < b) {
        throw std::out_of_range{"sub overflow"};
    }
    return x;
}

template <class T>
typename std::enable_if<std::is_integral<T>::value && std::is_unsigned<T>::value,
                        T>::type
mul(T a, T b) {
    T x = a * b;
    if (a != 0 && x / a != b) {
        throw std::out_of_range{"mul overflow"};
    }
    return x;
}

#if !defined(XCXX14)

template<typename T, typename U>
constexpr
T const &
get(std::pair<T, U> const & pair) noexcept {
    return (std::get<0>(pair));
}

template<typename U, typename T>
constexpr
U const &
get(std::pair<T, U> const & pair) noexcept {
    return (std::get<1>(pair));
}

template<typename T, typename U>
constexpr
T &
get(std::pair<T, U> & pair) noexcept {
    return (std::get<0>(pair));
}

template<typename U, typename T>
constexpr
U &
get(std::pair<T, U> & pair) noexcept {
    return (std::get<1>(pair));
}

template<typename T, typename U>
constexpr
T &&
get(std::pair<T, U> && pair) noexcept {
    return (std::get<0>(std::move(pair)));
}

template<typename U, typename T>
constexpr
U &&
get(std::pair<T, U> && pair) noexcept {
    return (std::get<1>(std::move(pair)));
}


//template <typename T, std::size_t I, typename Head, typename ... Remains>
//constexpr
//T &
//get_helper(std::tuple<Head, Remains...> & tuple, ) noexcept {
//}
//
//template<typename T, typename ... Types>
//constexpr T &
//get(std::tuple<Types...> & t) noexcept {
//    return std::get<T>(t);
//}
//
//template< typename T, typename... Types >
//constexpr T &&
//get(tuple<Types...>&& t) noexcept {
//}
//
//template< typename T, typename... Types >
//constexpr T const & get(std::tuple<Types...> const & t) noexcept {
//}
//
//template< typename T, typename... Types >
//constexpr T const &&
//get(tuple<Types...> const && t) noexcept {
//}

// Stores a tuple of indices.  Used by tuple and pair, and by bind() to
// extract the elements in a tuple.
template <size_t... Indexes>
struct xtcash_index_tuple {
    using next = xtcash_index_tuple<Indexes..., sizeof...(Indexes)>;
};

template <size_t N>
struct xtcash_build_index_tuple {
    using type = typename xtcash_build_index_tuple<N - 1>::type::next;
};

template <>
struct xtcash_build_index_tuple<0> {
    using type =  xtcash_index_tuple<>;
};

/// Class template integer_sequence
template<typename T, T... I>
struct integer_sequence {
    using value_type = T;
    static constexpr size_t size() { return sizeof...(I); }
};

template<typename T, T N, typename ISeq = typename xtcash_build_index_tuple<N>::type>
struct xtcash_make_integer_sequence;

template<typename T, T N,  size_t... Idx>
struct xtcash_make_integer_sequence<T, N, xtcash_index_tuple<Idx...>> {
    static_assert(N >= 0, "Cannot make integer sequence of negative length" );

    using type = integer_sequence<T, static_cast<T>(Idx)...>;
};

/// Alias template make_integer_sequence
template<typename T, T N>
using make_integer_sequence = typename xtcash_make_integer_sequence<T, N>::type;

/// Alias template index_sequence
template<size_t... Idx>
using index_sequence = integer_sequence<size_t, Idx...>;

/// Alias template make_index_sequence
template<size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

/// Alias template index_sequence_for
template<typename... Types>
using index_sequence_for = make_index_sequence<sizeof...(Types)>;

#else

template <typename T, T... I>
using integer_sequence = std::integer_sequence<T, I...>;

/// Alias template make_integer_sequence
template<typename T, T N>
using make_integer_sequence = std::make_integer_sequence<T, N>;

/// Alias template index_sequence
template<size_t... Idx>
using index_sequence = std::integer_sequence<size_t, Idx...>;

/// Alias template make_index_sequence
template<size_t N>
using make_index_sequence = std::make_integer_sequence<size_t, N>;

/// Alias template index_sequence_for
template<typename... Types>
using index_sequence_for = std::make_index_sequence<sizeof...(Types)>;

template <typename T, typename U>
constexpr
T const &
get(std::pair<T, U> const & pair) noexcept {
     return std::get<T>(pair);
}

template <typename U, typename T>
constexpr
U const &
get(std::pair<T, U> const & pair) noexcept {
    return std::get<U>(pair);
}

template<typename T, typename U>
constexpr
T &
get(std::pair<T, U> & pair) noexcept {
    return std::get<T>(pair);
}

template<typename U, typename T>
constexpr
U &
get(std::pair<T, U> & pair) noexcept {
    return std::get<U>(pair);
}

template<typename T, typename U>
constexpr
T &&
get(std::pair<T, U> && pair) noexcept {
    return std::get<T>(std::move(pair));
}

template<typename U, typename T>
constexpr
U &&
get(std::pair<T, U> && pair) noexcept {
    return std::get<U>(std::move(pair));
}

#endif

#if !defined(XCXX23)

[[noreturn]] inline void unreachable() {
#    if defined(__GNUC__)  // GCC, Clang, ICC
    __builtin_unreachable();
#    elif defined(_MSC_VER)  // MSVC
    __assume(false);

#        if defined(_DEBUG)
    ::abort();
#        endif  // _DEBUG
#    endif
}

#else

[[noreturn]] inline void unreachable() {
    std::unreachable();
}

#endif

NS_END1

#if !defined NDEBUG
#   if defined XPRINT
#       error "XPRINT redefined"
#   endif

#   define XSTD_PRINT(...)          \
        std::printf(__VA_ARGS__);   \
        std::fflush(stdout)
#else
#   define XSTD_PRINT(...)
#endif

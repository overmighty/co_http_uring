// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/http_utils.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>

namespace co_http_uring::http_utils {

namespace {

using CharPredicateTable =
    std::array<bool, std::numeric_limits<unsigned char>::max()>;

constexpr std::string_view WHITESPACE_CHARS = "\t ";

consteval CharPredicateTable get_is_token_char_table() {
    CharPredicateTable table{};
    table['!'] = true;
    std::fill(table.begin() + '#', table.begin() + '\'' + 1, true);
    std::fill(table.begin() + '*', table.begin() + '+' + 1, true);
    std::fill(table.begin() + '-', table.begin() + '.' + 1, true);
    std::fill(table.begin() + '^', table.begin() + '`' + 1, true);
    table['|'] = true;
    table['~'] = true;
    std::fill(table.begin() + '0', table.begin() + '9' + 1, true);
    std::fill(table.begin() + 'A', table.begin() + 'Z' + 1, true);
    std::fill(table.begin() + 'a', table.begin() + 'z' + 1, true);
    return table;
}

consteval CharPredicateTable get_is_field_value_char_table() {
    CharPredicateTable table{};

    for (unsigned char c : WHITESPACE_CHARS) {
        table[c] = true;
    }

    // Visible characters.
    std::fill(table.begin() + '!', table.begin() + '~' + 1, true);
    // Opaque data, not ASCII.
    std::fill(table.begin() + 128, table.end(), true);
    return table;
}

constexpr CharPredicateTable IS_TOKEN_CHAR = get_is_token_char_table();
constexpr CharPredicateTable IS_FIELD_VALUE_CHAR =
    get_is_field_value_char_table();

} // namespace

bool is_token(std::string_view str) {
    return std::all_of(str.cbegin(), str.cend(), [](unsigned char c) {
        return IS_TOKEN_CHAR[c];
    });
}

bool is_digit(unsigned char c) {
    return c >= '0' && c <= '9';
}

bool is_number(std::string_view str) {
    return std::all_of(str.cbegin(), str.cend(), [](unsigned char c) {
        return is_digit(c);
    });
}

bool is_field_value(std::string_view str) {
    return std::all_of(str.cbegin(), str.cend(), [](unsigned char c) {
        return IS_FIELD_VALUE_CHAR[c];
    });
}

std::string make_header_name(std::string_view name) {
    std::string owned_name{name};
    std::transform(
        owned_name.cbegin(),
        owned_name.cend(),
        owned_name.begin(),
        [](unsigned char c) { return std::tolower(c); }
    );
    return owned_name;
}

std::string make_header_value(std::string_view value) {
    size_t non_whitespace_start = value.find_first_not_of(WHITESPACE_CHARS);

    if (non_whitespace_start != std::string_view::npos) {
        value.remove_prefix(non_whitespace_start);
        value.remove_suffix(
            value.size() - 1 - value.find_last_not_of(WHITESPACE_CHARS)
        );
    }

    return std::string{value};
}

} // namespace co_http_uring::http_utils

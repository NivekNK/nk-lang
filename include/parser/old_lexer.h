#pragma once

#include "core/arr.h"
#include "core/dyarr.h"
#include "parser/token.h"

namespace nk {
    class Lexer {
    public:
        Lexer(str source);
        ~Lexer();

        Arr<Token> scan_tokens();

        void pretty_print();

    private:
        inline bool end_of_file() { return m_current >= m_source.length(); }
        inline char advance() { return m_source[m_current++]; }
        inline char peek() { return end_of_file() ? '\0' : m_source[m_current]; }
        inline char peek_next() { return m_current + 1 >= m_source.length() ? '\0' : m_source[m_current + 1]; }
        inline bool is_digit(const char c) const { return c >= '0' && c <= '9'; }
        inline bool is_alpha(const char c) const { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
        inline bool is_alpha_numeric(const char c) const { return is_alpha(c) || is_digit(c); }

        void identifier_or_keyword();
        void identifier_or_number();
        void string();
        bool match(const char expected);
        void add_token(const TokenType type);
        void add_match_token(const char expected, const TokenType expected_type, const TokenType current_type);

        void scan_token();

        Allocator* m_allocator;
        Dyarr<Token> m_tokens;

        str m_source;
        u64 m_start = 0;
        u64 m_current = 0;
        u64 m_line = 1;
    };
}

#pragma once

#include "core/arr.h"
#include "parser/token.h"

namespace nk {
    class Allocator;

    class Lexer {
    public:
        Lexer(str& source);
        ~Lexer();

        Token next_token();

        void pretty_print();

    private:
        inline bool is_numeric(const char c) const { return c >= '0' && c <= '9'; }
        inline bool is_alpha(const char c) const { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
        inline bool is_alpha_numeric(const char c) const { return is_alpha(c) || is_numeric(c); }

        void read_character();
        bool end_of_file() const;
        bool match_character(const char expected);
        char peek_next_character();

        void skip_whitespace();
        void skip_comments();

        Token create_token(const TokenType type) const;
        Token create_match_token(const char expected, const TokenType expected_type, const TokenType current_type);
        Token create_string_token();
        Token create_id_or_number_token();
        Token create_id_or_keyword_token();

        Arr<char> m_source;
        char m_character;
        u64 m_start = 0;
        u64 m_current = 0;
        u64 m_next = 0;
        u64 m_line = 1;

        Allocator* m_allocator;
    };
}

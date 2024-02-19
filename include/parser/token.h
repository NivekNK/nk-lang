#pragma once

#include "core/enum.h"

namespace nk {
    ENUM(TokenType, (
        INVALID,

        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        LEFT_BRACKET,
        RIGHT_BRACKET,

        SEMICOLON,
        NOT,

        SLASH,
        SLASH_EQUAL,

        STAR,
        STAR_EQUAL,
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        XOR,
        XOR_EQUAL,

        COLON,
        COLON_EQUAL,
        COLON_COLON,
        AND,
        AND_AND,
        AND_EQUAL,
        OR,
        OR_OR,
        OR_EQUAL,
        LESS,
        LEFT_SHIFT,
        LESS_EQUAL,
        GREATER,
        RIGHT_SHIFT,
        GREATER_EQUAL,
        MINUS,
        MINUS_MINUS,
        MINUS_EQUAL,
        PLUS,
        PLUS_PLUS,
        PLUS_EQUAL,

        IDENTIFIER,
        STRING,
        INTEGER,
        FLOATING_POINT,

        TYPE,
        IF,
        ELSE,
        BOOL_TRUE,
        BOOL_FALSE,
        FOR,
        FUNC,
        RETURN,
        SELF,
        MUT,

        U8,
        U16,
        U32,
        U64,

        I8,
        I16,
        I32,
        I64,

        F32,
        F64,

        DIM,
        CHAR,
        STR,

        END_OF_FILE
    ));

    class Token {
    public:
        Token()
            : m_type{TokenType::INVALID},
              m_line{u64_max},
              m_start{u64_max},
              m_end{u64_max} {}

        Token(TokenType type, const u64 line, const u64 start, const u64 end)
            : m_type{type},
              m_line{line},
              m_start{start},
              m_end{end} {}

        TokenType type() const { return m_type; }

        u64 line() const { return m_line; }

        u64 start() const { return m_start; }
        u64 end() const { return m_end; }
        u64 length() const { return m_end - m_start; }

    private:
        TokenType m_type;

        u64 m_line;

        u64 m_start;
        u64 m_end;
    };
}

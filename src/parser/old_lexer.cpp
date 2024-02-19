#include "nkpch.h"

#include "parser/lexer.h"

#include "memory/malloc_allocator.h"
#include "parser/report.h"
#include "parser/token_keyword_map.h"

namespace nk {
    Lexer::Lexer(str source)
        : m_source{source},
          m_allocator{new MallocAllocator()},
          m_tokens{m_allocator, 100} {
    }

    Lexer::~Lexer() {
        m_tokens.free();
        delete m_allocator;
    }

    Arr<Token> Lexer::scan_tokens() {
        while (!end_of_file()) {
            m_start = m_current;
            scan_token();
        }

        m_tokens.push({TokenType::END_OF_FILE, m_line, m_source.length(), m_source.length()});
        return m_tokens;
    }

    void Lexer::pretty_print() {
        auto tokens = scan_tokens();
        for (Token token : tokens) {
            DebugLog("Type: {} | Line: {} | Lexeme: {}",
                TokenTypeImpl::string(token.type()), token.line(), m_source.substr(token.start(), token.end() - token.start()));
        }
    }

    void Lexer::identifier_or_keyword() {
        while (is_alpha_numeric(peek()))
            advance();

        str value = m_source.substr(m_start, m_current - m_start);
        if (auto keyword = TokenKeywordMap::is_valid_keyword(value.c_str(), value.length())) {
            add_token(keyword->type);
            return;
        }

        add_token(TokenType::IDENTIFIER);
    }

    void Lexer::identifier_or_number() {
        while (is_digit(peek()))
            advance();

        if (is_alpha(peek())) {
            identifier_or_keyword();
            return;
        }

        if (peek() == '.' && is_digit(peek_next())) {
            advance();

            while(is_digit(peek()))
                advance();

            add_token(TokenType::FLOATING_POINT);
            return;
        }

        add_token(TokenType::INTEGER);
    }

    void Lexer::string() {
        while (peek() != '"' && !end_of_file()) {
            if (peek() == '\n')
                m_line++;
            advance();
        }

        if (end_of_file()) {
            // TODO: Error unterminated string.
            return;
        }

        advance();
        m_tokens.push({TokenType::STRING, m_line, m_start + 1, m_current - 1});
    }

    bool Lexer::match(const char expected) {
        if (end_of_file() || m_source[m_current] != expected)
            return false;

        m_current++;
        return true;
    }

    void Lexer::add_token(const TokenType type) {
        m_tokens.push({type, m_line, m_start, m_current});
    }

    void Lexer::add_match_token(const char expected, const TokenType expected_type, const TokenType current_type) {
        if (match(expected)) {
            add_token(expected_type);
            return;
        }
        add_token(current_type);
    }

    void Lexer::scan_token() {
        char c = advance();
        switch (c) {
            case '(': add_token(TokenType::LEFT_PAREN); break;
            case ')': add_token(TokenType::RIGHT_PAREN); break;
            case '{': add_token(TokenType::LEFT_BRACE); break;
            case '}': add_token(TokenType::RIGHT_BRACE); break;
            case '[': add_token(TokenType::LEFT_BRACKET); break;
            case ']': add_token(TokenType::RIGHT_BRACKET); break;
            case ';': add_token(TokenType::SEMICOLON); break;
            case '~': add_token(TokenType::NOT); break;
            case '*':
                add_match_token('=', TokenType::STAR_EQUAL, TokenType::STAR);
                break;
            case '!':
                add_match_token('=', TokenType::BANG_EQUAL, TokenType::BANG);
                break;
            case '=':
                add_match_token('=', TokenType::EQUAL_EQUAL, TokenType::EQUAL);
                break;
            case '^':
                add_match_token('=', TokenType::XOR_EQUAL, TokenType::XOR);
                break;
            case ':':
                if (match(':')) {
                    add_token(TokenType::COLON_COLON);
                } else {
                    add_match_token('=', TokenType::COLON_EQUAL, TokenType::COLON);
                }
                break;
            case '&':
                if (match('&')) {
                    add_token(TokenType::AND_AND);
                } else {
                    add_match_token('=', TokenType::AND_EQUAL, TokenType::AND);
                }
                break;
            case '|':
                if (match('|')) {
                    add_token(TokenType::OR_OR);
                } else {
                    add_match_token('=', TokenType::OR_EQUAL, TokenType::OR);
                }
                break;
            case '<':
                if (match('<')) {
                    add_token(TokenType::LEFT_SHIFT);
                } else {
                    add_match_token('=', TokenType::LESS_EQUAL, TokenType::LESS);
                }
                break;
            case '>':
                if (match('>')) {
                    add_token(TokenType::RIGHT_SHIFT);
                } else {
                    add_match_token('=', TokenType::GREATER_EQUAL, TokenType::GREATER);
                }
                break;
            case '-':
                if (match('-')) {
                    add_token(TokenType::MINUS_MINUS);
                } else {
                    add_match_token('=', TokenType::MINUS_EQUAL, TokenType::MINUS);
                }
                break;
            case '+':
                if (match('+')) {
                    add_token(TokenType::PLUS_PLUS);
                } else {
                    add_match_token('=', TokenType::PLUS_EQUAL, TokenType::PLUS);
                }
                break;
            case '/':
                if (match('/')) {
                    while(peek() != '\n' && !end_of_file())
                        advance();
                } else {
                    add_match_token('=', TokenType::SLASH_EQUAL, TokenType::SLASH);
                }
                break;

            case ' ':
            case '\r':
            case '\t':
                break;

            case '\n':
                m_line++;
                break;

            case '"': string(); break;

            default:
                if (is_digit(c)) {
                    identifier_or_number();
                } else if (is_alpha(c)) {
                    identifier_or_keyword();
                } else {
                    // TODO: Error Unexpected character
                }
                break;
        }
    }
}

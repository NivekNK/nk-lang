#include "nkpch.h"

#include "parser/lexer.h"

#include "memory/malloc_allocator.h"
#include "parser/token_keyword_map.h"

namespace nk {
    Lexer::Lexer(str& source)
        : m_source{std::move(source).data(), source.length(), true},
          m_allocator{new MallocAllocator()} {
        read_character();
    }

    Lexer::~Lexer() {
        delete m_allocator;
    }

    void Lexer::pretty_print() {
        TokenType current = TokenType::INVALID;
        DebugLog("Here");
        while (current != TokenType::END_OF_FILE) {
            DebugLog("Here");
            auto token = next_token();
            DebugLog("Here");
            vstr lexeme{m_source.data() + token.start(), token.end() - token.start()};
            DebugLog("Type: {} | Line: {} | Lexeme: {}",
                TokenTypeImpl::string(token.type()), token.line(), lexeme);
            DebugLog("Here");
            current = token.type();
            break;
        }
    }

    Token Lexer::next_token() {
        Token token;

        //skip_whitespace();
        //skip_comments();

        m_start = m_current;

        switch (m_character) {
            case 0:
                return create_token(TokenType::END_OF_FILE);

            case '(':
                token = create_token(TokenType::LEFT_PAREN);
                break;
            case ')':
                token = create_token(TokenType::RIGHT_PAREN);
                break;
            case '{':
                token = create_token(TokenType::LEFT_BRACE);
                break;
            case '}':
                token = create_token(TokenType::RIGHT_BRACE);
                break;
            case '[':
                token = create_token(TokenType::LEFT_BRACKET);
                break;
            case ']':
                token = create_token(TokenType::RIGHT_BRACKET);
                break;
            case ';':
                token = create_token(TokenType::SEMICOLON);
                break;
            case '~':
                token = create_token(TokenType::NOT);
                break;
            case '/':
                token = create_match_token('=', TokenType::SLASH_EQUAL, TokenType::SLASH);
                break;
            case '*':
                token = create_match_token('=', TokenType::STAR_EQUAL, TokenType::STAR);
                break;
            case '!':
                token = create_match_token('=', TokenType::BANG_EQUAL, TokenType::BANG);
                break;
            case '=':
                token = create_match_token('=', TokenType::EQUAL_EQUAL, TokenType::EQUAL);
                break;
            case '^':
                token = create_match_token('=', TokenType::XOR_EQUAL, TokenType::XOR);
                break;
            case ':':
                if (match_character(':')) {
                    token = create_token(TokenType::COLON_COLON);
                } else {
                    token = create_match_token('=', TokenType::COLON_EQUAL, TokenType::COLON);
                }
                break;
            case '&':
                if (match_character('&')) {
                    token = create_token(TokenType::AND_AND);
                } else {
                    token = create_match_token('=', TokenType::AND_EQUAL, TokenType::AND);
                }
                break;
            case '|':
                if (match_character('|')) {
                    token = create_token(TokenType::OR_OR);
                } else {
                    token = create_match_token('=', TokenType::OR_EQUAL, TokenType::OR);
                }
                break;
            case '<':
                if (match_character('<')) {
                    token = create_token(TokenType::LEFT_SHIFT);
                } else {
                    token = create_match_token('=', TokenType::LESS_EQUAL, TokenType::LESS);
                }
                break;
            case '>':
                if (match_character('>')) {
                    token = create_token(TokenType::RIGHT_SHIFT);
                } else {
                    token = create_match_token('=', TokenType::GREATER_EQUAL, TokenType::GREATER);
                }
                break;
            case '-':
                if (match_character('-')) {
                    token = create_token(TokenType::MINUS_MINUS);
                } else {
                    token = create_match_token('=', TokenType::MINUS_EQUAL, TokenType::MINUS);
                }
                break;
            case '+':
                if (match_character('+')) {
                    token = create_token(TokenType::PLUS_PLUS);
                } else {
                    token = create_match_token('=', TokenType::PLUS_EQUAL, TokenType::PLUS);
                }
                break;
            case '"':
                token = create_string_token();
                break;
            default:
                if (is_numeric(m_character)) {
                    token = create_id_or_number_token();
                } else if (is_alpha(m_character)) {
                    token = create_id_or_keyword_token();
                }
                break;
        }

        read_character();
        return token;
    }

    void Lexer::read_character() {
        if (m_next >= m_source.length()) {
            m_character = 0;
            m_current = m_source.length();
            return;
        }

        m_character = m_source[m_next];
        m_current = m_next;
        m_next++;
    }

    bool Lexer::end_of_file() const {
        return m_current >= m_source.length();
    }

    bool Lexer::match_character(const char expected) {
        if (end_of_file() && expected != 0)
            return false;

        if (m_source[m_next] != expected)
            return false;

        read_character();
        return true;
    }

    char Lexer::peek_next_character() {
        if (m_next >= m_source.length())
            return 0;

        return m_source[m_next];
    }

    void Lexer::skip_whitespace() {
        while (m_character == ' ' || m_character == '\t' || m_character == '\r' || m_character == '\n') {
            if (m_character == '\n')
                m_line++;
            read_character();
        }
    }

    void Lexer::skip_comments() {
        while (m_character == '/' && match_character('/')) {
            while(m_character != '\n' && !end_of_file())
                read_character();

            if (!end_of_file()) {
                read_character();
                m_line++;
            }
        }
    }

    Token Lexer::create_token(const TokenType type) const {
        return {type, m_line, m_start, m_next};
    }

    Token Lexer::create_match_token(const char expected, const TokenType expected_type, const TokenType current_type) {
        if (match_character(expected))
            return create_token(expected_type);

        return create_token(current_type);
    }

    Token Lexer::create_string_token() {
        while (m_current != '"' && !end_of_file()) {
            if (m_current == '\n')
                m_line++;
            read_character();
        }

        if (end_of_file()) {
            // TODO: Error unterminated string.
            return {};
        }

        return {TokenType::STRING, m_line, m_start + 1, m_current};
    }

    Token Lexer::create_id_or_number_token() {
        while (is_numeric(m_current))
            read_character();

        if (is_alpha(m_current)) {
            return create_id_or_keyword_token();
        }

        if (m_current == '.' && is_numeric(peek_next_character())) {
            read_character();

            while(is_numeric(m_current))
                read_character();

            return create_token(TokenType::FLOATING_POINT);
        }

        return create_token(TokenType::INTEGER);
    }

    Token Lexer::create_id_or_keyword_token() {
        while (is_alpha_numeric(m_current))
            read_character();

        const u64 length = m_current - m_start;
        char* buffer = m_allocator->allocate<char>(length);
        std::memcpy(buffer, &m_source.data()[m_start], length - 1);
        buffer[length - 1] = '\0';

        if (auto keyword = TokenKeywordMap::is_valid_keyword(buffer, length)) {
            m_allocator->free<char>(buffer, length);
            return create_token(keyword->type);
        }

        m_allocator->free<char>(buffer, length);
        return create_token(TokenType::IDENTIFIER);
    }
}

#pragma once

namespace nk {
    template <typename D, typename T>
    concept IDyarr = requires(D& dyarr) {
        { dyarr.data() } -> std::same_as<T*>;
        { dyarr.length() } -> std::same_as<u64>;
    };

    template <typename T>
    class Arr {
    public:
        Arr(T* data, const u64 length, bool own_data)
            : m_data{data}, m_length{length}, m_own_data{own_data} {
        }

        template <IDyarr<T> D>
        Arr(D& dyarr)
            : m_data{dyarr.data()}, m_length{dyarr.length()} {
        }

        ~Arr() {
            if (m_own_data && m_data != nullptr)
                free();
        }

        T& operator[](const u64 index) {
            ErrorIfLog(index >= m_length, "At index: {} with length: {}!", index, m_length);
            return m_data[index];
        }

        const T& operator[](const u64 index) const {
            ErrorIfLog(index >= m_length, "At index: {} with length: {}!", index, m_length);
            return m_data[index];
        }

        T* data() { return m_data; }
        u64 length() const { return m_length; }

        void free() { std::free(m_data); }

        struct Iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;

            Iterator(pointer ptr) : m_ptr{ptr} {}

            reference operator*() const { return *m_ptr; }
            pointer operator->() { return m_ptr; }

            Iterator& operator++() {
                m_ptr++;
                return *this;
            }

            Iterator operator++(int) {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            friend bool operator==(const Iterator& a, const Iterator& b) {
                return a.m_ptr == b.m_ptr;
            }

            friend bool operator!=(const Iterator& a, const Iterator& b) {
                return a.m_ptr != b.m_ptr;
            }

        private:
            pointer m_ptr;
        };

        Iterator begin() { return Iterator{&m_data[0]}; }
        Iterator end() { return Iterator{&m_data[m_length]}; }

    private:
        T* m_data = nullptr;
        u64 m_length = 0;
        bool m_own_data = false;
    };
}

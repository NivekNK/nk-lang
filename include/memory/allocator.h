#pragma once

namespace nk {
    class Allocator {
    public:
        Allocator(const u64 size_bytes, void* const start);

        Allocator(const Allocator&) = delete;
        Allocator& operator=(Allocator&) = delete;

        Allocator(Allocator&&);
        Allocator& operator=(Allocator&&);

        virtual ~Allocator();

        template <typename T>
        inline T* allocate() {
            return static_cast<T*>(allocate(sizeof(T), alignof(T)));
        }

        template <typename T>
        inline T* allocate_lot(const u64 lot) {
            return static_cast<T*>(allocate(sizeof(T) * lot, alignof(T)));
        }

        template <typename T>
        inline void free(T* ptr) {
            free(ptr, sizeof(T));
        }

        template <typename T>
        inline void free_lot(T* ptr, const u64 lot) {
            free(ptr, lot * sizeof(T));
        }

        template <typename T, typename... Args>
        inline T* construct(Args&&... args) {
            return new (allocate(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
        }

        template <typename T>
        inline void destroy(T* ptr) {
            if (ptr == nullptr) {
                WarnLog("Trying to destroy nullptr!");
                return;
            }

            ptr->~T();
            free(ptr, sizeof(T));
        }

        inline u64 size() const { return m_size_bytes; }
        inline u64 used() const { return m_used_bytes; }
        inline u64 allocation_count() const { return m_allocation_count; }
        inline const void* start() const { return m_start; }

        virtual void* allocate(const u64 size_bytes, [[maybe_unused]] const u64 alignment) = 0;
        virtual void free(void* const ptr, [[maybe_unused]] const u64 size_bytes) = 0;

    protected:
        u64 m_size_bytes;
        u64 m_used_bytes;
        u64 m_allocation_count;

        void* m_start;
    };
}

#pragma once
#include <cassert>

template <class T>
class Span {
    T *m_data{nullptr};
    size_t m_size{0};

public:
    Span() = default;
    explicit Span(T *data, size_t size) : m_data(data), m_size(size) {}

    inline T &operator[](size_t id) {
        assert(m_data);
        assert(id < m_size);
        return m_data[id];
    }

    inline Span operator+(size_t offset) {
        assert(m_data);
        assert(offset < m_size);
        return Span(m_data + offset, m_size - offset);
    }

    inline const T *Get() const { return m_data; }
    inline size_t Size() const { return m_size; }
    // @NOTE(PKiyashko): I believe this is unconventional, but why use an iterator when a pointer is fine?
    inline T *Begin() { return m_data; }
    inline T *End() { return m_data + m_size; }

    // For range-based loops. 
    inline T *begin() { return Begin(); }
    inline T *end() { return End(); }
};

template <class T>
inline Span<T> make_span(T *data, size_t size)
{
    return Span<T>(data, size);
}

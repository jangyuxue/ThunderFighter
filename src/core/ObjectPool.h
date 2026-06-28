#pragma once

#include <cstddef>
#include <cstdint>

// ============================================================
// 模板对象池 — 用于子弹和粒子的高效复用
// 固定大小数组，无动态内存分配
// ============================================================
template <typename T, size_t MaxSize>
class ObjectPool {
public:
    ObjectPool() = default;

    // 获取一个未使用的对象，返回 nullptr 表示池已满
    T* Acquire() {
        for (size_t i = 0; i < MaxSize; ++i) {
            if (!m_active[i]) {
                m_active[i] = true;
                ++m_activeCount;
                return &m_objects[i];
            }
        }
        return nullptr;
    }

    // 归还对象到池中
    void Release(T* obj) {
        size_t index = obj - m_objects;
        if (index < MaxSize && m_active[index]) {
            m_active[index] = false;
            --m_activeCount;
        }
    }

    // 释放所有对象
    void ReleaseAll() {
        for (size_t i = 0; i < MaxSize; ++i) {
            m_active[i] = false;
        }
        m_activeCount = 0;
    }

    // 获取活跃数量
    size_t ActiveCount() const { return m_activeCount; }

    // 获取总容量
    constexpr size_t Capacity() const { return MaxSize; }

    // 访问原始数组（用于初始化/析构）
    T* Data() { return m_objects; }
    const T* Data() const { return m_objects; }

    // 判断指定索引是否活跃
    bool IsActive(size_t index) const {
        return index < MaxSize && m_active[index];
    }

    // 获取指定索引的对象
    T* At(size_t index) {
        return (index < MaxSize) ? &m_objects[index] : nullptr;
    }

    // ---- 迭代器支持 ----

    class Iterator {
    public:
        Iterator(T* obj, const bool* active, size_t index, size_t max)
            : m_obj(obj), m_active(active), m_index(index), m_max(max) {
            AdvanceToNext();
        }

        T& operator*()  { return *m_obj; }
        T* operator->() { return m_obj; }

        Iterator& operator++() {
            ++m_index;
            ++m_obj;
            AdvanceToNext();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return m_index != other.m_index;
        }

    private:
        void AdvanceToNext() {
            while (m_index < m_max && !m_active[m_index]) {
                ++m_index;
                ++m_obj;
            }
        }
        T* m_obj;
        const bool* m_active;
        size_t m_index;
        size_t m_max;
    };

    Iterator begin() {
        return Iterator(m_objects, m_active, 0, MaxSize);
    }

    Iterator end() {
        return Iterator(m_objects + MaxSize, m_active, MaxSize, MaxSize);
    }

private:
    T      m_objects[MaxSize];
    bool   m_active[MaxSize] = {};
    size_t m_activeCount     = 0;
};

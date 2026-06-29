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
        // 从上次释放位置开始搜索，减少扫描开销
        for (size_t j = 0; j < MaxSize; ++j) {
            size_t i = (m_nextFree + j) % MaxSize;
            // 槽位空闲，或槽位虽被占用但对象已逻辑停用(Deactivate 但未 Release)，
            // 均可复用。
            // 修复：原版仅检查 m_active[i]，而游戏中子弹/粒子出屏只调
            // Entity::Deactivate() (置实体 m_active=false) 从不调 pool.Release()，
            // 导致槽位标记永不归零、池逐渐占满、Acquire 最终返回 nullptr、
            // 射击/特效渐进式静默失效。
            if (!m_active[i] || !m_objects[i].IsActive()) {
                if (m_active[i] && !m_objects[i].IsActive()) {
                    // 复用"占用但已停用"的槽位：先逻辑释放，保持计数自洽
                    m_active[i] = false;
                    --m_activeCount;
                }
                m_active[i] = true;
                ++m_activeCount;
                m_nextFree = (i + 1) % MaxSize;
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
            m_nextFree = index;  // 下次从释放位置开始搜索
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
    size_t m_nextFree        = 0;   // 搜索优化
};

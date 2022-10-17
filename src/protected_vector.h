#pragma once

#include <memory>
#include <shared_mutex>
#include <mutex>

namespace kms 
{

template<typename T>
class protected_vector_t
{
    using write_lock = std::unique_lock<std::shared_mutex>;
    using read_lock = std::shared_lock<std::shared_mutex>;

public:
    void add(std::unique_ptr<T>&& item)
    {
        write_lock wl(m_Mutex);
        m_Data.push_back(std::move(item));
    }

    auto remove(T* item)
    {
        write_lock wl(m_Mutex);
        std::erase_if(m_Data, [&item](auto& itemData) -> auto {
            return item == itemData.get();
        });
    }

    std::vector<T*> get(std::vector<T*>& items) const
    {
        read_lock rl(m_Mutex);
        items.reserve(m_Data.size());
        for(auto& item : m_Data)
        {
            items.push_back(item.get());
        }
        return items;
   }

private:
    std::vector<std::unique_ptr<T>> m_Data;
    mutable std::shared_mutex m_Mutex;
};


template<typename T>
class base_protected_vector_t
{
    using write_lock = std::unique_lock<std::shared_mutex>;
    using read_lock = std::shared_lock<std::shared_mutex>;
public:

    using READ_ONLY_VECTOR = std::vector<const T*>;


    void add(T&& item)
    {
        write_lock wl(m_Mutex);
        m_Data.push_back(std::move(item));
    }

    auto remove(const T& item)
    {
        write_lock wl(m_Mutex);
        std::erase_if(m_Data, [&item](auto& itemData) -> auto {
            return item == itemData;
        });
    }

    READ_ONLY_VECTOR& get(READ_ONLY_VECTOR& items) const
    {
        read_lock rl(m_Mutex);
        items.reserve(m_Data.size());
        for(auto& item : m_Data)
        {
            items.push_back(&item);
        }
        return items;
   }

private:
    std::vector<T> m_Data;
    mutable std::shared_mutex m_Mutex;
};



}
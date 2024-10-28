#ifndef Stream_hpp
#define Stream_hpp

#include <vector>
#include <functional>
#include <iostream>
#include <numeric>
#include <atomic>
#include <memory>
#include <unordered_map>
#include <mutex>

template<typename T>
class Stream;

template<typename T, typename P>
class LazyStream
{
public:

    std::function<T(const P &)> func_to_map;
    std::function<bool(const P&)> func_to_filter;
    const std::shared_ptr<const std::vector<P>> data;


    LazyStream(const std::shared_ptr<const std::vector<P>>& original_data, std::function<T(const P&)> map, std::function<bool(const P&)> filter)
        : data(original_data), func_to_map(map), func_to_filter(filter)
    {
    }

    template<typename M>
    LazyStream<M, P> map(std::function<M(const T &)> map_func) const
    {
        auto this_map = func_to_map;
        auto this_filter = func_to_filter;
        return LazyStream<M, P>(data, 
            [=](const P & p) -> M
            {
                return map_func(this_map(p));
            },
            [=](const P & p) -> bool
            {
                return this_filter(p);
            }
        );
    }
    
    LazyStream<T, P> filter(std::function<bool(const T &)> filter_func) const
    {
        auto this_map = func_to_map;
        auto this_filter = func_to_filter;
        return LazyStream<T, P>(data, 
            [=](const P & p) -> T
            {
                return this_map(p);
            },
            [=](const P & p) -> bool
            {
                return this_filter(p) && filter_func(this_map(p));
            }
        );
    }    

    Stream<T> to_Stream() const
    {
        auto & datap = *data;
        auto ret = std::vector<T>(datap.size());
        std::atomic_int len(0);
        #pragma omp parallel for
        for(size_t i=0;i<datap.size();i++)
        {
            auto valid = func_to_filter(datap[i]);
            if(valid)
            {
                auto fd = func_to_map(datap[i]);
                ret[len++] = fd;
            }
        }
        ret.erase(ret.begin()+len, ret.end());
        return Stream<T>(ret);
    }

    template<typename K, typename V>
    std::unordered_map<K, Stream<V>> group_by_key(std::function<K(const T&)> get_key, std::function<V(const T&)> get_value) const
    {
        Stream<T> fdata = to_Stream();
        return fdata.group_by_key(get_key, get_value);
    }    

    T reduce(T init, std::function<T(const T&, const T&)> combination_func)  const
    {
        Stream<T> fdata = to_Stream();
        return fdata.reduce(init, combination_func);
    }    

    std::vector<T> collect() const
    {
        Stream<T> fdata = to_Stream();
        return fdata.collect();
    }
    
};

template<typename T>
class Stream
{
public:

    const std::shared_ptr<const std::vector<T>> data;

    Stream(const std::vector<T>& original_data)
        : data(std::make_shared<const std::vector<T>>(original_data))
    {
    }

    Stream(const std::shared_ptr<const std::vector<T>>& original_data)
        : data(original_data)
    {
    }

    template<typename M>
    LazyStream<M, T> map(std::function<M(const T &)> map_func) const
    {
        return LazyStream<M, T>(data, 
            [=](const T & t) -> M
            {
                return map_func(t);
            },
            [=](const T & t) -> bool
            {
                return true;
            }
        );
    }
    
    LazyStream<T, T> filter(std::function<bool(const T &)> filter_func) const
    {
        return LazyStream<T, T>(data, 
            [=](const T & t) -> T
            {
                return t;
            },
            [=](const T & t) -> bool
            {
                return filter_func(t);
            }
        );
    }    

    template<typename K, typename V>
    std::unordered_map<K, Stream<V>> group_by_key(std::function<K(const T&)> get_key, std::function<V(const T&)> get_value) const
    {
        const int SLOTS = 128;
        std::unordered_map<K, std::vector<V>> res[SLOTS];
        std::mutex locks[SLOTS];
        std::unordered_map<K, Stream<V>> res_map;

        auto & datap = *data;

        #pragma omp parallel for
        for(size_t i=0;i<datap.size();i++)
        {
            auto key = get_key(datap[i]);
            auto value = get_value(datap[i]);
            uint64_t hash = std::hash<typename std::remove_reference<decltype(key)>::type>()(key);
            auto slot = hash%SLOTS;
            std::lock_guard<std::mutex> lock(locks[slot]);
            auto iter = res[slot].find(key);
            if(iter == res[slot].end()) iter = res[slot].emplace_hint(iter, std::make_pair(key, std::vector<V>()));
            iter->second.emplace_back(value);
        }

        for(int i=0;i<SLOTS;i++)
        {
            for(auto &v: res[i])
            {
                res_map.emplace(v.first, Stream<V>(v.second));
            }
        }
        return res_map;
    }    

    T reduce(T init, std::function<T(const T&, const T&)> combination_func) const
    {
        for(auto &d : *data)
        {
            init = combination_func(init, d);
        }
        return init;
    }    

    std::vector<T> collect() const
    {
        return *data;
    }
    
};

#endif /* Stream_hpp */

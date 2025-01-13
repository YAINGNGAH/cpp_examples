#pragma once

#include <constexpr_map.h>

#include <type_traits>

template <class K, class V, int S>
constexpr auto Sort(ConstexprMap<K, V, S> map) {
    ConstexprMap<K, V, S> newmap;
    for (int i = 0; i < map.Size(); ++i) {
        newmap[map.GetByIndex(i).first] = map.GetByIndex(i).second;
    }
    if constexpr (std::is_integral_v<K>) {
        for (int i = 0; i < map.Size(); ++i) {
            for (int j = 1; j < map.Size(); ++j) {
                if (newmap.GetByIndex(j - 1) < newmap.GetByIndex(j)) {
                    std::pair<K, V> tmp = newmap.GetByIndex(j - 1);
                    newmap.GetByIndex(j - 1) = newmap.GetByIndex(j);
                    newmap.GetByIndex(j) = tmp;
                }
            }
        }
    } else {
        for (int i = 0; i < map.Size(); ++i) {
            for (int j = 1; j < map.Size(); ++j) {
                if (newmap.GetByIndex(j) < newmap.GetByIndex(j - 1)) {
                    std::pair<K, V> tmp = newmap.GetByIndex(j - 1);
                    newmap.GetByIndex(j - 1) = newmap.GetByIndex(j);
                    newmap.GetByIndex(j) = tmp;
                }
            }
        }
    }
    return newmap;
}

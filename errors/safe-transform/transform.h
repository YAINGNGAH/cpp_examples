#pragma once
#include <vector>
#include <type_traits>
#include <stddef.h>

template <typename Iterator, typename Predicate, typename Function>
void TransformIf(Iterator begin, Iterator end, Predicate p, Function f) {
    using T = std::remove_reference_t<decltype(*begin)>;
    bool edup = false;
    Iterator it = begin;
    size_t size = 0;
    std::vector<T*> log;
    std::vector<Iterator> iters;
    it = begin;
    while (it != end) {
        bool check = false;
        try {
            check = p(*it);
        } catch (...) {
            if (edup) {
                for (size_t i = 0; i < log.size(); ++i) {
                    delete log[i];
                }
                throw;
            }
            for (size_t i = 0; i < log.size(); ++i) {
                try {
                    *(iters[i]) = *log[i];
                } catch (...) {
                }
            }
            for (size_t i = 0; i < log.size(); ++i) {
                delete log[i];
            }
            throw;
        }
        if (check) {
            try {
                log.push_back(new T(*it));
                iters.push_back(it);
            } catch (...) {
                edup = true;
            }
            try {
                f(*it);
            } catch (...) {
                if (edup) {
                    for (size_t i = 0; i < log.size(); ++i) {
                        delete log[i];
                    }
                    throw;
                }
                for (size_t i = 0; i < log.size(); ++i) {
                    try {
                        *(iters[i]) = *log[i];
                    } catch (...) {
                    }
                }
                for (size_t i = 0; i < log.size(); ++i) {
                    delete log[i];
                }
                throw;
            }
        }
        ++it;
    }
    for (size_t i = 0; i < log.size(); ++i) {
        delete log[i];
    }
}
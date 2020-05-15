//
// Copyright (c) 2020 Masashi Fujita. All rights reserved.
//
#pragma once

#include <cstdint>
#include <algorithm>

/// @brief Destructively bipartite the sequence.
/// @tparam Iter_ The iterator type. Should satisfy the LegacyRandomAccessIterator.
/// @tparam F_ The predicate type
/// @param b Start of the sequence
/// @param e End of the sequence
/// @param pred The predicate
/// @return An iterator pointing the start of the sequence which make the `pred ()` true.
/// @example
/// pred  : is_even (x) { return x % 2 == 0 ; }
/// input : [0 1 2 3 4 5 6 7 8]
/// result: [1 3 5 7  0 2 4 6 8]
///                   ^-- returned iterator points here

template <typename Iter_, typename F_>
    Iter_ bipartite (Iter_ b, Iter_ e, F_ &&pred) {
        if (b == e) {
            return e;
        }
        auto it = b;
        size_t cnt = 0;

        while (it != (e - cnt)) {
            auto const &v = *it;
            if (pred (v)) {
                std::rotate (it, it + 1, e);
                ++cnt;
                continue;
            }
            ++it;
        }
        return e - cnt;
    }

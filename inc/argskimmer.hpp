
#pragma once

#include <cstdint>
#include <algorithm>

/// @brief Bipartite the sequence.
/// @tparam Iter_ The iterator type. Should satisfy the LegacyRandomAccessIterator.
/// @tparam F_ The predicate type
/// @param b Start of the sequence
/// @param e End of the sequence
/// @param pred The predicate
/// @return An iterator pointing the start of the sequence which make the `pred ()` true.
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

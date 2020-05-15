
#pragma once

#include <cstdint>
#include <algorithm>

/// @brief Move arguments which makes `pred (a)` is true toward the end.
/// @tparam F_
/// @param argc
/// @param argv
/// @param pred
/// @return
template <typename F_>
int skimArguments (int argc, const char **argv, F_ &&pred) {
    if (argc <= 0) {
        return argc;
    }
    int i = 0;
    int cnt = 0;    // # of skimmed
    while (i < (argc - cnt)) {
        auto ap = argv[i];
        if (pred (ap)) {
            // moves argv [i + 1...argc - 1]
            memmove (&argv[i], &argv[i + 1], (argc - i - 1) * sizeof (argv[0]));
            argv[argc - 1] = ap;
            ++cnt;
            continue;
        }
        ++i;
    }
    return argc - cnt;
}

template <typename Iter_, typename F_>
    Iter_ split (Iter_ b, Iter_ e, F_ &&pred) {
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

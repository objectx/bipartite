
#include "doctest-rapidcheck.hpp"
#include <argskimmer.hpp>

#include <doctest/doctest.h>

#include <array>
#include <cstring>
#include <map>
#include <ostream>
#include <vector>

namespace {
    bool is_doctest_argument (const char *s) {
        return strncmp (s, "--dt-", 5) == 0;
    }
}// namespace
TEST_CASE ("Examples"
           * doctest::test_suite ("example")) {
    SUBCASE ("no doctest args") {
        std::array<const char *, 8> src = {"a", "b", "c", "d", "e", "f", "g", "h"};
        std::array<const char *, 8> dst {src};
        auto start = bipartite (dst.begin (), dst.end (), is_doctest_argument);
        CHECK_EQ (start, dst.end ());
        CHECK_EQ (dst[0], src[0]);
        CHECK_EQ (dst[1], src[1]);
        CHECK_EQ (dst[2], src[2]);
        CHECK_EQ (dst[3], src[3]);

        CHECK_EQ (dst[4], src[4]);
        CHECK_EQ (dst[5], src[5]);
        CHECK_EQ (dst[6], src[6]);
        CHECK_EQ (dst[7], src[7]);
    }
    SUBCASE ("non changing") {
        std::array<const char *, 8> src = {"a", "b", "c", "d", "--dt-e", "--dt-f", "-g", "h"};
        std::array<const char *, 8> dst {src};
        auto start = bipartite (dst.begin (), dst.end (), is_doctest_argument);
        CHECK_EQ (start, dst.begin() + 6);
        CHECK_EQ (dst[0], src[0]);
        CHECK_EQ (dst[1], src[1]);
        CHECK_EQ (dst[2], src[2]);
        CHECK_EQ (dst[3], src[3]);

        CHECK_EQ (dst[4], src[6]);
        CHECK_EQ (dst[5], src[7]);
        CHECK_EQ (dst[6], src[4]);
        CHECK_EQ (dst[7], src[5]);
    }
    SUBCASE ("Interleaved") {
        std::array<const char *, 8> src = {"a", "--dt-b", "c", "--dt-d", "e", "--dt-f", "g", "--dt-h"};
        // std::array<const char *, 8> src = {"a", "c", "e", "g", "--dt-b", "--dt-d", "--dt-f", "--dt-h"};
        std::array<const char *, 8> dst {src};
        auto start = bipartite (dst.begin (), dst.end (), is_doctest_argument);
        CHECK_EQ (start, dst.begin () + 4);
        CHECK_EQ (dst[0], src[0]);
        CHECK_EQ (dst[1], src[2]);
        CHECK_EQ (dst[2], src[4]);
        CHECK_EQ (dst[3], src[6]);

        CHECK_EQ (dst[4], src[1]);
        CHECK_EQ (dst[5], src[3]);
        CHECK_EQ (dst[6], src[5]);
        CHECK_EQ (dst[7], src[7]);
    }
}

namespace {
    bool is_special_option (const char *s) {
        return strncmp (s, "--dt-", 5) == 0;
    }

    rc::Gen<std::string> genSpecial () {
        static const std::string empty {};
        static const std::string special {"--dt-"};
        return rc::gen::element (empty, special);
    }

    rc::Gen<std::string> genOpt () {
        static const std::string chars {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_."};
        return rc::gen::container<std::string> (rc::gen::elementOf (chars));
    }
    rc::Gen<std::string> genOption () {
        return rc::gen::apply ([] (const std::string &pfx, const std::string &opt) {
            return pfx + opt;
        }, genSpecial (), genOpt ());
    }

    template <typename Iter_>
    void dump (std::ostream &os, Iter_ b, Iter_ e) {
        if (b == e) {
            return;
        }
        auto it = b;
        os << *it++;
        while (it != e) {
            os << " " << *it++;
        }
    }
}// namespace

TEST_CASE ("Test bipartite property"
           * doctest::test_suite ("property")) {
    rc::prop ("property", [] {
        auto const src = *rc::gen::unique<std::vector<std::string>> (genOption ());
        std::map<std::string, size_t> dict;
        std::vector<std::string> opts;
        opts.reserve (src.size ());
        for (auto const &s : src) {
            dict.try_emplace (s, opts.size ());
            opts.emplace_back (s);
        }
        auto is_spl = [] (auto const &s) -> bool { return is_special_option (s.c_str ()); };
        auto start  = bipartite (opts.begin (), opts.end (), is_spl);
        auto cntSpl = std::count_if (src.begin (), src.end (), is_spl);
        // dump (std::cout, opts.cbegin (), opts.cend ());
        // Total # of options should be same.
        RC_ASSERT (std::distance (start, opts.end ()) == cntSpl);

        if (opts.begin () != start) {
            // Normal options existed.
            for (auto it = opts.begin (); it != start; ++it) {
                // Should be a normal option.
                RC_ASSERT_FALSE (is_special_option (it->c_str ()));
            }
            // Relative order of two normal options are same as original.
            for (auto it = opts.begin (); it != (start - 1); ++it) {
                auto e0 = dict.find (*it);
                RC_ASSERT (e0 != dict.end ());
                auto e1 = dict.find (*(it + 1));
                RC_ASSERT (e1 != dict.end ());
                RC_ASSERT (e0->second < e1->second);
            }
        }
        if (start != opts.end ()) {
            for (auto it = start; it != opts.end (); ++it) {
                // Should be a normal option.
                RC_ASSERT (is_special_option (it->c_str ()));
            }
            for (auto it = start; it != (opts.end () - 1); ++it) {
                auto e0 = dict.find (*it);
                RC_ASSERT (e0 != dict.end ());
                auto e1 = dict.find (*(it + 1));
                RC_ASSERT (e1 != dict.end ());
                RC_ASSERT (e0->second < e1->second);
            }
        }
    });
}

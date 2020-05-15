
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
TEST_CASE ("Examples") {
    SUBCASE ("no doctest args") {
        std::array<const char *, 8> src = {"a", "b", "c", "d", "e", "f", "g", "h"};
        std::array<const char *, 8> dst {src};
        auto start = skimArguments (dst.size (), dst.data (), is_doctest_argument);
        CHECK_EQ (start, 8);
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
        auto start = skimArguments (dst.size (), dst.data (), is_doctest_argument);
        CHECK_EQ (start, 6);
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
        auto start = skimArguments (dst.size (), dst.data (), is_doctest_argument);
        CHECK_EQ (start, 4);
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
        },
                               genSpecial (), genOpt ());
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


TEST_CASE ("Property") {
    rc::prop ("test skimArguments property", [] {
        auto const src = *rc::gen::unique<std::vector<std::string>> (genOption ());
        std::map<std::string, size_t> dict;
        std::vector<const char *> opts;
        opts.reserve (src.size ());
        for (auto const &s : src) {
            dict.try_emplace (s, opts.size ());
            opts.emplace_back (s.c_str ());
        }
        auto start  = skimArguments (opts.size (), opts.data (), is_special_option);
        auto cntSpl = std::count_if (src.begin (), src.end (), [] (auto const &s) { return is_special_option (s.c_str ()); });
        if (false) {
            dump (std::cout, opts.cbegin (), opts.cend ());
        }
        // Total # of options should be same.
        RC_ASSERT ((start + cntSpl) == src.size ());
        int prevIndex = -1;
        for (int i = 0; i < start; ++i) {
            std::string k {opts[i]};
            // Should be a normal option.
            RC_ASSERT_FALSE (is_special_option (opts[i]));
            auto it = dict.find (k);
            // Order of option should be preserved.
            RC_ASSERT (it != dict.end ());
            RC_ASSERT (prevIndex < static_cast<int> (it->second));
            prevIndex = static_cast<int> (it->second);
        }
        prevIndex = -1;
        for (int i = start; i < opts.size (); ++i) {
            std::string k {opts[i]};
            RC_ASSERT (is_special_option (opts[i]));
            auto it = dict.find (k);
            RC_ASSERT (it != dict.end ());
            RC_ASSERT (prevIndex < static_cast<int> (it->second));
            prevIndex = static_cast<int> (it->second);
        }
    });
}

TEST_CASE ("Split") {
    rc::prop ("prop", [] {
        auto const src = *rc::gen::unique<std::vector<std::string>> (genOption ());
        std::map<std::string, size_t> dict;
        std::vector<std::string> opts;
        opts.reserve (src.size ());
        for (auto const &s : src) {
            dict.try_emplace (s, opts.size ());
            opts.emplace_back (s);
        }
        auto start  = split (opts.begin(), opts.end (), [](auto const &s) { return is_special_option (s.c_str ()); });
        auto cntSpl = std::count_if (src.begin (), src.end (), [] (auto const &s) { return is_special_option (s.c_str ()); });
        if (false) {
            dump (std::cout, opts.cbegin (), opts.cend ());
        }
        // Total # of options should be same.
        RC_ASSERT (std::distance (start, opts.end ()) == cntSpl);
        int prevIndex = -1;
        for (auto it = opts.begin (); it != start; ++it) {
            // Should be a normal option.
            RC_ASSERT_FALSE (is_special_option (it->c_str ()));
            auto e = dict.find (*it);
            // Order of option should be preserved.
            RC_ASSERT (e != dict.end ());
            RC_ASSERT (prevIndex < static_cast<int> (e->second));
            prevIndex = static_cast<int> (e->second);
        }
        prevIndex = -1;
        for (auto it = start ; it != opts.end (); ++it) {
            RC_ASSERT (is_special_option (it->c_str()));
            auto e = dict.find (*it);
            RC_ASSERT (e != dict.end ());
            RC_ASSERT (prevIndex < static_cast<int> (e->second));
            prevIndex = static_cast<int> (e->second);
        }
    });
}

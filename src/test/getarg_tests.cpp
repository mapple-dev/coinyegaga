#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-CLG");
    BOOST_CHECK(GetBoolArg("-CLG"));
    BOOST_CHECK(GetBoolArg("-CLG", false));
    BOOST_CHECK(GetBoolArg("-CLG", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-CLGo"));
    BOOST_CHECK(!GetBoolArg("-CLGo", false));
    BOOST_CHECK(GetBoolArg("-CLGo", true));

    ResetArgs("-CLG=0");
    BOOST_CHECK(!GetBoolArg("-CLG"));
    BOOST_CHECK(!GetBoolArg("-CLG", false));
    BOOST_CHECK(!GetBoolArg("-CLG", true));

    ResetArgs("-CLG=1");
    BOOST_CHECK(GetBoolArg("-CLG"));
    BOOST_CHECK(GetBoolArg("-CLG", false));
    BOOST_CHECK(GetBoolArg("-CLG", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noCLG");
    BOOST_CHECK(!GetBoolArg("-CLG"));
    BOOST_CHECK(!GetBoolArg("-CLG", false));
    BOOST_CHECK(!GetBoolArg("-CLG", true));

    ResetArgs("-noCLG=1");
    BOOST_CHECK(!GetBoolArg("-CLG"));
    BOOST_CHECK(!GetBoolArg("-CLG", false));
    BOOST_CHECK(!GetBoolArg("-CLG", true));

    ResetArgs("-CLG -noCLG");  // -CLG should win
    BOOST_CHECK(GetBoolArg("-CLG"));
    BOOST_CHECK(GetBoolArg("-CLG", false));
    BOOST_CHECK(GetBoolArg("-CLG", true));

    ResetArgs("-CLG=1 -noCLG=1");  // -CLG should win
    BOOST_CHECK(GetBoolArg("-CLG"));
    BOOST_CHECK(GetBoolArg("-CLG", false));
    BOOST_CHECK(GetBoolArg("-CLG", true));

    ResetArgs("-CLG=0 -noCLG=0");  // -CLG should win
    BOOST_CHECK(!GetBoolArg("-CLG"));
    BOOST_CHECK(!GetBoolArg("-CLG", false));
    BOOST_CHECK(!GetBoolArg("-CLG", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--CLG=1");
    BOOST_CHECK(GetBoolArg("-CLG"));
    BOOST_CHECK(GetBoolArg("-CLG", false));
    BOOST_CHECK(GetBoolArg("-CLG", true));

    ResetArgs("--noCLG=1");
    BOOST_CHECK(!GetBoolArg("-CLG"));
    BOOST_CHECK(!GetBoolArg("-CLG", false));
    BOOST_CHECK(!GetBoolArg("-CLG", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CLG", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CLG", "eleven"), "eleven");

    ResetArgs("-CLG -bar");
    BOOST_CHECK_EQUAL(GetArg("-CLG", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CLG", "eleven"), "");

    ResetArgs("-CLG=");
    BOOST_CHECK_EQUAL(GetArg("-CLG", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-CLG", "eleven"), "");

    ResetArgs("-CLG=11");
    BOOST_CHECK_EQUAL(GetArg("-CLG", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-CLG", "eleven"), "11");

    ResetArgs("-CLG=eleven");
    BOOST_CHECK_EQUAL(GetArg("-CLG", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-CLG", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-CLG", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-CLG", 0), 0);

    ResetArgs("-CLG -bar");
    BOOST_CHECK_EQUAL(GetArg("-CLG", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-CLG=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-CLG", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-CLG=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-CLG", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--CLG");
    BOOST_CHECK_EQUAL(GetBoolArg("-CLG"), true);

    ResetArgs("--CLG=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-CLG", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noCLG");
    BOOST_CHECK(!GetBoolArg("-CLG"));
    BOOST_CHECK(!GetBoolArg("-CLG", true));
    BOOST_CHECK(!GetBoolArg("-CLG", false));

    ResetArgs("-noCLG=1");
    BOOST_CHECK(!GetBoolArg("-CLG"));
    BOOST_CHECK(!GetBoolArg("-CLG", true));
    BOOST_CHECK(!GetBoolArg("-CLG", false));

    ResetArgs("-noCLG=0");
    BOOST_CHECK(GetBoolArg("-CLG"));
    BOOST_CHECK(GetBoolArg("-CLG", true));
    BOOST_CHECK(GetBoolArg("-CLG", false));

    ResetArgs("-CLG --noCLG");
    BOOST_CHECK(GetBoolArg("-CLG"));

    ResetArgs("-noCLG -CLG"); // CLG always wins:
    BOOST_CHECK(GetBoolArg("-CLG"));
}

BOOST_AUTO_TEST_SUITE_END()

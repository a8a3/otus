#include <gtest/gtest.h>
#include <random>
#include <chrono>
#include <algorithm>

#include <ip_filter.h>

using namespace ip_filter;

// ------------------------------------------------------------------
TEST(TestSplit, SplitCorrectness)
{
    ASSERT_THROW(split_to_tokens("",      '.'), std::invalid_argument);
    ASSERT_EQ   (split_to_tokens("11",    '.'), (tokens{11, 0, 0, 0}));
    ASSERT_THROW(split_to_tokens("..",    '.'), std::invalid_argument);
    ASSERT_THROW(split_to_tokens("11.",   '.'), std::invalid_argument);
    ASSERT_THROW(split_to_tokens(".11",   '.'), std::invalid_argument);
    ASSERT_EQ   (split_to_tokens("11.22", '.'), (tokens{11, 22, 0, 0}));

    ASSERT_THROW(split_to_tokens("1.2.3.4.5", '.'), std::out_of_range);

    ASSERT_THROW(split_to_tokens("192.168.0.256", '.'), std::runtime_error);
    ASSERT_THROW(split_to_tokens("-192.168.0.256", '.'), std::runtime_error);
    
    ASSERT_THROW(split_to_tokens("1.2.3.4.5", '.'), std::out_of_range);

    ASSERT_EQ(split_to_tokens("127.0.0.1", '.'), (tokens{127,0,0,1}));
}

// ------------------------------------------------------------------
TEST(TestJoin, JoinCorrectness)
{
    const auto joined_tokens = join({1, 1, 1, 1}, '.');
    ASSERT_EQ(joined_tokens, std::string{"1.1.1.1"});
}

// ------------------------------------------------------------------
struct ip_pool_sort_type {
    ip_pool                       ips;
    std::function<void(ip_pool&)> sort_func;

    ip_pool_sort_type(const ip_pool& _ips, const std::function<void(ip_pool&)>& _f)
    : ips(_ips)
    , sort_func(_f) {}

    friend std::ostream& operator<<(std::ostream& _os, const ip_pool_sort_type& _ip_sort_type) {

        for( const auto& ip: _ip_sort_type.ips ) {
            _os << join( ip ) << '\n';
        }
        return _os;
    }
};

// ------------------------------------------------------------------
struct SortTest : testing::Test,
                  testing::WithParamInterface<ip_pool_sort_type>
{
    std::default_random_engine rand_engine;
    SortTest() : rand_engine(std::chrono::system_clock::now().time_since_epoch().count())
    {}
};

// ------------------------------------------------------------------
TEST_P(SortTest, SortCorrectness)
{
    const auto ips = GetParam().ips;

    auto random_ips = ips;
    std::shuffle(random_ips.begin(), random_ips.end(), rand_engine);
    
    GetParam().sort_func( random_ips );
    EXPECT_EQ(ips, random_ips);
}

// ------------------------------------------------------------------
INSTANTIATE_TEST_CASE_P(ReverseLexicographicSort, SortTest, 
    testing::Values(ip_pool_sort_type(
        ip_pool{
        {68, 2, 2, 1},
        {46, 2, 1, 1},
        { 5, 1, 1, 1},
        { 1, 1, 1, 1}}, reverse_lexicographic_sort)
));

// ------------------------------------------------------------------
TEST(FilterTest, TestFilterByFirstSymbol)
{
    const auto filtered_ips = filter(ip_pool{
        {1, 2, 3, 4},
        {1,12,13,14},
        {2,22,23,24},
        {2,32,33,34},
        {1,42,43,44}}, 1);

    const auto result_ips = ip_pool{
            {1, 2, 3, 4},
            {1,12,13,14},
            {1,42,43,44}};

    ASSERT_EQ(filtered_ips, result_ips);
}

// ------------------------------------------------------------------
TEST(FilterTest, TestFilterByTwoFirstSymbols)
{
    const auto filtered_ips = filter(ip_pool{
        {1, 2, 3, 4},
        {1, 2,13,14},
        {1,22,23,24},
        {2, 2,33,34},
        {1,42,43,44}}, 1, 2);

    const auto result_ips = ip_pool{
            {1, 2, 3, 4},
            {1, 2,13,14} };

    ASSERT_EQ(filtered_ips, result_ips);
}

// ------------------------------------------------------------------
TEST(FilterTest, TestFilterAny)
{
    const auto filtered_ips = filter_any(ip_pool{
        { 1, 2, 3, 4},
        {21, 2, 1,14},
        {11,22,23, 1},
        { 2, 2,33,34},
        {31, 1,43,44}}, 1);

    const auto result_ips = ip_pool{
        { 1, 2, 3, 4},
        {21, 2, 1,14},
        {11,22,23, 1},
        {31, 1,43,44}};

    ASSERT_EQ(filtered_ips, result_ips);
}

// ------------------------------------------------------------------
int main( int argc, char* argv[] ) {
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
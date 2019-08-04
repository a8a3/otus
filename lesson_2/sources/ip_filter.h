
#pragma once

#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <functional>

#include <boost/lexical_cast.hpp>

namespace ip_filter
{
using token   = uint8_t;
using tokens  = std::array<token, 4>;
using ip_pool = std::vector<tokens>;

// ------------------------------------------------------------------
static token get_token(const std::string& str) 
{
    const int value = std::stoi( str );

    if ( value < 0 || value > 255 )
    {
        throw std::runtime_error(std::string("ip token value= ") + std::to_string(value) + std::string(" is out of range 0..255"));
    }
    return static_cast<token>(value);
}

// ------------------------------------------------------------------
tokens split_to_tokens(const std::string &str, char d)
{
    tokens res{0, 0, 0, 0};    

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);

    try 
    {
        size_t i = 0;
        for( ; stop != std::string::npos; ++i ) 
        {
            res.at(i) = get_token(str.substr(start, stop - start));
            
            start = stop + 1;
            stop = str.find_first_of(d, start);
        }
        res.at( i ) = get_token(str.substr(start));
    }
    catch(const std::invalid_argument& e)
    {
        std::cerr << e.what() << std::endl;
        throw;        
    }
    catch(const std::out_of_range& e)
    {
        std::cerr << e.what() << std::endl;
        throw;
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        throw;
    }
    catch(...)
    {
        std::cerr << "unknown exception!\n";
        throw;
    }
    return res;
}

// ------------------------------------------------------------------
std::vector<std::string> split_to_strings(const std::string &str, char d)
{
    std::vector<std::string> res;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);

    while(stop != std::string::npos)
    {
        res.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }
    res.push_back(str.substr(start));
    return res;
}

// ------------------------------------------------------------------
std::string join( const tokens& from, char separator = '.')
{
    std::string res;
    for(auto b = from.cbegin(), e = from.cend(); b != e; ++b)
    {
        if ( b != from.cbegin() ) 
        {
            res += separator;
        }
        res += std::to_string(*b);
    }
    return res;
}

// ------------------------------------------------------------------
void print( const ip_pool& _ips )
{
    for ( const auto& ip : _ips ) {
        std::cout << join(ip) << std::endl;
    }
}

// ------------------------------------------------------------------
void reverse_lexicographic_sort(ip_pool& ips) {
    std::sort( ips.begin(), ips.end(), [](const tokens& t1, const tokens& t2){
        assert(t1.size() == t2.size());
        
        for ( size_t i = 0; i < t1.size(); ++i ) {
            if (t1[i] != t2[i]) {
                return t1[i] > t2[i];
            }
        }
        return true;
    } );
}

// ------------------------------------------------------------------
static ip_pool filter(const ip_pool& ips, const std::function<bool(const tokens&)>& f) {
    ip_pool res;
    std::copy_if(ips.begin(), ips.end(), std::back_inserter(res), f);
    return res;
}

// ------------------------------------------------------------------
ip_pool filter(const ip_pool& ips, int value) {
    if (ips.empty()) {
        return ips;
    }
    return filter(ips, [value](const auto& tokens) {
        return !tokens.empty() && value == tokens.at(0);
    });
}

// ------------------------------------------------------------------
ip_pool filter(const ip_pool& ips, int value1, int value2) {
    if (ips.empty()) {
        return ips;
    }

    return filter(ips, [value1, value2](const auto& tokens) {
        return tokens.size() > 1 && value1 == tokens.at(0)
                                 && value2 == tokens.at(1);

    });
}

// ------------------------------------------------------------------
ip_pool filter_any(const ip_pool& ips, int value) {
    if (ips.empty()){
        return ips;
    }
    
    return filter(ips, [&value](const auto& tokens){
        return tokens.end() != std::find_if(tokens.begin(), tokens.end(), [&value](const auto& token){
            return value == token;
        });
    });
}

} // namespace ip_filter
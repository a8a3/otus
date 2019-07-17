
#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <functional>

namespace ip_filter 
{
using token   = std::string;
using tokens  = std::vector<token>;
using ip_pool = std::vector<tokens>;

// ------------------------------------------------------------------
tokens split(const std::string &str, char d)
{
    tokens res;

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
std::string join( const tokens& _from, char _separator = '.')
{
    std::string res;
    for(auto b = _from.cbegin(), e = _from.cend(); b != e; ++b)
    {
        if ( b != _from.cbegin() ) 
        {
            res += _separator;
        }
        res += *b;
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
static inline void sort( ip_pool& _ips, std::function<bool(const tokens&, const tokens&)> _f ) {
    std::sort( _ips.begin(), _ips.end(), _f );
}

// ------------------------------------------------------------------
inline void reverse_lexicographic_sort(ip_pool& _ips) {
    std::sort( _ips.begin(), _ips.end(), [](const tokens& _t1, const tokens& _t2){
        assert(_t1.size() == _t2.size());
        
        for ( size_t i = 0; i < _t1.size(); ++i ) {
            if (_t1[i] != _t2[i]) {
                return std::atoi(_t1[i].c_str()) > std::atoi(_t2[i].c_str());
            }
        }
        return true;
    } );
}

// ------------------------------------------------------------------
static inline ip_pool filter(const ip_pool& _ips, const std::function<bool(const tokens&)>& _f) {
    ip_pool res;
    std::copy_if(_ips.begin(), _ips.end(), std::back_inserter(res), _f);
    return res;
}

// ------------------------------------------------------------------
inline ip_pool filter(const ip_pool& _ips, int _value) {
    if (_ips.empty()) {
        return _ips;
    }
    return filter(_ips, [_value](const auto& tokens) {
        return !tokens.empty() && std::to_string(_value) == (tokens.at(0));
    });
}

// ------------------------------------------------------------------
inline ip_pool filter(const ip_pool& _ips, int _value1, int _value2) {
    if (_ips.empty()) {
        return _ips;
    }
    
    const auto str_value1 = std::to_string(_value1);
    const auto str_value2 = std::to_string(_value2);

    return filter( _ips, [str_value1, str_value2](const auto& tokens) {
        return tokens.size() > 1 && str_value1 == tokens.at(0)
                                 && str_value2 == tokens.at(1);
    });
}

// ------------------------------------------------------------------
inline ip_pool filter_any(const ip_pool& _ips, int _value) {
    if (_ips.empty()){
        return _ips;
    }
    const auto str_value = std::to_string(_value);
    return filter(_ips, [&str_value](const auto& tokens){
        return tokens.end() != std::find_if(tokens.begin(), tokens.end(), [&str_value](const auto& _token){
            return str_value == _token;
        });
    });
}

} // namespace ip_filter
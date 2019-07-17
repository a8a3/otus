#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "ip_filter.h"

int main(int, char const**)
{
    try
    {
        ip_filter::ip_pool ips;      
        for(std::string line; std::getline(std::cin, line);)
        {
            std::vector<std::string> v = ip_filter::split(line, '\t');
            ips.push_back(ip_filter::split(v.at(0), '.'));
        }
        // reverse lexicographically sort
        ip_filter::reverse_lexicographic_sort(ips);
        ip_filter::print( ips );

        // filter by first byte and output
        ip_filter::print( ip_filter::filter(ips, 1) );

        // filter by first and second bytes and output
        ip_filter::print( ip_filter::filter(ips, 46, 70) );

        // filter by any byte and output
        ip_filter::print( ip_filter::filter_any(ips, 46) );
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

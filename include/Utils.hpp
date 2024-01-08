#pragma once
#include <string>

namespace DBaseTools
{

std::string trim(const std::string& s)
{
    int l=0, r=(int)s.size();
    while(l<r && s[l]==' ') ++l;
    while(l<r && s[r-1]==' ') --r;
    return s.substr(l, r-l);
}


}
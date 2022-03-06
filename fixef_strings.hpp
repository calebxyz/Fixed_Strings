#include <array>
#include <algorithm>
#include <string>
#include <iostream>

using namespace std::string_view_literals;

constexpr std::size_t operator""_MB(unsigned long long mbs){
    return mbs*1024*1024;
}

struct FixedStringWrapper{
    const char* data_;

    constexpr std::size_t size(){
    std::size_t count = 0;
    for (int i = 0;  data_[i] != 0; i++){
        count++;
    }

    return count;
}

    constexpr const char* end(){
    std::size_t count = 0;
    for (int i = 0;  data_[i] != 0; i++){
        count++;
    }

    return data_ + count;
}

    constexpr const char* begin(){
    return data_;
    }

    
}; 

//keeps up to 1mb size string
struct FIxedSizeArray{
    std::array<char, 1_MB> data_{}; 
    std::size_t size_; 
};


constexpr auto getFixedSizeArray(const std::string& string) { 
    FIxedSizeArray res; 
    std::copy(string.begin(), string.end(), res.data_.data()); 
    res.size_ = string.size(); 
    return res;
}

constexpr auto concatFixedString(auto generator){
    constexpr auto fixedSize = getFixedSizeArray(generator());
    std::array<char, fixedSize.size_> res; 
    std::copy(fixedSize.data_.begin(), fixedSize.data_.begin() + fixedSize.size_, res.begin());
    return res;
}


template <auto Data>
constexpr const auto &StaticData(){
    return Data; 
}

constexpr auto FixedStaticString(auto generator){
    constexpr auto& static_data =  StaticData<concatFixedString(generator)>();
    return std::string_view(static_data.begin(), static_data.size());
}

/*constexpr auto make_crazy_string(std::string_view sc){
    std::string s(sc);
    return s+s+s;
}*/

/*another attemp without constexpr string*/
template <std::size_t Size>
struct fixed_string {
    char _data[Size + 1]{0};
    static constexpr std::size_t _size = Size;

    constexpr explicit(false) fixed_string(char const* str) {
        std::copy_n(str, Size + 1, _data);
    }

    constexpr explicit(false) fixed_string(auto const*... strs) {
        auto size = 0;
        ([&size, this](char const* str){
            auto s = FixedStringWrapper{str}.size();
            std::copy_n(str, s, _data+size);
            size += s; 
        }(strs), ...);
    }


    constexpr explicit(false) fixed_string() = default;

    constexpr explicit(false) operator std::string_view() const {
        return {_data, _size};
    }

    constexpr std::size_t size() const {
        return _size;
    }

    constexpr const char* data() const {
        return _data;
    }

    constexpr auto operator<=>(const fixed_string&) const = default;

    constexpr auto operator+(const fixed_string& other) const{
        return fixed_string<_size+std::decay_t<decltype(other)>::_size>(_data, other._data);
    }

    constexpr auto as_array() const{
        std::array<char, _size+1> res;
        std::copy_n(_data, _size+1, res.data());
        return res;
    }
};

template <unsigned int Size> 
fixed_string(char const (&)[Size]) -> fixed_string<Size - 1>;

template<fixed_string Name>
constexpr auto operator""_fs() { return Name; };

template <std::size_t size>
constexpr auto static_str_impl(const std::array<char, size>& str){
    std::array<char, size> res; 
    std::copy_n(str, size, res.data());
    constexpr auto& static_data =  StaticData<res>();
    return std::string_view(static_data.begin(), static_data.size());
}

constexpr auto static_str(auto generator){
    return static_str_impl(generator());
}



int main(){
    //constexpr auto generator1 = []{return make_crazy_string("Hello World!  ");};
    constexpr auto x = "*Hello* "_fs;
    constexpr auto y = "*World!*"_fs; 
    //constexpr auto static_z = StaticData<x.data>();
    //constexpr auto xx = std::string_view(z);
    constexpr auto generator2 = [x, y](){
        return(x+y);};
    constexpr auto zs =  generator2();   
    //constexpr auto svc = FixedStaticString(generator2);
    //constexpr static auto sv1 = FixedStaticString(generator1);
    //std::cout << sv1;
    std::cout << std::string_view(generator2());
    //std::cout << std::string_view(z).size() << (std::string_view(z) == "*Hello* *World!*"sv);
}

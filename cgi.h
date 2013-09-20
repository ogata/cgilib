// -----------------------------------------------------------------------------
// cgi.h
// -----------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cassert>

extern char **environ;

namespace cgi
{ 
    typedef std::pair<std::string, std::string> kv;
    typedef std::map<std::string, std::string> hash;
    typedef std::vector<std::string> string_array;

    template <typename T> class Nullable
    {
    public:
        Nullable() : initialized_(false)
        {
        }
    public:
        operator bool() const
        {
            return initialized_;
        }

        void set(T const &x)
        {
            initialized_ = true;
            t_ = x;
        }

        T get() const
        {
            return t_;
        }
    private:
        bool initialized_;
        T t_;
    };

    class Utility
    {
    public:
        static std::string htmlEncode(std::string const &str)
        {
            char const *s = str.c_str();
            std::string result;
            for ( ; *s != '\0' ; s++ ) {
                if ( *s == '&' ) {
                    result += "&amp;";
                }
                else if ( *s == '>' ) {
                    result += "&gt;";
                }
                else if ( *s == '<' ) {
                    result += "&lt;";
                }
                else if ( *s == '"' ) {
                    result += "&quot;";
                }
                else {
                    result.push_back(*s);
                }
            }
            return result;
        }

        // application/x-www-form-urlencoded
        // RFC 1866 (NOT 3986)
        static std::string httpFormDecode(std::string const &str)
        {
            char const *s = str.c_str();
            std::string result;
            for ( ; *s != '\0' ; s++ ) {
                if ( *s == '%' ) {
                    char first = *(s + 1);
                    char second = *(s + 2);
                    int n = hoge(first, second);
                    result.push_back(n);
                    s += 2;
                }
                else if ( *s == '+' ) {
                    result.push_back(' ');
                }
                else {
                    result.push_back(*s);
                }
            }
            return result;
        }

        static hash convertToHash(
            std::string const &str, 
            std::string const &delimiter 
            ) 
        {
            char const *s = str.c_str();
            hash m;
            string_array vec = split1(s, delimiter);
            for ( const auto& x : vec ) {
                m.insert(split2(x.c_str(), '='));
            }
            return m;
        }

        static kv split2(char const *s, char delimiter)
        {
            std::string temp(s);
            auto pos = temp.find_first_of(delimiter);
            return make_pair(
                temp.substr(0, pos),
                temp.substr(pos + 1)
                );
        }

        static string_array split1(
            const char* s,
            std::string const &delimiter 
            )
        {
            string_array vec;
            std::string temp;
            for ( ; *s != '\0' ; s++ ) {
                if ( delimiter.find(*s) == std::string::npos ) {
                    temp.push_back(*s);
                }
                else {
                    vec.push_back(temp);
                    temp.clear();
                }
            }
            if ( temp.length() > 0 ) {
                vec.push_back(temp);
            }
            return vec;
        }
    private:
        static int henkan(char c)
        {
            switch ( c ) {
            case '0': 
            case '1': 
            case '2':
            case '3': 
            case '4': 
            case '5':
            case '6': 
            case '7': 
            case '8': 
            case '9':
                return c - 0x30;
            case 'A': 
            case 'B': 
            case 'C':
            case 'D': 
            case 'E': 
            case 'F':
                return c - 0x41 + 0x0a;
            default:
                return -1;
            }
        }

        static int hoge(char a, char b)
        {
            int x = henkan(a);
            int y = henkan(b);
            return ( x << 4 ) + y;
        }
    };

    class DateTime
    {
    public:
        DateTime()
        {
            seconds_from_epoch_ = time(nullptr);
        }
    private:
        time_t seconds_from_epoch_;
    public:
        std::string toString() const
        {
            tm const *gmt = gmtime(&seconds_from_epoch_);
            int const SIZE = 100;
            char buf[SIZE];
            size_t s = strftime( buf, SIZE, "%a, %d %b %Y %T GMT", gmt);
            assert(s != 0);
            return buf;
        }

        void addSeconds(int sec)
        {
            seconds_from_epoch_ += sec;
        }
    };

    std::ostream &operator<<(std::ostream &os, const DateTime &datetime)
    {
        os << datetime.toString();
        return os;
    }

    class Environment
    {
    public:
        Environment()
        {
            char **env = environ;
            while ( *env ) {
                hash_.insert( Utility::split2(*env, '=') );
                env++;
            }
        }

        std::string operator[](const char *key)
        {
            return hash_[key];
        }
    private:
        hash hash_;
    };

    class Form
    {
    public:
        Form()
        {
            std::string s;
            std::cin >> s;
            hash_ = Utility::convertToHash(s, "&");
            for ( auto &kv : hash_ ) {
                hash_[kv.first] = Utility::httpFormDecode(kv.second);
            }
        }

        std::string operator[](const char *key)
        {
            return hash_[key];
        }
    private:
        hash hash_;
    };

    class Cookie
    {
    public:
        Cookie(std::string const &name, std::string const &value)
        {
            hash_[name] = value;
        }

        Cookie(std::string const &str)
        {
            hash hash2 = Utility::convertToHash(str, "; ");
            for ( auto &kv : hash2 ) {
                hash_[kv.first] = kv.second;
            }
        }
    private:
        hash hash_;
        Nullable<DateTime> expires_;
    public:
        std::string operator[](const char *key)
        {
            return hash_[key];
        }

        std::string header() const
        {
            std::ostringstream st;
            st << "Set-Cookie: ";
            for ( auto &kv : hash_ ) {
                st << kv.first << "=" << kv.second << "; ";
            }
            if ( expires_ ) {
                st << "Expires=" << expires_.get();
            }
            return st.str();
        }
    public:
        void setExpires(DateTime const &expires)
        {
            expires_.set(expires);
        }
    };
}

#pragma once

#include <string>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <type_traits>
#include <cstring>

/*
`bool StartsWith(STR string, STR text)` — проверяет, что строка `string` начинается с `text`.

`bool EndsWith(STR string, STR text)` — проверяет, что строка `string` оканчивается на `text`.

`STR StripPrefix(STR string, STR prefix)` — возвращает `string` с убранным `prefix`,
если `string` не начинается на `prefix`, возвращает `string`.

`STR StripSuffix(STR string, STR suffix)` — тоже самое, но с суффиксом.

`STR ClippedSubstr(STR s, size_t pos, size_t n = STR::npos)` — тоже самое, что и `s.substr(pos, n)`,
но если `n` больше `s.size()`, то возвращается `s`.

`STR StripAsciiWhitespace(STR)` — `strip` строки, удаляем все символы с обоих концов
вида [isspace](https://en.cppreference.com/w/cpp/string/byte/isspace).

`std::vector<STR> StrSplit(STR text, STR delim)` — делаем `split` строки по `delim`. Подумайте,
прежде чем копипастить из уже имеющейся задачи. Обойдитесь одной аллокацией памяти.

`STR ReadN(STR filename, int n)` — открывает файл и читает `n` байт из filename. Используйте Linux
Syscalls `open`, `read`, `close`. Если открыть или прочитать файл нельзя, возвращает пустую строчку.

`STR AddSlash(STR path)` — добавляет к `path` файловой системы символ `/`, если его не было.

`STR RemoveSlash(STR path)` — убирает `/` из `path`, если это не сам путь `/` и путь заканчивается
на `/`.

`STR Dirname(STR path)` — известно, что `path` — корректный путь до файла без слеша на конце,
верните папку, в которой этот файл лежит без слеша на конце, если это не корень.

`STR Basename(STR path)` — известно, что `path` — корректный путь до файла, верните его название.

`STR CollapseSlashes(STR path)` — известно, что `path` — корректный путь, но `/` могут повторяться,
надо убрать все повторения.

`STR StrJoin(const std::vector<STR>& strings, STR delimiter)` — склеить все строки в одну через
`delimiter`. Обойдитесь одной аллокацией памяти.

`STR StrCat(Args...)` — склеить все аргументы в один в их строковом представлении.
Должны поддерживаться числа (`int, long, long long` и их `unsigned` версии), также все строковые
типы (`std::string, std::string_view, const char*`). Аргументов в `StrCat` не больше пяти.
Придумайте как это сделать за одну аллокацию памяти.
*/

bool StartsWith(const std::string_view string, const std::string_view text);           // zero
bool EndsWith(const std::string_view string, const std::string_view text);             // zero
std::string_view StripPrefix(std::string_view string, const std::string_view prefix);  // zero
std::string_view StripSuffix(std::string_view string, const std::string_view suffix);  // zero
std::string_view ClippedSubstr(const std::string_view s, size_t pos,
                               size_t n = std::string::npos);     // zero
std::string_view StripAsciiWhitespace(const std::string_view s);  // zero
std::vector<std::string_view> StrSplit(std::string_view text, const std::string_view delim);  // one
std::string ReadN(const std::string& filename, size_t n);                                     // one
std::string AddSlash(std::string_view path);          // zero/one
std::string_view RemoveSlash(std::string_view path);  // zero
std::string_view Dirname(std::string_view path);      // without . and  .. zero
std::string_view Basename(std::string_view path);     // without . and  .. zero
std::string CollapseSlashes(std::string_view path);   // fixed to one
std::string StrJoin(const std::vector<std::string_view>& strings,
                    std::string_view delimiter);  // fixed

template <typename T>
size_t SymNum(const T& a) {
    size_t c = 0;
    if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            if (a < 0) {
                c++;
            }
        }
        if (a == 0) {
            c++;
        } else {
            T b = a;
            while (b != 0) {
                c++;
                b /= 10;
            }
        }
    } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        c += a.size();
    } else if constexpr (std::is_array_v<T>) {
        c += std::strlen(a);
    }
    return c;
}

template <typename T>
void WriteIt(std::string& s, const T& a, size_t& i) {
    if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            if (a < 0) {
                s[i] = '-';
                i++;
            }
        }
        if (a == 0) {
            s[i] = '0';
            i++;
        } else {
            T b = a;
            size_t c = 0;
            while (b != 0) {
                c++;
                b /= 10;
            }
            b = a;
            while (b != 0) {
                s[i + c - 1] = '0' + (b % 10 >= 0 ? b % 10 : -(b % 10));
                b /= 10;
                c--;
            }
            b = a;
            while (b != 0) {
                i++;
                b /= 10;
            }
        }
    } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        std::copy(a.begin(), a.end(), s.begin() + i);
        i += a.size();
    } else if constexpr (std::is_array_v<T>) {
        for (size_t j = 0; j < std::strlen(a); ++j) {
            s[i] = a[j];
            i++;
        }
    }
}

template <typename... Args>
std::string StrCat(const Args&... args) {

    size_t len = (SymNum<Args>(args) + ... + 0);
    std::string s(len, ' ');
    size_t i = 0;
    (WriteIt<Args>(s, args, i), ...);
    return s;
}
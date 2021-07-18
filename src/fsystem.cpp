#include <fsystem.h>
#include <fmt/format.h>

void fsystem::open_in_associated_app(const std::string& file)
{
#ifdef _WIN32
    constexpr const char* code = "start \"\" \"{}\"";
#elif __linux__
    constexpr const char* code = "xdg-open \"{}\"";
#else
    constexpr const char* code = "";
#endif

    auto res = fmt::format(code, file);
    std::system(res.c_str());
}

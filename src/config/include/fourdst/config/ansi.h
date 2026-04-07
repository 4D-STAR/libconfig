#pragma once

#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <windows.h>
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

namespace fourdst::config::utils {

    inline bool supports_ansi_colors() {
        if (std::getenv("NO_COLOR")) return false;
        if (std::getenv("FORCE_COLOR")) return true;

        if (!ISATTY(FILENO(stdout))) return false;

#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return false;
        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return false;
        return (dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
#else
        const char* term = std::getenv("TERM");
        if (!term) return false;
        const std::string term_str(term);
        return term_str != "dumb";
#endif
    }

    inline const bool TERM_COLOR_SUPPORT = supports_ansi_colors();

    class ANSIColor {
    public:
        explicit ANSIColor(const std::string& value) :  m_value(value) {}

        std::string_view get() const {
            if (TERM_COLOR_SUPPORT) {
                return m_value;
            }
            return "";
        }
    private:
        std::string m_value{""};
        ANSIColor() = default;

    };

    inline ANSIColor RED{"\033[31m"};
    inline ANSIColor GREEN{"\033[32m"};
    inline ANSIColor BLUE{"\033[34m"};
    inline ANSIColor CYAN{"\033[36m"};
    inline ANSIColor RESET{"\033[0m"};
}
#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <iostream>
#include <string>

//
// 🎨 Colores ANSI
//
#define RESET   "\033[0m"
#define RED     "\033[0;31m"
#define GREEN   "\033[0;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[0;36m"
#define MAGENTA "\033[0;35m"

//
// 🧱 Funciones auxiliares de formato
//
inline void separator(const std::string& title = "")
{
    std::cout << "\n" << BLUE << "══════════════════════════════════════════════════════════════" << RESET << "\n";
    if (!title.empty())
        std::cout << CYAN << "🔍 " << title << RESET << "\n";
    std::cout << BLUE << "══════════════════════════════════════════════════════════════" << RESET << "\n";
}

inline void info(const std::string& msg)
{
    std::cout << CYAN << "ℹ️  " << msg << RESET << std::endl;
}

inline void warn(const std::string& msg)
{
    std::cout << YELLOW << "⚠️  " << msg << RESET << std::endl;
}

inline void success(const std::string& msg)
{
    std::cout << GREEN << "✅ " << msg << RESET << std::endl;
}

inline void error(const std::string& msg)
{
    std::cerr << RED << "❌ " << msg << RESET << std::endl;
}

#endif // TEST_UTILS_HPP

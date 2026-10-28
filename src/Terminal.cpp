//==============================================================================
// Terminal.cpp
//------------------------------------------------------------------------------
// Terminal.h 的实现：逐字输出（slowprint）、标点停顿、任意键快进、
// UTF-8 中文支持、跨平台键盘检测、颜色与菜单读取。
//==============================================================================
#include "Terminal.h"

#include <chrono>
#include <cstddef>
#include <iostream>
#include <string>
#include <thread>

//--- 平台相关头文件 ----------------------------------------------------------
#if defined(_WIN32)
#  include <windows.h>
#  include <conio.h>
#else
#  include <sys/select.h>
#  include <termios.h>
#  include <unistd.h>
#endif

namespace term
{

#if defined(_WIN32)
// Windows：用 conio 的 _kbhit 非阻塞检测按键（不取出，留给后续 cin 读取）。
static bool inputReady() { return _kbhit() != 0; }
#else
// POSIX：临时把标准输入切为“非规范、无回显”模式，用 select 探测是否有键
// （只探测、不读取，字节留在缓冲区供后续 getline 使用）。
static bool inputReady()
{
    termios oldTerm{};
    tcgetattr(STDIN_FILENO, &oldTerm);
    termios newTerm = oldTerm;
    newTerm.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    tcsetattr(STDIN_FILENO, TCSANOW, &newTerm);

    timeval tv{};
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    const int ready = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm);
    return ready > 0;
}
#endif

void init()
{
#if defined(_WIN32)
    // 开启 Windows 控制台的 ANSI 转义序列支持。
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD  mode   = 0;
    if (handle != nullptr && GetConsoleMode(handle, &mode))
    {
        SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
    // 统一使用 UTF-8，避免中文乱码。
    std::cout << "\033[?25h"; // 确保光标可见
}

//--- UTF-8 工具 --------------------------------------------------------------
// 返回从字节 i 开始的 UTF-8 字符所占字节数（1~4）。
static int utf8CharLength(const std::string& s, std::size_t i)
{
    const auto c = static_cast<unsigned char>(s[i]);
    if ((c & 0x80) == 0x00) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    return 4;
}

// 若字节 i 位于一个“中文标点”的首字节，返回应停顿的毫秒数；否则返回 0。
static int chinesePunctPause(const std::string& s, std::size_t i)
{
    if (i + 2 >= s.size()) return 0;
    const auto a = static_cast<unsigned char>(s[i]);
    const auto b = static_cast<unsigned char>(s[i + 1]);
    const auto cc = static_cast<unsigned char>(s[i + 2]);

    // 全角标点位于 U+FF00..U+FFEF（UTF-8: EF BC xx）
    if (a == 0xEF && b == 0xBC)
    {
        switch (cc)
        {
            case 0x8C: // ，
            case 0x9A: // ：
            case 0x9B: // ；
                return 120;
            case 0x81: // ！
            case 0x9F: // ？
                return 360;
            default:
                return 0;
        }
    }
    if (a == 0xE3 && b == 0x80 && cc == 0x82) return 380; // 。
    if (a == 0xE2 && b == 0x80 && cc == 0xA6) return 180; // …
    if (a == 0xE2 && b == 0x80 && cc == 0x94) return 120; // —
    return 0;
}

void slowPrint(std::string_view text, int charDelayMs)
{
    const std::string s(text);
    bool skipped = false; // 按下任意键后变为 true，剩余字符瞬时输出

    for (std::size_t i = 0; i < s.size();)
    {
        if (!skipped && inputReady())
        {
            // 仅切换为“快进”，不要取出该键：
            // 字节留在输入缓冲区，随后菜单的 getline 仍能读到它——
            // 这样玩家在播放期间提前输入的数字 / 回车都不会被吞掉。
            skipped = true;
        }

        const int len = utf8CharLength(s, i);
        std::cout.write(s.data() + static_cast<std::ptrdiff_t>(i), len);
        std::cout.flush();

        if (!skipped)
        {
            int delay = charDelayMs;
            if (len == 1)
            {
                const char c = s[i];
                if (c == '\n')
                    delay = 160;
                else if (c == ',' || c == ';' || c == ':')
                    delay = 110;
                else if (c == '.' || c == '!' || c == '?')
                    delay = 320;
            }
            else if (len == 3)
            {
                const int punct = chinesePunctPause(s, i);
                if (punct > 0) delay = punct;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        i += static_cast<std::size_t>(len);
    }
}

void slowLine(std::string_view text, int charDelayMs)
{
    slowPrint(text, charDelayMs);
    std::cout << '\n';
}

void pause(std::string_view prompt)
{
    slowPrint(prompt);
    std::string dummy;
    std::getline(std::cin, dummy);
}

void clear()
{
    // ANSI 清屏 + 光标归位（Windows VT 同样支持）。
    std::cout << "\033[2J\033[H";
    std::cout.flush();
}

int readChoice(int maxChoice)
{
    while (true)
    {
        std::string line;
        std::getline(std::cin, line);
        if (!line.empty())
        {
            if (line[0] == 'L' || line[0] == 'l') return -1;
            if (line[0] == 'Q' || line[0] == 'q') return -2;
            try
            {
                std::size_t pos = 0;
                const int value = std::stoi(line, &pos);
                if (value >= 1 && value <= maxChoice) return value;
            }
            catch (...)
            {
                // 落入下方提示
            }
        }
        slowPrint("  （请输入 1~" + std::to_string(maxChoice) +
                  " 之间的编号；L=线索本，Q=退出）\n");
    }
}

} // namespace term

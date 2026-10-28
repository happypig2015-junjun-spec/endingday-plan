//==============================================================================
// Terminal.h
//------------------------------------------------------------------------------
// 终端交互层：封装所有与“屏幕 / 键盘”直接相关的操作，
// 包括核心需求 slowprint（剧情逐字显示）。
//
// 特性：
//   * 逐字符输出，字符间可配置延迟；
//   * 遇到逗号 / 句号 / 省略号等标点自动加长停顿，模拟叙事节奏；
//   * 输出过程中按任意键，可立即“快进”完当前段落；
//   * 完整支持中文（按 UTF-8 多字节序列输出，不会把汉字切成乱码）；
//   * 跨平台：Linux / macOS（termios + select）与 Windows（conio + VT）。
//==============================================================================
#ifndef TERMINAL_H
#define TERMINAL_H

#include <string_view>

namespace term
{

inline constexpr std::string_view kReset   = "\033[0m";
inline constexpr std::string_view kBold    = "\033[1m";
inline constexpr std::string_view kDim     = "\033[2m";
inline constexpr std::string_view kRed      = "\033[31m";
inline constexpr std::string_view kGreen    = "\033[32m";
inline constexpr std::string_view kYellow  = "\033[33m";
inline constexpr std::string_view kBlue    = "\033[34m";
inline constexpr std::string_view kMagenta  = "\033[35m";
inline constexpr std::string_view kCyan     = "\033[36m";
inline constexpr std::string_view kGray     = "\033[90m";

void init();
void slowPrint(std::string_view text, int charDelayMs = 22);
void slowLine(std::string_view text, int charDelayMs = 22);
void pause(std::string_view prompt = "\n[ 按回车继续 ]");
void clear();

// 返回 >=1 为选中编号；-1 为查看线索本；-2 为退出游戏。
int readChoice(int maxChoice);

} // namespace term

#endif // TERMINAL_H

#pragma once

#ifdef _WIN32

#    define END_COLOR

#    define START_GRAY
#    define START_RED
#    define START_GREEN
#    define START_YELLOW
#    define START_BLUE
#    define START_MAGENTA
#    define START_CYAN

#    define START_UNDERLINE
#    define START_GLITTER
#    define START_REVERSE
#    define START_DISAPPEAR

#    define FG_BLACK
#    define FG_RED
#    define FG_GREEN
#    define FG_YELLOW
#    define FG_BLUE
#    define FG_PURPLE
#    define FG_DEEPGREEN
#    define FG_WRITE

#    define BG_BLACK
#    define BG_RED
#    define BG_GREEN
#    define BG_YELLOW
#    define BG_BLUE
#    define BG_PURPLE
#    define BG_DEEPGREEN
#    define BG_WRITE

#    define STYLE_HIGHLIGHT
#    define STYLE_UNDERLINE
#    define STYLE_GLITTER
#    define STYLE_REVERSE
#    define STYLE_DISAPPEAR

#    define cursor_up(N)
#    define cursor_down(N)
#    define cursor_right(N)
#    define cursor_left(N)

#define text_format_fg(text, ForeGround)                text
#define text_format_bg(text, BackGround)                text
#define text_format_fg_bg(text, ForeGround, BackGround) text
#define text_format_fgOrBg_style(text, Fg_or_Bg, Style) text

#define cursor_setPosition(X,Y)
#define clear_screen
#define clear_cursor_to_end
#define cursor_savePosition
#define cursor_resumePosition
#define cursor_hide
#define cursor_show

#else /* not _WIN32 */

#define END_COLOR       "\033[0m"

#define START_GRAY      "\033[1;30m"
#define START_RED       "\033[1;31m"
#define START_GREEN     "\033[1;32m"
#define START_YELLOW    "\033[1;33m"
#define START_BLUE      "\033[1;34m"
#define START_MAGENTA   "\033[1;35m"
#define START_CYAN      "\033[1;36m"

#define START_UNDERLINE "\033[4m"
#define START_GLITTER   "\033[5m"
#define START_REVERSE   "\033[7m"
#define START_DISAPPEAR "\033[8m"

#define FG_BLACK            "30"            // 黑
#define FG_RED              "31"            // 红
#define FG_GREEN            "32"            // 绿
#define FG_YELLOW           "33"            // 黄
#define FG_BLUE             "34"            // 蓝色
#define FG_PURPLE           "35"            // 紫色
#define FG_DEEPGREEN        "36"            // 深绿
#define FG_WRITE            "37"            // 白色

#define BG_BLACK            "40"            // 黑
#define BG_RED              "41"            // 深红
#define BG_GREEN            "42"            // 绿
#define BG_YELLOW           "43"            // 黄色
#define BG_BLUE             "44"            // 蓝色
#define BG_PURPLE           "45"            // 紫色
#define BG_DEEPGREEN        "46"            // 深绿
#define BG_WRITE            "47"            // 白色

#define STYLE_HIGHLIGHT     "1"             // 设置高亮度
#define STYLE_UNDERLINE     "4"             // 下划线
#define STYLE_GLITTER       "5"             // 闪烁
#define STYLE_REVERSE       "7"             // 反显
#define STYLE_DISAPPEAR     "8"             // 消隐


#define cursor_up(N)        "\033["#N"A"    // 光标上移N行
#define cursor_down(N)      "\033["#N"B"    // 光标下移N行
#define cursor_right(N)     "\033["#N"C"    // 光标右移N行
#define cursor_left(N)      "\033["#N"D"    // 光标左移N行

#define text_format_fg(text, ForeGround)                    "\033[" ForeGround "m" text END_COLOR
#define text_format_bg(text, BackGround)                    text_format_fg(text, BackGround)
#define text_format_fg_bg(text, ForeGround, BackGround)     "\033[" BackGround ";" ForeGround "m" text END_COLOR
#define text_format_fgOrBg_style(text, Fg_or_Bg, Style)     text_format_fg_bg(text,Fg_or_Bg,Style)

#define cursor_setPosition(X,Y)     "\033["#Y";"#X"H"   // 设置光标位置
#define clear_screen                "\033[2J"           // 清屏
#define clear_cursor_to_end         "\033[K"            // 清除从光标到行尾的内容
#define cursor_savePosition         "\033[s"            // 保存光标位置
#define cursor_resumePosition       "\033[u"            // 恢复光标位置
#define cursor_hide                 "\033[?25l"         // 隐藏光标
#define cursor_show                 "\033[?25h"         // 显示光标

#endif /* _WIN32 */

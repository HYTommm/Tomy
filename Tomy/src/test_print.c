/*
 * test_print.c — 打印测试
 *
 * 基于 emincin/code (https://github.com/emincin/code/blob/main/c/print/main.c)
 * 的原始实现改造和扩展，MIT License。
 * 原始代码 Copyright (c) 2025 emincin.
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "tomy.h"
#include "test.h"

void print_256_color_table(void)
{
    for (int i = 0; i < 256; i++)
    {
        char buf[4] = { 0 };
        snprintf(buf, sizeof(buf), "%3d", i);
        print(set_bg_idx(i), buf, reset_style(), set(sep = "", end = ""));
        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
        else
        {
            printf(" ");
        }
    }
}

void print_any_test(void)
{
    print();
    print("Hello", "Emin", 42, 3.141592);
}

void print_fmt_test(void)
{
    print("{} is {}", "Emin", 42);
}

void print_color_test(void)
{
    print(set_colors_idx(COLOR_RED, COLOR_WHITE), "REDRUM", reset_style(), set(sep = ""));
    print(set_fg_rgb(102, 255, 178), "Here is Johnny~", reset_style(), set(sep = ""));
}

void test_1(void)
{
    print(1, 2, set(sep = "", end = ""));
    print(3, 4, set(sep = "", end = ""));
    print(set_cursor_pos(2, 1), set(end = ""));
    print(set_fg_idx(COLOR_BRIGHT_GREEN), format("{}/{}", 70, 100), reset_style(), set(sep = ""));
}

void test_2(void)
{
    String* content = format("{{+} print in {}", "C23");
    print((const String*)content);
    FILE* file = fopen("test.txt", "wb");
    print(content, set(file = file));
    fclose(file);
    Call(String, content, Delete);
}

void test_3(void)
{
    Color24 color = rgba(255, 0, 0, 1);
    print(color);
    color = rgba(0, 255, 0, 1);
    print(&color);
    color = rgba(144, 12, 12, 1);
    print(set_fg_color(color), "MURDER", reset_style(), set(sep = ""));
}
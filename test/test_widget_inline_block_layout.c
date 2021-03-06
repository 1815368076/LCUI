﻿#include <LCUI_Build.h>
#include <LCUI/LCUI.h>
#include <LCUI/timer.h>
#include <LCUI/display.h>
#include <LCUI/gui/widget.h>
#include <LCUI/gui/builder.h>
#include <LCUI/gui/widget/textview.h>
#include <stdio.h>
#include "test.h"

/* clang-format off */

#define BOX_SIZE	64.0
#define SCREEN_WIDTH	680
#define SCREEN_HEIGHT	480
#define MENU_HEIGHT	34 * 3 + 2 + 8 * 2
#define XML_FILE	"test_widget_inline_block_layout.xml"

/* clang-format on */

static int check_dropdown_menu(void)
{
	int ret = 0;
	LCUI_Widget menu;
	float height = MENU_HEIGHT;
	menu = LCUIWidget_GetById("test-dropdown");
	CHECK(menu->x == 20 && menu->y == SCREEN_HEIGHT - height - 20);
	CHECK(menu->width > 220 && menu->width < 280);
	CHECK(menu->height == height);
	return ret;
}

static int check_layout(void)
{
	int ret = 0;
	char str[256];
	float width, block_size;
	LCUI_Widget child, box, block;

	block_size = BOX_SIZE + (5 + 1) * 2;
	box = LCUIWidget_GetById("box-container");
	CHECK(box->x == 0 && box->y == 0);
	CHECK(box->width > block_size * 3);
	CHECK(box->width < block_size * 5);
	CHECK(box->height == BOX_SIZE * 2 + (5 + 1) * 2 * 3);

	child = LinkedList_Get(&box->children, 0);
	sprintf(str, "[inline-block %d] (%g, %g) == (%g, %g)", child->index,
		child->x, child->y, 5.0, 5.0);
	CHECK_WITH_TEXT(str, child->x == 5.0 && child->y == 5.0);

	child = LinkedList_Get(&box->children, 1);
	width = (BOX_SIZE + (5 + 1) * 2) * 2 + (5 + 1) * 2 + 5;
	sprintf(str, "[inline-block %d] (%g, %g) == (%g, %g)", child->index,
		child->x, child->y, width, 5.0);
	CHECK_WITH_TEXT(str, child->x == width && child->y == 5.0);

	block = LCUIWidget_GetById("test-block");
	width = block->parent->box.content.width;
	sprintf(str, "[block] (%g, %g) == (%g, %g)", block->width,
		block->height, width, BOX_SIZE);
	CHECK_WITH_TEXT(str,
			block->width == width && block->height == BOX_SIZE);

	ret += check_dropdown_menu();
	return ret;
}

static int check_menu_short_text(void)
{
	int ret = 0;
	LCUI_Widget w, menu;

	w = LCUIWidget_GetById("test-text");
	menu = LCUIWidget_GetById("test-dropdown");
	TextView_SetText(w, "nice");
	LCUIWidget_Update();
	CHECK(menu->width > 220 && menu->width < 280);
	CHECK(menu->height == MENU_HEIGHT);
	return ret;
}

static int check_menu_long_text(void)
{
	int ret = 0;
	LCUI_Widget w, menu;

	w = LCUIWidget_GetById("test-text");
	menu = LCUIWidget_GetById("test-dropdown");
	TextView_SetText(w, "test test test test test test test test test");
	LCUIWidget_Update();
	CHECK(menu->width > 280 && menu->width < 360);
	CHECK(menu->height == MENU_HEIGHT);
	return ret;
}

int test_widget_inline_block_layout(void)
{
	int ret = 0;
	LCUI_Widget root, pack;

	LCUI_Init();
	TEST_LOG("test widget inline block layout\n");
	LCUIDisplay_SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	root = LCUIWidget_GetRoot();
	CHECK(pack = LCUIBuilder_LoadFile(XML_FILE));
	if (!pack) {
		LCUI_Destroy();
		return ret;
	}
	Widget_UpdateStyle(root, TRUE);
	Widget_Append(root, pack);
	Widget_Unwrap(pack);
	LCUIWidget_Update();
	ret += check_layout();
	ret += check_menu_long_text();
	ret += check_menu_short_text();
	LCUI_Destroy();
	return ret;
}

#ifdef PREVIEW_MODE
#include <errno.h>

int tests_count = 0;

static void SetLongTextItem(void *arg)
{
	check_menu_long_text();
}

static void SetShortTextItem(void *arg)
{
	check_menu_short_text();
}

static void TestDropdownMenu(void)
{
	LCUI_SetTimeout(2000, SetLongTextItem, NULL);
	LCUI_SetTimeout(4000, SetShortTextItem, NULL);
}

static void OnTestDropdownMenuRight(void *arg)
{
	LCUI_Widget w = arg;
	Widget_Show(w);
}

static void TestDropdownMenuRight(void)
{
	LCUI_Widget w = LCUIWidget_GetById("test-dropdown-right");
	Widget_Hide(w);
	LCUI_SetTimeout(2000, OnTestDropdownMenuRight, w);
}

int main(void)
{
	LCUI_Widget root, pack;

	LCUI_Init();
	LCUIDisplay_SetSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	root = LCUIWidget_GetRoot();
	pack = LCUIBuilder_LoadFile(XML_FILE);
	if (!pack) {
		LCUI_Destroy();
		return -ENOENT;
	}
	Widget_Append(root, pack);
	Widget_Unwrap(pack);
	TestDropdownMenu();
	TestDropdownMenuRight();
	return LCUI_Main();
}
#endif

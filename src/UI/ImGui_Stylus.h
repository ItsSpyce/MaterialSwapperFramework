#pragma once

#include "ImGui_Sugar.h"

namespace ImGui::Stylus {
#define OPT(_TYPE) std::optional<_TYPE>
#define COLOR(_NAME) std::optional<ImVec4> _NAME##Color

struct Styles {
  OPT(float) alpha;
  OPT(float) disabledAlpha;
  OPT(ImVec2) windowPadding;
  OPT(float) windowRounding;
  OPT(float) windowBorderSize;
  OPT(ImVec2) windowMinSize;
  OPT(ImVec2) windowTitleAlign;
  OPT(float) childRounding;
  OPT(float) childBorderSize;
  OPT(float) popupRounding;
  OPT(float) popupBorderSize;
  OPT(ImVec2) framePadding;
  OPT(float) frameRounding;
  OPT(float) frameBorderSize;
  OPT(ImVec2) itemSpacing;
  OPT(ImVec2) itemInnerSpacing;
  OPT(float) indentSpacing;
  OPT(ImVec2) cellPadding;
  OPT(float) scrollbarSize;
  OPT(float) scrollbarRounding;
  OPT(float) grabMinSize;
  OPT(float) grabRounding;
  OPT(float) tabRounding;
  OPT(float) tabBorderSize;
  OPT(float) tabBarBorderSize;
  OPT(float) tabBarOverlineSize;
  OPT(float) tableAngledHeadersAngle;
  OPT(ImVec2) tableAngledHeadersTextAlign;
  OPT(ImVec2) buttonTextAlign;
  OPT(ImVec2) selectableTextAlign;
  OPT(float) separatorTextBorderSize;
  OPT(ImVec2) separatorTextAlign;
  OPT(ImVec2) separatorTextPadding;
  // colors
  COLOR(text);
  COLOR(textDisabled);
  COLOR(windowBg);
  COLOR(childBg);
  COLOR(popupBg);
  COLOR(border);
  COLOR(borderShadow);
  COLOR(frameBg);
  COLOR(frameBgHovered);
  COLOR(frameBgActive);
  COLOR(titleBg);
  COLOR(titleBgActive);
  COLOR(titleBgCollapsed);
  COLOR(menuBarBg);
  COLOR(scrollbarBg);
  COLOR(scrollbarGrab);
  COLOR(scrollbarGrabHovered);
  COLOR(scrollbarGrabActive);
  COLOR(checkMark);
  COLOR(sliderGrab);
  COLOR(sliderGrabActive);
  COLOR(button);
  COLOR(buttonHovered);
  COLOR(buttonActive);
  COLOR(header);
  COLOR(headerHovered);
  COLOR(headerActive);
  COLOR(separator);
  COLOR(separatorHovered);
  COLOR(separatorActive);
  COLOR(resizeGrip);
  COLOR(resizeGripHovered);
  COLOR(resizeGripActive);
  COLOR(tabHovered);
  COLOR(tab);
  COLOR(tabSelected);
  COLOR(tabSelectedOverline);
  COLOR(tabDimmed);
  COLOR(tabDimmedSelected);
  COLOR(tabDimmedSelectedOverline);
  COLOR(plotLines);
  COLOR(plotLinesHovered);
  COLOR(plotHistogram);
  COLOR(plotHistogramHovered);
  COLOR(tableHeaderBg);
  COLOR(tableBorderStrong);
  COLOR(tableBorderLight);
  COLOR(tableRowBg);
  COLOR(tableRowBgAlt);
  COLOR(textLink);
  COLOR(textSelectedBg);
  COLOR(dragDropTarget);
  COLOR(navCursor);
  COLOR(navWindowingHighlight);
  COLOR(navWindowingDimBg);
  COLOR(modalWindowDimBg);
};

struct StyleCountFrame {
  int styles = 0;
  int colors = 0;
};
static std::stack<StyleCountFrame> g_frames;

static void PushStyles(Styles styles) {
  auto styleCount = 0;
  auto colorCount = 0;

#define PUSH_STYLE_VAR(_NAME, _ENUM)                           \
  if (styles._NAME) {                                          \
    PushStyleVar(ImGuiStyleVar_##_ENUM, styles._NAME.value()); \
    styleCount++;                                              \
  }

#define PUSH_COLOR(_NAME, _ENUM)                                   \
  if (styles._NAME##Color) {                                       \
    PushStyleColor(ImGuiCol_##_ENUM, styles._NAME##Color.value()); \
    colorCount++;                                                  \
  }

  // Styles
  PUSH_STYLE_VAR(alpha, Alpha)
  PUSH_STYLE_VAR(disabledAlpha, DisabledAlpha)
  PUSH_STYLE_VAR(windowPadding, WindowPadding)
  PUSH_STYLE_VAR(windowRounding, WindowRounding)
  PUSH_STYLE_VAR(windowBorderSize, WindowBorderSize)
  PUSH_STYLE_VAR(windowMinSize, WindowMinSize)
  PUSH_STYLE_VAR(windowTitleAlign, WindowTitleAlign)
  PUSH_STYLE_VAR(childRounding, ChildRounding)
  PUSH_STYLE_VAR(childBorderSize, ChildBorderSize)
  PUSH_STYLE_VAR(popupRounding, PopupRounding)
  PUSH_STYLE_VAR(popupBorderSize, PopupBorderSize)
  PUSH_STYLE_VAR(framePadding, FramePadding)
  PUSH_STYLE_VAR(frameRounding, FrameRounding)
  PUSH_STYLE_VAR(frameBorderSize, FrameBorderSize)
  PUSH_STYLE_VAR(itemSpacing, ItemSpacing)
  PUSH_STYLE_VAR(itemInnerSpacing, ItemInnerSpacing)
  PUSH_STYLE_VAR(indentSpacing, IndentSpacing)
  PUSH_STYLE_VAR(cellPadding, CellPadding)
  PUSH_STYLE_VAR(scrollbarSize, ScrollbarSize)
  PUSH_STYLE_VAR(scrollbarRounding, ScrollbarRounding)
  PUSH_STYLE_VAR(grabMinSize, GrabMinSize)
  PUSH_STYLE_VAR(grabRounding, GrabRounding)
  PUSH_STYLE_VAR(tabRounding, TabRounding)
  PUSH_STYLE_VAR(tabBorderSize, TabBorderSize)
  PUSH_STYLE_VAR(tabBarBorderSize, TabBarBorderSize)
  PUSH_STYLE_VAR(tabBarOverlineSize, TabBarOverlineSize)
  PUSH_STYLE_VAR(tableAngledHeadersAngle, TableAngledHeadersAngle)
  PUSH_STYLE_VAR(tableAngledHeadersTextAlign, TableAngledHeadersTextAlign)
  PUSH_STYLE_VAR(buttonTextAlign, ButtonTextAlign)
  PUSH_STYLE_VAR(selectableTextAlign, SelectableTextAlign)
  PUSH_STYLE_VAR(separatorTextBorderSize, SeparatorTextBorderSize)
  PUSH_STYLE_VAR(separatorTextAlign, SeparatorTextAlign)
  PUSH_STYLE_VAR(separatorTextPadding, SeparatorTextPadding)

  // Colors
  PUSH_COLOR(text, Text)
  PUSH_COLOR(textDisabled, TextDisabled)
  PUSH_COLOR(windowBg, WindowBg)
  PUSH_COLOR(childBg, ChildBg)
  PUSH_COLOR(popupBg, PopupBg)
  PUSH_COLOR(border, Border)
  PUSH_COLOR(borderShadow, BorderShadow)
  PUSH_COLOR(frameBg, FrameBg)
  PUSH_COLOR(frameBgHovered, FrameBgHovered)
  PUSH_COLOR(frameBgActive, FrameBgActive)
  PUSH_COLOR(titleBg, TitleBg)
  PUSH_COLOR(titleBgActive, TitleBgActive)
  PUSH_COLOR(titleBgCollapsed, TitleBgCollapsed)
  PUSH_COLOR(menuBarBg, MenuBarBg)
  PUSH_COLOR(scrollbarBg, ScrollbarBg)
  PUSH_COLOR(scrollbarGrab, ScrollbarGrab)
  PUSH_COLOR(scrollbarGrabHovered, ScrollbarGrabHovered)
  PUSH_COLOR(scrollbarGrabActive, ScrollbarGrabActive)
  PUSH_COLOR(checkMark, CheckMark)
  PUSH_COLOR(sliderGrab, SliderGrab)
  PUSH_COLOR(sliderGrabActive, SliderGrabActive)
  PUSH_COLOR(button, Button)
  PUSH_COLOR(buttonHovered, ButtonHovered)
  PUSH_COLOR(buttonActive, ButtonActive)
  PUSH_COLOR(header, Header)
  PUSH_COLOR(headerHovered, HeaderHovered)
  PUSH_COLOR(headerActive, HeaderActive)
  PUSH_COLOR(separator, Separator)
  PUSH_COLOR(separatorHovered, SeparatorHovered)
  PUSH_COLOR(separatorActive, SeparatorActive)
  PUSH_COLOR(resizeGrip, ResizeGrip)
  PUSH_COLOR(resizeGripHovered, ResizeGripHovered)
  PUSH_COLOR(resizeGripActive, ResizeGripActive)
  PUSH_COLOR(tabHovered, TabHovered)
  PUSH_COLOR(tab, Tab)
  PUSH_COLOR(tabSelected, TabSelected)
  PUSH_COLOR(tabSelectedOverline, TabSelectedOverline)
  PUSH_COLOR(tabDimmed, TabDimmed)
  PUSH_COLOR(tabDimmedSelected, TabDimmedSelected)
  PUSH_COLOR(tabDimmedSelectedOverline, TabDimmedSelectedOverline)
  PUSH_COLOR(plotLines, PlotLines)
  PUSH_COLOR(plotLinesHovered, PlotLinesHovered)
  PUSH_COLOR(plotHistogram, PlotHistogram)
  PUSH_COLOR(plotHistogramHovered, PlotHistogramHovered)
  PUSH_COLOR(tableHeaderBg, TableHeaderBg)
  PUSH_COLOR(tableBorderStrong, TableBorderStrong)
  PUSH_COLOR(tableBorderLight, TableBorderLight)
  PUSH_COLOR(tableRowBg, TableRowBg)
  PUSH_COLOR(tableRowBgAlt, TableRowBgAlt)
  PUSH_COLOR(textLink, TextLink)
  PUSH_COLOR(textSelectedBg, TextSelectedBg)
  PUSH_COLOR(dragDropTarget, DragDropTarget)
  PUSH_COLOR(navCursor, NavCursor)
  PUSH_COLOR(navWindowingHighlight, NavWindowingHighlight)
  PUSH_COLOR(navWindowingDimBg, NavWindowingDimBg)
  PUSH_COLOR(modalWindowDimBg, ModalWindowDimBg)

  g_frames.push({styleCount, colorCount});
}

static void PopStyles() {
  if (g_frames.empty()) {
    return;
  }
  auto& frame = g_frames.top();
  if (frame.styles > 0) {
    PopStyleVar(frame.styles);
  }
  if (frame.colors > 0) {
    PopStyleColor(frame.colors);
  }
}

#define ImGui_Stylus(...) \
  IMGUI_SUGAR_SCOPED_VOID_N(ImGui::Stylus::PushStyles, ImGui::Stylus::PopStyles, __VA_ARGS__)
}  // namespace ImGui::Stylus
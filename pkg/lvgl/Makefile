PKG_NAME=lvgl
PKG_URL=https://github.com/littlevgl/lvgl
PKG_VERSION=2e942060f077de19667ec200afcfd2b32352064b  # v8.0.2
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-empty-body

LVGL_MODULES =      \
    lvgl_core       \
    lvgl_draw       \
    lvgl_extra      \
    lvgl_font       \
    lvgl_hal        \
    lvgl_misc       \
    lvgl_widgets    \
    #

LVGL_LAYOUTS_MODULES =  \
    lvgl_layout_flex   \
    lvgl_layout_grid \
    #

LVGL_THEMES_MODULES =  \
    lvgl_theme_basic   \
    lvgl_theme_default \
    lvgl_theme_mono \
    #

LVGL_WIDGETS_MODULES =  \
    lvgl_widget_calendar \
    lvgl_widget_chart   \
    lvgl_widget_colorwheel \
    lvgl_widget_keyboard   \
    lvgl_widget_led \
    lvgl_widget_list   \
    lvgl_widget_meter \
    lvgl_widget_msgbox \
    lvgl_widget_spinbox   \
    lvgl_widget_spinner   \
    lvgl_widget_tileview   \
    lvgl_widget_tabview \
    lvgl_widget_win \
    #

.PHONY: lvgl_% lvgl_theme_% lvgl_widget_% lvgl_layout_% lvgl_extra

all: $(LVGL_MODULES) $(LVGL_LAYOUTS_MODULES) $(LVGL_THEMES_MODULES) $(LVGL_WIDGETS_MODULES) lvgl_extra

lvgl_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_extra:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/extra -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_layout_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/extra/layouts/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_theme_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/extra/themes/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

lvgl_widget_%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/extra/widgets/$* -f $(CURDIR)/Makefile.lvgl_module MODULE=$@

ifdef DEBUG
APP_OPTIM   := debug
APP_MODULES := tb_graphicsd
else
APP_OPTIM   := release
APP_MODULES := tb_graphics
endif
APP_PLATFORM := android-9
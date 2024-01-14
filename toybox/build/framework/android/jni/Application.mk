ifdef DEBUG
APP_OPTIM   := debug
APP_MODULES := tb_frameworkd
else
APP_OPTIM   := release
APP_MODULES := tb_framework
endif
APP_PLATFORM := android-9

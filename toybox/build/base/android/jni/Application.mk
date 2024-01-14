ifdef DEBUG
APP_OPTIM   := debug
APP_MODULES := tb_based
else
APP_OPTIM   := release
APP_MODULES := tb_base
endif
APP_PLATFORM := android-9
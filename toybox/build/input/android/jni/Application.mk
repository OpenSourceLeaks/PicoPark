ifdef DEBUG
APP_OPTIM   := debug
APP_MODULES := tb_inputd
else
APP_OPTIM   := release
APP_MODULES := tb_input
endif
APP_PLATFORM := android-9
ifdef DEBUG
APP_OPTIM   := debug
APP_MODULES := tb_uid
else
APP_OPTIM   := release
APP_MODULES := tb_ui
endif
APP_PLATFORM := android-9

ifdef DEBUG
APP_OPTIM   := debug
APP_MODULES := tb_networkd
else
APP_OPTIM   := release
APP_MODULES := tb_network
endif
APP_PLATFORM := android-9

######################################
# source
######################################
# C sources
C_SOURCES +=  \
Src/BASIC_HelloWorld.c \
Src/GUI_X.c \
Src/GUIConf.c \
Src/ili9340.c \
Src/LCDConf.c \
Src/misc_fatfs.c \
Src/retarget.c \
Src/vs10xx.c \
Src/mp3_player.c \
Src/ili9486.c \
Middlewares/Third_Party/lwrb/lwrb/src/lwrb/lwrb.c


######################################
# C includes
######################################
C_INCLUDES +=  \
-IMiddlewares/Third_Party/STemWin/inc \
-IMiddlewares/Third_Party/lwrb/lwrb/src/include

#######################################
# LDFLAGS
####################################### 
LIBS += -lSTemWin_CM3_wc16
LIBDIR += -LMiddlewares/Third_Party/STemWin 

#######################################
# C defines
#######################################
C_DEFS += -DLCD_ILI9340

#######################################
# C Options
#######################################
OPT += -fshort-wchar

# *** EOF ***

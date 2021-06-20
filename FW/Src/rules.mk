######################################
# source
######################################
# C sources
C_SOURCES +=  \
Src/GUI_X.c \
Src/GUIConf.c \
Src/ili9340.c \
Src/LCDConf.c \
Src/misc_fatfs.c \
Src/retarget.c \
Src/vs10xx.c \
Src/mp3_player.c \
Src/helper.c \
Src/ili9486.c \
Src/owl.c \
Middlewares/Third_Party/lwrb/lwrb/src/lwrb/lwrb.c 

######################################
##C_SOURCES +=  \
##Middlewares/Third_Party/TraceRecorder/trcKernelPort.c \
##Middlewares/Third_Party/TraceRecorder/trcSnapshotRecorder.c \
##Middlewares/Third_Party/TraceRecorder/trcStreamingRecorder.c 
######################################




######################################
# C includes
######################################
C_INCLUDES +=  \
-IMiddlewares/Third_Party/STemWin/inc \
-IMiddlewares/Third_Party/lwrb/lwrb/src/include \
-IMiddlewares/Third_Party/TraceRecorder/include \
-IMiddlewares/Third_Party/TraceRecorder/config \

#######################################
# LDFLAGS
####################################### 
LIBS += -lSTemWin_CM3_wc16
LIBDIR += -LMiddlewares/Third_Party/STemWin 

#######################################
# C defines
#######################################
C_DEFS += -DLCD_ILI9340
C_DEFS += -DDEBUG
C_DEFS += -DTRACE_FACILITY="0"

#######################################
# C Options
#######################################
OPT += -fshort-wchar

# *** EOF ***

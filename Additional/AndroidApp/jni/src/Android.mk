LOCAL_PATH := $(call my-dir)

NEO_PATH := ../../../../libs/armeabi-v7a
NEO_INCLUDE := ../../../../3rdparty/sdl2/include

include $(CLEAR_VARS)
LOCAL_MODULE    := NeoCommon
LOCAL_SRC_FILES := $(NEO_PATH)/libNeoCommon.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := NeoCore 
LOCAL_SRC_FILES := $(NEO_PATH)/libNeoCore.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := NeoEngine 
LOCAL_SRC_FILES := $(NEO_PATH)/libNeoEngine.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := vorbis
LOCAL_SRC_FILES := $(NEO_PATH)/libvorbis.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := tinyxml
LOCAL_SRC_FILES := $(NEO_PATH)/libtinyxml.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := png
LOCAL_SRC_FILES := $(NEO_PATH)/libpng.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := openal
LOCAL_SRC_FILES := $(NEO_PATH)/libopenal.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := ogg
LOCAL_SRC_FILES := $(NEO_PATH)/libogg.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := npk
LOCAL_SRC_FILES := $(NEO_PATH)/libnpk.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := lua
LOCAL_SRC_FILES := $(NEO_PATH)/liblua.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := jpeg
LOCAL_SRC_FILES := $(NEO_PATH)/libjpeg.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := freetype
LOCAL_SRC_FILES := $(NEO_PATH)/libfreetype.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := bullet
LOCAL_SRC_FILES := $(NEO_PATH)/libbullet.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := NeoPlayer
LOCAL_SRC_FILES := $(NEO_PATH)/libNeoPlayer.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := Neo2D
LOCAL_SRC_FILES := $(NEO_PATH)/libNeo2D.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(NEO_INCLUDE)

# Add your application source files here...
LOCAL_SRC_FILES := glue_main.c
		
LOCAL_SHARED_LIBRARIES := 
LOCAL_STATIC_LIBRARIES := SDL2 SDL2_static NeoPlayer Neo2D NeoCommon NeoEngine NeoCore SDL2 SDL2_static freetype bullet openal vorbis npk zlib jpeg png ogg tinyxml lua stlport_static

LOCAL_LDLIBS := -L../../libs/armeabi-v7a/ -lGLESv1_CM -lGLESv2 -llog -landroid -lOpenSLES -lz
#-lNeoPlayer -lNeoCommon -lNeoEngine -lNeoCore -lfreetype -lbullet -lopenal -lvorbis -lnpk -ljpeg -lpng -logg -ltinyxml -llua -lgnustl_static -lsup++

LOCAL_CPPFLAGS += -std=c++11

include $(BUILD_SHARED_LIBRARY)


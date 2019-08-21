#! /bin/sh

OUT_FILE=/home/manas/Downloads/video-tool/out.mp4

echo Cleaning up..
rm $OUT_FILE 2>/dev/null || true

echo Building...
esy || exit -1

echo Running...
esy x env DOWNLOADS_DIR='/home/manas/Downloads' ASSETS_PATH='#{self.share}' GST_DEBUG=3 nuggets.exe -f ~/development/nuggets-working-directory/input.json

echo Opening in VLC...
if [[ $? -eq 0 && -f $OUT_FILE ]]
then
    vlc $OUT_FILE
fi


# DYLD_LIBRARY_PATH=/usr/lib:/usr/local/Cellar/gettext/0.19.8.1/lib:/System/Library/Frameworks/ImageIO.framework/Resources:/usr/local/lib:/Users/manas/development/likewyss/ges-local/cerbero/build/dist/darwin_x86_64/lib

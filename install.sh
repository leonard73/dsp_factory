# if  [    "$TARGET_COMPILE_PLATFORM" == "$LABEL_win_amd64"  ||
#         "$TARGET_COMPILE_PLATFORM" == "$LABEL_linux_amd64"      ||
#         "$TARGET_COMPILE_PLATFORM" == "$LABEL_linux_arm64"       ||
#         "$TARGET_COMPILE_PLATFORM" == "$LABEL_linux_arm"       ||
#         "$TARGET_COMPILE_PLATFORM" == "$LABEL_android_arm64v8a"   ] ; then
# echo "install dependencies on platform " + $TARGET_COMPILE_PLATFORM
# else
# echo 2
# fi
if  [ $TARGET_COMPILE_PLATFORM == '$LABEL_win_amd64' ] ; then
echo "LABEL_win_amd64"
else
echo 2
fi
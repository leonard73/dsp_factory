DEMO_PATH=${DSP_FACTORY_ROOT_PATH}/Examples/EigenDemo
sh ${NDK_COMPILE_SCRIPT_PATH}/clean.sh
sh ${NDK_COMPILE_SCRIPT_PATH}/build.sh
adb devices
adb push libs/arm64-v8a/EigenDemo /data/local/tmp/
adb shell chmod +x /data/local/tmp/splash.bmp
adb shell chmod +x /data/local/tmp/EigenDemo
adb shell /data/local/tmp/EigenDemo
sh ${NDK_COMPILE_SCRIPT_PATH}/clean.sh
DEMO_PATH=${DSP_FACTORY_ROOT_PATH}/Examples/Tutorial3_PixelTransform
sh ${NDK_COMPILE_SCRIPT_PATH}/clean.sh
sh ${NDK_COMPILE_SCRIPT_PATH}/build.sh
adb devices
adb push libs/arm64-v8a/dspFactoryT3_NeonLinearTransform /data/local/tmp/
adb push ${DEMO_PATH}/DATA/splash.bmp /data/local/tmp/
adb shell chmod +x /data/local/tmp/splash.bmp
adb shell chmod +x /data/local/tmp/dspFactoryT3_NeonLinearTransform
adb shell /data/local/tmp/dspFactoryT3_NeonLinearTransform
adb pull /data/local/tmp/splash_out_neon.bmp ${DEMO_PATH}/Android/
adb pull /data/local/tmp/splash_out_cpu.bmp  ${DEMO_PATH}/Android/
sh ${NDK_COMPILE_SCRIPT_PATH}/clean.sh
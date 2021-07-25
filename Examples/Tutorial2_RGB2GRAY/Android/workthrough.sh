DEMO_PATH=${DSP_FACTORY_ROOT_PATH}/Examples/Tutorial2_RGB2GRAY
sh ${NDK_COMPILE_SCRIPT_PATH}/clean.sh
sh ${NDK_COMPILE_SCRIPT_PATH}/build.sh
adb devices
adb push libs/arm64-v8a/dspFactoryT2_NeonLDTest /data/local/tmp/
adb push ${DEMO_PATH}/DATA/splash.bmp /data/local/tmp/
adb shell chmod +x /data/local/tmp/splash.bmp
adb shell chmod +x /data/local/tmp/dspFactoryT2_NeonLDTest
adb shell /data/local/tmp/dspFactoryT2_NeonLDTest
adb pull /data/local/tmp/splash_out_neon_I8.bmp ${DEMO_PATH}/Android/
adb pull /data/local/tmp/splash_out_neon_I16.bmp ${DEMO_PATH}/Android/
adb pull /data/local/tmp/splash_out_cpu_I16.bmp  ${DEMO_PATH}/Android/
adb pull /data/local/tmp/splash_out_cpu_I32.bmp  ${DEMO_PATH}/Android/
adb pull /data/local/tmp/splash_out_cpu_F32.bmp  ${DEMO_PATH}/Android/
adb pull /data/local/tmp/splash.bmp  ${DEMO_PATH}/Android/
sh ${NDK_COMPILE_SCRIPT_PATH}/clean.sh
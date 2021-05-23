DEMO_PATH=${DSP_FACTORY_ROOT_PATH}/Examples/Tutorial1
sh ${NDK_COMPILE_SCRIPT_PATH}/clean.sh
sh ${NDK_COMPILE_SCRIPT_PATH}/build.sh
adb devices
adb push libs/arm64-v8a/dspFactoryT1_NeonLDTest /data/local/tmp/
adb push ${DEMO_PATH}/DATA/tutorial1_data.jpg /data/local/tmp/
adb shell chmod +x /data/local/tmp/tutorial1_data.jpg
adb shell chmod +x /data/local/tmp/dspFactoryT1_NeonLDTest
adb shell /data/local/tmp/dspFactoryT1_NeonLDTest
adb pull /data/local/tmp/tutorial1_data_outArm.jpg ${DEMO_PATH}/Android/
adb pull /data/local/tmp/tutorial1_data_outNEON.jpg ${DEMO_PATH}/Android/
sh ${NDK_COMPILE_SCRIPT_PATH}/clean.sh
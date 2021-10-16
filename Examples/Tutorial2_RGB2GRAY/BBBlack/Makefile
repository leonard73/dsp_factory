DEMO_PATH:=${DSP_FACTORY_ROOT_PATH}/Examples/Tutorial2_RGB2GRAY
cc:=gcc  -std=c99 -mfpu=neon-vfpv4 -mfloat-abi=hard -DPLATFORM_LINUX_ARM64
srcs:=${DEMO_PATH}/Codes/NeonRgb2Gray.c
exe:=dspFactoryT1_NeonLDTest
TEST_DATA:=${DEMO_PATH}/DATA/splash.bmp
work_path:=${DEMO_PATH}/BBBlack
all:
	$(cc)  $(srcs) -o $(exe)
	sudo cp -f $(TEST_DATA) $(work_path)/ 
run:
	./$(exe)
clean:
	rm -f $(exe)
	rm -f *.jpg

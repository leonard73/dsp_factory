DEMO_PATH:=${DSP_FACTORY_ROOT_PATH}/Examples/Tutorial1_MEMCPY
cc:=gcc  -std=c99 -mfpu=neon-vfpv4 -mfloat-abi=hard -DPLATFORM_LINUX_ARM64
srcs:=${DEMO_PATH}/Codes/NeonLoadStore.c
exe:=dspFactoryT1_NeonLDTest
TEST_DATA:=${DEMO_PATH}/DATA/tutorial1_data.jpg
work_path:=${DEMO_PATH}/BBBlack
all:
	$(cc)  $(srcs) -o $(exe)
	sudo cp -f $(TEST_DATA) $(work_path)/ 
run:
	./$(exe)
clean:
	rm -f $(exe)
	rm -f *.jpg

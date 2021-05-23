DEMO_PATH:=${DSP_FACTORY_ROOT_PATH}/Examples/Tutorial1/
cc:=gcc  -std=c99 -mfpu=neon-vfpv4 -mfloat-abi=hard
srcs:=${DEMO_PATH}/Codes/NeonLoadStore.c
exe:=dspFactoryT1_NeonLDTest
all:
	$(cc)  $(srcs) -o $(exe)
run:
	./$(exe)
clean:
	rm -f $(exe)

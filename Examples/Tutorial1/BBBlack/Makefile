cc=gcc -mfpu=neon-vfpv4 -mfloat-abi=hard
srcs=../Codes/NeonLoadStore.c
exe=dspFactoryT1_NeonLDTest
all:
        $(cc)  $(srcs) -o $(exe)
run:
        ./$(exe)
clean:
        rm -f $(exe)

# How to use buildroot to build the sdcard image

Clone Buildroot in this directory

```bash
git clone https://github.com/buildroot/buildroot.git -b 2018.02.x
```

Create Buildroot output directory, where all the files are going to downloaded and built

```bash
mkdir output
```

Initialize Buildroot to create an sdcard image including the PREEMPT-RT patch

```bash
make BR2_EXTERNAL=$PWD/external -C buildroot/ O=$PWD/output sabrelite_4_14_rt_defconfig
```

Or alternatively, run this command to create an sdcard image without the PREEMPT-RT patch

```bash
make BR2_EXTERNAL=$PWD/external -C buildroot/ O=$PWD/output sabrelite_4_14_defconfig
```

Start build process

```bash
cd output
make
```

When the process is finished, the sdcard image can be found in `output/images/sdcard.img'. To copy it to an sdcard (change sdX to the device name of your sdcard):

```bash
sudo dd if=images/sdcard.img of=/dev/sdX
```

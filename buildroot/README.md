# How to build sdcard image

TODO

```bash
git clone https://github.com/buildroot/buildroot.git -b 2018.02.x
```

```bash
mkdir output
```

```bash
make BR2_EXTERNAL=$PWD/external -C buildroot/ O=$PWD/output sabrelite_4_14_rt_defconfig
```

```bash
cd output
make
```

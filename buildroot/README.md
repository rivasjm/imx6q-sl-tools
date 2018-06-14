# Buildroot configuration

Buildroot configuration files to generate an sdcard for the sabrelite, with or without the PREEMPT-RT patch

Also, my own configuration steps to configure the image once it is running.

## Buildroot configuration characteristics

This Buildroot configuration generates and image with the following characteristics:

* Linux 4.14.12 (Freescale fslc kernel)
* U-boot Boundary Devices branch boundary-v2017.07
* Includes NFS and OpenSSH packages for ethernet access and file sharing
* Includes OpenMP support for the cross-compiler
* Automatically configures `eth0`
* Default root password: `sabrelite`
* Optional: PREEMPT-RT patch (4.14.12-rt10)

## How to use buildroot to build the sdcard image

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

## Additional configuration

This tips assume that the image is running on the board already.

### Add current working directory to the prompt

Edit the `PS1` portion in file `/etc/profile`:

``` bash
if [ "$PS1" ]; then
        if [ "`id -u`" -eq 0 ]; then
                export PS1='\w #: '
        else
                export PS1='\w $: '
        fi
fi
```

### Allow SSH access

Edit file `/etc/ssh/sshd_config` to include these lines (be careful to check if lines are already defined):

```
AllowUsers root
PermitRootLogin yes
PasswordAuthentication yes
```

To connect to the board via ssh: `ssh root@<board ip>`, and enter password `sabrelite`

### Isolate cores (linux isolcpus)

To exclude cores 1, 2 and 3 from the linux scheduler, execute this in u-boot's prompt:

```bash
setenv cmd_custom 'setenv bootargs isolcpus=1,2,3'
```

Cores 1, 2 and 3 can still be used via task affinities.

To persist this u-boot change:

```bash
saveenv
```

We can check if these options have been passed to the linux kernel, by running this in the board once linux has booted-up:

```bash
cat /proc/cmdline
```



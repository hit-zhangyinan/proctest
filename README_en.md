# How to Install a Kernel Module (ko) on Fedora 42 using proctest as an Example

## Setting up DKMS

```bash
sudo dnf install dkms openssl
sudo dkms generate_mok

# we need this defined password later
MOK_PASSWD="fedora"

sudo mokutil -i /var/lib/dkms/mok.pub << EOI
${MOK_PASSWD}
${MOK_PASSWD}
EOI

# use this command to enter "UEFI key management"
sudo systemctl reboot
```

After rebooting, press any key to enter MOK management at the blue screen, see Figure 1

![Figure 1](https://github.com/dell/dkms/blob/main/images/mok-key-1.png)

Select "Enroll MOK", choose "Continue", select "Yes", enter password "fedora" at the password prompt, then reboot

## Writing dkms.conf and Building the ko

Follow my configuration file, modify BUILT_MODULE_NAME and PACKAGE_NAME to your ko name

DEST_MODULE_LOCATION specifies where to store the compressed ko file:

/kernel represents /lib/modules/6.14.6-300.fc42.x86_64/kernel

/updates represents /lib/modules/6.14.6-300.fc42.x86_64/updates

/extra represents /lib/modules/6.14.6-300.fc42.x86_64/extra

After writing the config, create a new directory under /usr/src, the directory name must be `PACKAGE_NAME-PACKAGE_VERSION`

For example: `sudo mkdir /usr/src/simproc-1.0`

Then copy the source code and Makefile needed to build the ko to /usr/src/simproc-1.0:

```bash
$ ls
dkms.conf  Makefile  README.md  simproc.c  usertest.c

$ sudo cp -r . /usr/src/simproc-1.0
$ cd /usr/src/simproc-1.0
```

Build under /usr/src/simproc-1.0:

```bash
$ sudo dkms build -m simproc -v 1.0
Deprecated feature: CLEAN (/usr/src/simproc-1.0/dkms.conf)
Creating symlink /var/lib/dkms/simproc/1.0/source -> /usr/src/simproc-1.0

Sign command: /lib/modules/6.14.6-300.fc42.x86_64/build/scripts/sign-file
Signing key: /var/lib/dkms/mok.key
Public certificate (MOK): /var/lib/dkms/mok.pub

Building module(s)... done.
Signing module /var/lib/dkms/simproc/1.0/build/./simproc.ko

$ sudo dkms install -m simproc -v 1.0
Deprecated feature: CLEAN (/var/lib/dkms/simproc/1.0/source/dkms.conf)
Installing /lib/modules/6.14.6-300.fc42.x86_64/extra/simproc.ko.xz
Running depmod..... done.
```

## Installing the ko

```bash
$ cd /lib/modules/6.14.6-300.fc42.x86_64/extra/

# under /lib/modules/6.14.6-300.fc42.x86_64/extra/
$ sudo xz -d simproc.ko.xz
$ sudo insmod simproc.ko
$ sudo dmesg | tail -10
```

## Testing with User Program

```bash
$ sudo chmod 777 /proc/mydev

yinan@acer:/proc$ cat mydev
irq = 20
mode = 1
yinan@acer:/proc$ echo "33 4" > mydev
yinan@acer:/proc$ cat mydev
irq = 33
mode = 4
```

Note: There's a strange issue where `sudo ./usertest` doesn't seem to work? Needs further investigation.

User space lseek operations require driver support, so proc_lseek needs to be implemented.

Without it, an "Illegal seek" error will be returned.

## Reference Links

[How to insmod ko on Fedora 42](https://discussion.fedoraproject.org/t/how-to-insmod-ko-on-fedora-42/152503)

[What does DKMS do? How do I use it?](https://askubuntu.com/questions/408605/what-does-dkms-do-how-do-i-use-it)

[dell/dkms](https://github.com/dell/dkms)

[Kernel modules contain .ko.xz files](https://forum.manjaro.org/t/kernel-modules-contain-ko-xz-files/90019)

[Illegal seek](https://unix.stackexchange.com/questions/783768/why-does-lseek-return-espipe-when-driver-doesnt-provide-implementation)

[strerror](https://en.cppreference.com/w/cpp/string/byte/strerror)
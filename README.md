# 以 proctest 为例演示在 Fedora 42 上如何安装 ko

## 设置 DKMS

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

在重启后的蓝色界面按任意键进入 MOK 管理，见图 1

![图 1](https://github.com/dell/dkms/blob/main/images/mok-key-1.png)

选择 "Enroll MOK"，选择 "Continue"，选择 "Yes"，在输入密码界面输入 fedora，然后重启

## 编写 dkms.conf 文件并构建 ko

仿照我的配置文件，把 BUILT_MODULE_NAME 和 PACKAGE_NAME 更改为自己的 ko 名字

DEST_MODULE_LOCATION 指定 ko 的压缩文件存放在哪个路径

/kernel 代表 /lib/modules/6.14.6-300.fc42.x86_64/kernel

/updates 代表 /lib/modules/6.14.6-300.fc42.x86_64/updates

/extra 代表 /lib/modules/6.14.6-300.fc42.x86_64/extra

在编写完成后，在 /usr/src 下新建一个文件夹，文件夹名字必须是 `PACKAGE_NAME-PACKAGE_VERSION`

例如 `sudo mkdir /usr/src/simproc-1.0`

然后把构建 ko 需要的源代码和 Makefile 复制到 /usr/src/simproc-1.0 中

```bash
$ ls
dkms.conf  Makefile  README.md  simproc.c  usertest.c

$ sudo cp -r . /usr/src/simproc-1.0
$ cd /usr/src/simproc-1.0
```

在 /usr/src/simproc-1.0 路径下 build：

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

## 安装 ko

```bash
$ cd /lib/modules/6.14.6-300.fc42.x86_64/extra/

# under /lib/modules/6.14.6-300.fc42.x86_64/extra/
$ sudo xz -d simproc.ko.xz
$ sudo insmod simproc.ko
$ sudo dmesg | tail -10
```

## 用户程序测试

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

有个奇怪的问题：`sudo ./usertest` 好像不起作用？需要再研究

## 参考链接

[How to insmod ko on Fedora 42](https://discussion.fedoraproject.org/t/how-to-insmod-ko-on-fedora-42/152503)

[What does DKMS do? How do I use it?](https://askubuntu.com/questions/408605/what-does-dkms-do-how-do-i-use-it)

[dell/dkms](https://github.com/dell/dkms)

[Kernel modules contain .ko.xz files](https://forum.manjaro.org/t/kernel-modules-contain-ko-xz-files/90019)

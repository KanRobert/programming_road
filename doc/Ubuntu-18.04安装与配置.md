# 内容概要
1. Ubuntu-18.04的安装（与Windows并存）
1. 日常使用环境的搭建
3. build最新版本的gcc

# 参考文献
1. [Github的markdown语法](https://guides.github.com/features/mastering-markdown)
1. [Gay友的Ubuntu安装记录](https://github.com/inkss/markdown/blob/master/Linux/Ubuntu/Ubuntu18.04%E5%AE%89%E8%A3%85%E8%AE%B0%E5%BD%95.md)
1. [配置Git](https://git-scm.com/book/zh/v1/%E8%87%AA%E5%AE%9A%E4%B9%89-Git-%E9%85%8D%E7%BD%AE-Git)
1. [Ubuntu关机卡死问题解决方案](https://askubuntu.com/questions/1029068/ubuntu-18-04-stuck-at-shutdown)
1. [vim入门](https://github.com/wsdjeg/vim-galore-zh_cn)
1. [Build GCC on Ubuntu](https://solarianprogrammer.com/2016/10/07/building-gcc-ubuntu-linux/)

# 1. 安装操作系统
## 1.1 引导盘
下载Ubuntu的镜像文件，使用Rufus进行刻录。在设备选项中，分区类型设为GPT，目标系统设为UEFI，在格式化选项中，文件系统设为FAT32，簇大小设为16K。
## 1.2 分区
利用 Windows 系统中的磁盘管理工具压缩出一个新分区给 Ubuntu 使用。**计算机→管理→磁盘管理工具**。对压缩后的新分区不要新建简单卷或格式化。
## 1.3 安装系统
进BIOS，从引导盘启动。选择**安装 Ubuntu，与 Windows boot manager共存**，将分区交给Ubuntu处理。

如果需要自主分区，选择其他项。分区只推荐划分出/ 和家 /home。
> Note1: swap分区可以不用，新系统下可以使用swap文件代替swap分区
>
> Note2: 安装时选择**最小安装**,可以避免安装视频播放、类似office的办公软件等。
> 
> Note3: 选择 / 为主分区，其他可以设为主分区或逻辑分区。Ubuntu下所有软件都是安装在/分区，home分区存放的是用户自己的文件（包括数据文件和配置文件）。

# 2. 系统配置
## 2.1 第一次更新
先移步到**所有软件 (Win + A)→软件更新器**，等待系统完成更新。
接着移步到**所有软件→设置→区域和语言→管理已安装的语言**，完成语言列表的更新。
## 2.2 双系统的时区问题
双系统下 Windows 与 Ubuntu 的时间将会不相同，原因不多阐述，解决方案如下：
```sh
timedatectl set-local-rtc 1 --adjust-system-clock
```
## 2.3 更换终端类型
更换默认终端为**fish**
```sh
sudo apt-add-repository ppa:fish-shell/release-2
sudo apt-get update
sudo apt-get install fish
chsh -s /usr/bin/fish
```
## 2.4 终端下软件包安装器
**Gdebi：**相比于 Ubuntu Software Srore ，在终端下安装 deb 包可以获得更多的信息提示，但是使用 dpkg 命令又无法自动解决依赖问题，所以这里建议使用 gdebi 安装器替代以上两者。
```sh
sudo apt install gdebi
```
## 2.5 输入法：搜狗输入法
Ubuntu 18.04 没有提供 Fcitx 输入框架，先安装框架：
```sh
sudo apt install fcitx
```
先去 搜狗输入法官网 下载输入法安装包，为 deb 格式的安装包，安装它：
```sh
sudo gdebi xxxxxx.deb
```
然后移步到**设置→区域和语言**，删除一部分输入源，只保留汉语，接着选择**管理已安装的语言**，修改*键盘输入法系统*为**fcitx**。关闭窗口，打开所有程序，选择软件**Fcitx 配置**，选择加号添加搜狗输入法。
> 如果没有找到搜狗，就重启系统，再次重复以上步骤即可。
## 2.6 编辑器
- **vim**

首先先安装
```sh
sudo apt install vim
```
更换目录到home
```sh
cd ~
```
打开/新建配置文件
```sh
vim .vimrc
```
将以下内容复制进去并保存退出
``` 
"
" A (not so) minimal vimrc.
"

" You want Vim, not vi. When Vim finds a vimrc, 'nocompatible' is set anyway.
" We set it explicitely to make our position clear!
set nocompatible

filetype plugin indent on  " Load plugins according to detected filetype.
syntax on                  " Enable syntax highlighting.

set autoindent             " Indent according to previous line.
set expandtab              " Use spaces instead of tabs.
set softtabstop =4         " Tab key indents by 4 spaces.
set shiftwidth  =4         " >> indents by 4 spaces.
set shiftround             " >> indents to next multiple of 'shiftwidth'.

set backspace   =indent,eol,start  " Make backspace work as you would expect.
set hidden                 " Switch between buffers without having to save first.
set laststatus  =2         " Always show statusline.
set display     =lastline  " Show as much as possible of the last line.

set showmode               " Show current mode in command-line.
set showcmd                " Show already typed keys when more are expected.

set incsearch              " Highlight while searching with / or ?.
set hlsearch               " Keep matches highlighted.

set ttyfast                " Faster redrawing.
set lazyredraw             " Only redraw when necessary.

set splitbelow             " Open new windows below the current window.
set splitright             " Open new windows right of the current window.

set cursorline             " Find the current line quickly.
set wrapscan               " Searches wrap around end-of-file.
set report      =0         " Always report changed lines.
set synmaxcol   =200       " Only highlight the first 200 columns.

set list                   " Show non-printable characters.
if has('multi_byte') && &encoding ==# 'utf-8'
  let &listchars = 'tab:▸ ,extends:❯,precedes:❮,nbsp:±'
else
  let &listchars = 'tab:> ,extends:>,precedes:<,nbsp:.'
endif

" The fish shell is not very compatible to other shells and unexpectedly
" breaks things that use 'shell'.
if &shell =~# 'fish$'
  set shell=/bin/bash
endif
```



- **visual studio code**
```sh
sudo gdebi xxxxxx.deb
```
## 2.7 更换显卡驱动
Ubuntu安装完成后默认的显卡驱动是**X. Org X**的，在一些机器上易出现常见的[关机卡死bug](https://askubuntu.com/questions/1029068/ubuntu-18-04-stuck-at-shutdown)，我们将其换成**NVIDIA**的。

**软件 & 更新->其他驱动->Using NVIDIA**
## 2.8 GUI分区工具（Gparted）
```sh
sudo apt install gparted
```
## 2.9 文件管理之Git
先安装git
```sh
sudo apt install git
```
配置用户名和邮箱地址
```sh
git config --global user.name "your-name"
git config --global user.email "your-email"
```
为Github账号添加SSH Keys,生成的keys会保存在~/.ssh/下
```sh
ssh-keygen -t rsa -C "your-email"
```
打开文件~/.ssh/id_rsa.pub，将公钥复制到Github中**github-> Settings-> SSH and GPG Keys-> New SSH key**

测试是否连接成功
```sh
ssh -T git@github.com
```
> Note1:这边配置Git使用的SSH协议，在git clone的时候注意与之对应，也使用SSH协议
>
> Note2:id_rsa文件中保存的是密钥，id_rsa.pub文件中保存的是公钥
## 2.10 代理软件之Shadowsocks
首先安装
```sh
ssh -T git@github.com
```
更换目录到home
```sh
cd ~
```
打开/新建配置文件
```sh
vim .json
```
填入以下内容，保存并退出，<>表示其中的内容不需要用""标起来
```
{
    "method": "rc4-md5",
    "password": "your-pass-word",
    "remarks": "your-remark",
    "server": "your-server",
    "server_port": <your-server-port>,
    "local_address":"127.0.0.1",
    "localPort": 1080,
    "shareOverLan": true
}
```
开启代理
```sh
sslocal -c FILEPATH .json
```
最后将浏览器设为手动代理，SocksHost设为127.0.0.1，Port设为1080。
## 2.11 屏幕截图功能
**所有软件 (Win + A)→Screen Shot**

# 3. build最新版的gcc
Ubuntu-18.04自带的GCC是gcc-7.3.0，目前gcc已经更新到8.2.0了，最新版本的gcc在编译时默认支持C++14和C11标准，在编译的时候就不需要加上-std=c++14和-std=c11参数啦。

首先，我们先保证系统已经更新了
```sh
sudo apt update
sudo apt upgrade
```
然后我们先装一下GCC的工具链
```sh
sudo apt install build-essential
```
接着我们下载gcc的源码并解压一下
```sh
cd ~
wget https://ftpmirror.gnu.org/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz
tar xf gcc-8.2.0.tar.gz
```
下载预备文件
```sh
cd gcc-8.2.0
contrib/download_prerequisites
```
在这个时候，我们就要配置build了，其实主要就是生成Makefile。为了保持系统干净，我们使用/user/gcc-8.2作为安装文件夹，并在编译器名字gcc后加上后缀8.2表明其版本。这样的话就不会跟系统默认版本的gcc混起来了,毕竟其他的包可能依赖默认版本的gcc。
```sh
cd ~
mkdir build && cd build
../gcc-8.2.0/configure -v --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --prefix=/usr/local/gcc-8.2 --enable-checking=release --enable-languages=c,c++ --disable-multilib --program-suffix=-8.2
```
现在要开始build了，为了尽可能地快，我们让并行的job数等于电脑的核的数目的两倍。我的电脑是4核的，所以我使并行的job数为8
```sh
make -j 8
```
上面的过程完成之后，gcc就build好了，我们安装一下
```sh
sudo make install
```
将编译器gcc-8.2加到系统的路径当中去
```sh
cd ~
vim .bashrc
```
然后将下面两行粘贴到文件最后并保存
```
export export PATH=/usr/local/gcc-8.2/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/gcc-8.2/lib64:$LD_LIBRARY_PATH
```
让Bash重新加载.bashrc文件，注意如果你现在的终端是fish等其他非系统自带终端的话，请先
输入命令
```sh
bash
```
回到系统自带终端。然后运行
```sh
. .bashrc
```
到这里我们就成功把gcc-8.2安装进系统啦。测试一下我们闪闪发光的新编译器吧！就拿C++14的generalized lambda开刀吧。
```sh
vim test_lambda.cpp
```
敲入以下内容并保存
```C++
// C++14 generalized lambda (could use "auto" for the type of a parameter)      
#include <iostream>

int main() {
    std::cout << [](auto a, auto b) { return a + b; } (5, 6) << std::endl;
    std::cout << [](auto a, auto b) { return a + b; } (5.23, 6.45) << std::endl;
    return 0;
}
```
编译完测试一下，成功～
```sh
g++-8.2 -Wall -pedantic test_lambda.cpp -o test_lambda
./test_lambda
11
11.68
```

# 4. 上传到Github
这是一个笔记，我想把它保存的Github上去。怎么做呢？首先我们在本地新建一个文件夹并设为当前目录
```sh
mkdir 小白的编译学习之路
cd 小白的编译学习之路
```
新建一个Git仓库
```sh
git init
```
我们可以用命令
```sh
ls -a
```
看一下当前目录下有哪些文件。接下来我们创建一份简单的说明文件吧
```sh
echo "This is my way towards knowledge about compilation" > README.md
```
输入以下命令看一下还有那些文件没被track
```sh
git status
```
将文件加入缓冲区
```sh
git add readme.txt
```
再看一下缓冲区状态
```sh
git status
```
提交修改
```sh
git commit -m "first commit“
```
提交成功后，可以查看一下提交记录
```sh
git log
```
我们可以用命令看
```sh
git branch
```
一下当前有哪些分支，当然我们没有创建任何分支，目前只会有一个master分支。
推送到github
```sh
git remote add origin git@github.com:<your-user-name>/<your-repository-name>.git
git push -u origin master
```
完成～
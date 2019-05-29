# 内容概要
1. 一份适合入门的vimrc文件
1. Vim模式简介
1. 文件的打开/创建与退出
1. 正常模式下的常见操作：光标定位和查看字符编码
1. 复制，剪切，粘贴
1. 撤销与重做
1. 缩进，大小写转换
1. 搜索与替换
1. 打开、关闭、保存、移动标签页
1. 显示文件名/行号，执行外部命令
1. 在多行行首插入相同字符串

# 参考文献
1. [vim-mode介绍](https://www.cnblogs.com/zzqcn/p/4619012.html)
1. [vim-visual-copy](https://vim.fandom.com/wiki/Cut/copy_and_paste_using_visual_selection)
1. [vim的搜索与替换](https://www.cnblogs.com/zzqcn/p/4687577.html)
1. [vim保存文件](https://vim.fandom.com/wiki/Saving_a_file)
1. [vim的分割窗口与标签页](https://www.cnblogs.com/zzqcn/p/4788754.html)
1. [vim获取当前文件名](https://vim.fandom.com/wiki/Get_the_name_of_the_current_file)
1. [如何在vim中删除而不是剪切](https://stackoverflow.com/questions/11993851/how-to-delete-not-cut-in-vim)
1. [insert the same characters across multiple lines](https://stackoverflow.com/questions/9549729/vim-insert-the-same-characters-across-multiple-lines)

# 一份适合入门的vimrc文件
将下面内容复制到`.vimrc`文件中，并将该文件放在Home目录下。
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

`.vimrc`配置完，当我们在Shell中使用命令`vim <filename>`打开文件时，就会自动使用`~/.vimrc`中的配置(`~`表示Home目录)，如果想打开某个文件而不使用配置，可以使用`vim <filename> -u NONE`命令。

# 1. Vim模式
Vim主要有4个模式：
* 正常模式(Normal mode)

* 插入模式(Insert mode)

* 命令模式(Command mode)

* 可视模式(Visual mode)

## 1.1 正常模式
正常模式一般用于浏览文件，也包括一些复制、粘贴、删除等操作。这时击键时，一般的键/键组合会被当成功能键，而不会键入对应的字符。

启动vim后默认位于正常模式。
## 1.2 插入模式
在正常模式中按下i, I, a, A等键会进入插入模式。插入模式中，击键时会写入相应的字符。

* `i` : 插入位置在当前字符之前

* `a` : 插入位置在当前字符之后

* `I` : 插入位置在行首第一个非空白字符之前

* `A` : 插入位置在当前行尾

## 1.3 命令模式
在正常模式中，按下：（冒号）键，会进入命令模式。在命令模式中可以执行
一些输入并执行一些vim或插件提供的指令，就像在shell里一样。这些指令包括设置环境、文件操作、调用某个功能等等。

## 1.4 可视模式
在正常模式按下`v`, `V`, `<Ctrl>+v`，可以进入可视模式。可视模式中的
操作有点像拿鼠标进行操作，选择文本的时候有一种鼠标选择的即视感，有时候会很方便。
## 1.5 回到正常模式
不论在其他什么模式下，按一次或两次`<Esc>`键都可以回到正常模式。在正常模式中，`h`,`j`,`k`,`l`与方向箭头对应：


* `h`: 左 

* `j`: 下 

* `k`: 上

* `l`: 右


> 记忆技巧：假设我们使用的是qwert键盘，在hjkl键中，`h`键位于最左边，`l`键位于最右边，`j`的形状像一个向下的勾，`k`可以理解为king，高高在上。

# 2. 打开/创建文件和退出
## 2.1 打开或创建一个文件
```bash
$ vim <file-name>
```
## 2.2 退出
* 无修改退出：  冒号 + q
* 修改后保存退出： 冒号 + wq
* 修改后丢弃修改退出： 冒号 + q!

# 3. 正常模式下的常见操作
## 3.1 光标定位
### 3.1.1 字符定位

* `h` : 左移一个字符

* `l` : 右移一个字符

* `f<c>` : 移动到本行下一个字符`<c>`

* `F<c>` : 移动到本行上一个字符`<c>`

### 3.1.2 单词定位
* `w` ： 移动到下一个单词的开头

* `b` : 移动到上一个单词的开头

* `e` : 移动到当前单词的末尾

* `*` : 移动到下一个相同单词

* `#` : 移动到上一个相同单词

* `g*` : 跳转到下一个包含当前单词的单词处

* `g#` : 跳转到上一个包含当前单词的单词处

### 3.1.3 行定位
* `j` : 向下移动一行

* `k` : 向上移动一行

* `0` : 移动到行首

* `^` : 移动到行首的第一个非空白字符

* `$` : 移动到行尾

* `<n>|` : 移动到当前行第`n`列，比如`12|`是移动到当前行第`12`列

* `<n>G` : 移动到第`n`行，比如`12G`是移动到第`12`行

### 3.1.4 屏幕定位

* `H` : 移动到屏幕最上面一行

* `L` : 移动到屏幕最下面一行

* `M` : 移动到屏幕中间一行

* `<Ctrl>+e` : 屏幕向下滚动一行，光标不动

* `<Ctrl>+y` : 屏幕向上滚动一行，光标不动

* `zt` : 将当前行移动到屏幕顶端

* `zb` : 将当前行移动到屏幕底端

* `zz` : 将当前行移动到屏幕中间

### 3.1.5 页面定位
* `<Ctrl>+f` : 向下滚动一页

* `<Ctrl>+b` : 向上滚动一页

* `<Ctrl>+d` : 向下滚动半页

* `<Ctrl>+u` : 向上滚动半页

### 3.1.6 文件定位
* `gg` : 移动到文件开头

* `G` : 移动到文件末尾

* `<n>%` : 移动到文件`n%`处，比如`30%`是定位到文件`30%`处


### 3.1.8 函数定位
* `[m` : 移动到当前块最开始

* `[[m` : 移动到当前函数的最开始

### 3.1.7 光标定位的前进/回退
* `<Ctrl>+i` : 跳到下一次光标位置

* `<Ctrl>+o` : 跳到上一次光标位置

## 3.2 查看字符编码
* `ga` : 查看光标处字符的ascii码

* `g8` : 查看光标处字符的utf-8编码


# 4. 常用编辑操作
## 4.1 复制
* `yw` ： 复制光标后的一个单词

* `yy` : 复制当前行

* `<n>yy` : 复制以下`n`行

> 复制选中内容：  将光标移动（击键）到要复制内容的开始，按下v进入可视模式，移动光标（击键）选中要拷贝的内容，按`y`复制

## 4.2 剪切
* `dw` : 剪切光标后的一个单词

* `dd` : 剪切当前行

* `<n>dd` : 剪切以下`n`行

> 剪切选中内容：  将光标移动（击键）到要复制内容的开始，按下v进入可视模式，移动光标（击键）选中要拷贝的内容，按`d`剪切

## 4.3 粘贴
* `p` : 将剪切板中的内容粘贴到光标后

* `P` : 将剪切班中的内容粘贴到光标前


## 4.4 撤销与重做
* `u` : 撤销

* `<Ctrl>+r` : 重做

## 4.5 缩进
* `>>` : 当前行向右缩进

* `<<` : 当前行向左缩进

* `<n>>>` : 以下`n`行向右缩进

* `<n><<` : 以下`n`行向左缩进

* `<n>==` : 自动调整以下`n`行的缩进

* `gg=G` : 自动调整文件所有行的缩进

## 4.6 大小写转换
* `～` ： 转换当前字符的大小写

* `guw` : 将光标后的单词变为小写

* `gUw` : 将管标后的单词变为大写

* `guu` : 将当前行全部变为小写

* `gUU` : 将当前行全部变为大写

## 4.7 替换单个字符
* `r` : 按下后，再输入字符将替换当前字符
* `R` : 按下后，进入替换模式，将依次替换之后的字符



# 5. 搜索
* `*` : 向下搜索光标所在处的单词

* `#` : 向上搜索光标所在处的单词

* `/<pattern>` : 向下搜索模式`<pattern>`

* `?<pattern>` : 向上搜索模式`<pattern>`

* `/\c<pattern>` : 不区分大小写向下搜索模式`<pattern>`

* `？\c<pattern>` : 不区分大小写向上搜索模式`<pattern>`

* `/\C<pattern>` : 区分大小写向下搜索模式`<pattern>`

* `？\C<pattern>` : 区分大小写向上搜索模式`<pattern>`

* `:noh` : 取消搜索后的高亮结果

* `:set ic` : 设定为不区分大小写的搜索模式

* `:set noic` :设定为区分大小写的搜索模式

> 搜索后，按`n`跳转到下一个匹配对象，`N`跳转到上一个匹配对象。`gg+n`可跳转到第一个匹配对象，`G+N`可跳转到最后一个匹配对象。

# 6. 替换
将`<scope>`范围内匹配`<pattern>`的所有字符替换为`<str>`，`<option>`为可选参数
```
<scope>s/<pattern>/<str>[/<option>]
```
举例
```
:%s/abc/xyz/gc
```
解释：将当前文件中所有`abc`替换为`xyz`，并在替换前进行确认。选项`g`表示替换每一行所有`abc`，如果不加只替换第一个；选项`c`表示替换前询问用户进行确认。用户可以选择`y`确认，
`n`否认并跳到下一个匹配，`q`停止，`a`不要再询问了全部yes

# 7. 标签页
## 7.1 打开标签页

* `vim -p <files>` : 打开多个文件，并将它们放在标签中

* `:tabe[dit] <file>` : 在当前标签页之后新建一个标签页，在其中放置file的内容

## 7.2 关闭标签页
* `:tabo[only] <file>` : 关闭其他标签页

* `:q` : 关闭当前标签页

* `:qa` : 关闭所有标签页

## 7.3 切换标签页
* `gt` : 切换到下一个标签页

* `gT` : 切换到上一个标签页

## 7.4 保存标签页

* `w` : 保存当前标签页

* `wa` : 保存所有标签页

## 7.5 移动标签页
* `:tabm[ove]<N>` ： 移动标签页到位置`N`

* `:tabm[ove]+<N>` ： 将标签页往右移`N`个位置

* `:tabm[ove]-<N>` ： 将标签页往左移`N`个位置


# 8. 显示文件名/行号
* `:f` : 显示当前文件名

* 先按`1`，再 `<Ctrl>+G` : 显示当前文件的绝对路径

* `:pwd` : 显示文件所在当前文件夹

* `:set laststatus=2` : 一直显示文件名

* `:set laststatus=1` : 取消一直显示文件名

* `:set nu` : 显示行号

* `:set nonu` : 不显示行号

# 9. 执行外部命令
* `:!<command>` 执行外部命令command

* `<Ctrl>+Z` : 挂起vim，切到shell

* `fg` : 切回vim

# 10. 在多行行首插入相同字符串
1. 在正常模式下将光标移动到要插入的第一行的行首
1. 按下`<Ctrl>+v`进入块可视模式（Visual Block Mode），是可视模式中的一种
1. 移动光标选中你要插入的行
1. 按下`I`键（在行首第一个非空白字符之前进行插入）
1. 键入你想要插入的字符串
1. 按`<Esc>`退出
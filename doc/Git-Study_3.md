# 内容概要
1. 远程分支的概念
1. 查看、添加、重命名、删除远程仓库
1. 本地分支相对远程分支的移动
1. 跟踪远程分支
1. 合并、推送、改跟、拉取、删除远程分支
1. 变基的使用场景，目的及原理，变基 vs. 合并
# 参考文献
1. [How to stop tracking a remote branch in Git](https://stackoverflow.com/questions/3046436/how-do-you-stop-tracking-a-remote-branch-in-git)
2. Pro Git
# 下期主题
1. Linux

# 1. 远程分支
## 1.1 远程分支的概念
首先我们先克隆一个仓库
```sh
$ git clone git@github.com:Mr-Awakened/Programming-Road.git
```
*git clone*命令会拉取远程仓库的所有数据，并创建两个指针，一个指针是master，另一个是origin/master,都初始化为与远程仓库的master指向相同的提交对象。其中master是**本地分支**，在我们使用*git commit*命令进行提交时会不断向前推进，也会随*git merge*、*git reset*等命令发生移动；而origin/master被称为**远程分支**,用来标识远程仓库中的master分支在本地提交历史中的位置，自己是不能移动的。
>## 关于“origin”的解释
>远程仓库名字“origin”与分支名字“master”一样，在Git中没有特别的含义。"master"是当你运行*git init*时默认的起始分支名字，“origin”是运行*git clone*时默认的远程仓库名字，原因仅仅是它们的广泛使用。
## 1.2 查看、添加、重命名、删除远程仓库
### 1.2.1 查看远程仓库
我们可以调用*git remote*查看所有远程仓库
```sh
$ git remote -v
origin	git@github.com:Mr-Awakened/Programming-Road.git (fetch)
origin	git@github.com:Mr-Awakened/Programming-Road.git (push)
```
### 1.2.2 添加远程仓库
使用*git remote add \<shortname\> \<url\>*添加远程仓库
```sh
$ git remote add algorithm git@github.com:Mr-Awakened/algorithmcpp.git
```
添加远程仓库实际上是针对该Git仓库的配置，我们可以在该仓库的.git/config文件中找到配置信息
```sh
$ cat .git/config
[core]
	repositoryformatversion = 0
	filemode = true
	bare = false
	logallrefupdates = true
[remote "origin"]
	url = git@github.com:Mr-Awakened/Programming-Road.git
	fetch = +refs/heads/*:refs/remotes/origin/*
[branch "master"]
	remote = origin
	merge = refs/heads/master
[remote "algorithm"]
	url = git@github.com:Mr-Awakened/algorithmcpp.git
	fetch = +refs/heads/*:refs/remotes/algorithm/*
```
与此形成对比的是，之前我们使用
```sh
git config --global user.name "your-name"
git config --global user.email "your-email"
```
配置的用户名和邮箱是对所有仓库的通用配置，配置信息可以在~/.gitconfig文件中找到
```sh
$ cat ~/.gitconfig
[user]
	name = LYC-BF
	email = 540536130@qq.com
[core]
	editor = vim
```
### 1.2.2 重命名、删除远程仓库
我们也可以使用*git remote rename \<oldname\> \<newname\>* 重命名远程仓库，使用*git remote rm \<shortname\>* 删除远程仓库
```sh
$ git remote rename algorithm alg
$ git remote rm alg
```
## 1.3 本地分支相对远程分支的移动
前面已经提到，远程分支是用来标识远程仓库中的分支在本地提交历史中的位置，只要我们不去更新这个标识，它是不会发生移动的。我们可以验证一下。

先查看当前所有分支的提交历史
```sh
$ git log --oneline --decorate --graph --all
* 70f3ab0 (HEAD -> master, origin/master) finish Git学习(二)
* ee7aad6 rename
* 74b75e4 revise abstract for Git learn
* 5bac74c fix path name
* 3adc774 add git basics
* a4f6645 not play game agagin
* ca39164 unspeakable
* 8c63007 delete one source file
* 00b9fbf first commit
```
### 1.3.1 *git commit*导致的偏移
此时master与origin/master指向同一提交对象。我们进行一次新的提交
```sh
$ vim README.md
$ git commit -a -m 'revise introduction'
$ git log --oneline --decorate --graph --all
* 4399b9f (HEAD -> master) revise introduction
* 70f3ab0 (origin/master, origin/HEAD) finish Git学习(二)
* ee7aad6 rename
* 74b75e4 revise abstract for Git learn
* 5bac74c fix path name
* 3adc774 add git basics
* a4f6645 not play game agagin
* ca39164 unspeakable
* 8c63007 delete one source file
* 00b9fbf first commit
```
提交后master相比于origin/master领先了一个提交对象。
### 1.3.2 *git reset*导致的偏移
我们再使用*git reset*命令移动HEAD指针所指向的分支试试。
```sh
$ git reset --soft HEAD~3
$ git log --oneline --decorate --graph --all
* 70f3ab0 (origin/master, origin/HEAD) finish Git学习(二)
* ee7aad6 rename
* 74b75e4 (HEAD -> master) revise abstract for Git learn
* 5bac74c fix path name
* 3adc774 add git basics
* a4f6645 not play game agagin
* ca39164 unspeakable
* 8c63007 delete one source file
* 00b9fbf first commit
```
此时master相比于origin/master落后了两个提交对象。
## 1.4 跟踪远程分支
将一个本地分支与一个远程分支作比较，观察它们在提交上领先或者落后了多少，这就是“跟踪”的概念。在我们使用*git clone*命令克隆一个仓库时，Git会让创建的master分支去跟踪origin/master分支。有时候，我们希望跟踪其他分支，或者自定义跟踪远程分支的本地分支名，这个时候我们就可以使用 *git checkout -b \<local-branch-name\> \<remote-branch-name\>* 命令
```sh
$ git checkout -b latter origin/master
M	README.md
Branch 'latter' set up to track remote branch 'master' from 'origin'.
Switched to a new branch 'latter'
```
如果想查看所有本地分支对应的跟踪情况，可以使用*git branch -vv*命令
```sh
$ git branch -vv
* latter 70f3ab0 [origin/master] finish Git学习(二)
  master 74b75e4 [origin/master: behind 2] revise abstract for Git learn
```
由于之前的*git reset*操作，master分支落后origin/master分支两个提交对象。
## 1.5 合并、推送、改跟、拉取、删除远程分支
### 1.5.1 将远程分支并入本地分支
合并远程分支与合并本地分支的操作一模一样
```sh
$ git checkout master
$ git merge origin/master
Updating 74b75e4..70f3ab0
Fast-forward
```
### 1.5.2 将本地分支推送到远程仓库
我们可以使用 *git push \<remote-name\> \<local-branch-name\>* 向远程仓库推送特定的本地分支
```sh
$ git push origin latter
Total 0 (delta 0), reused 0 (delta 0)
remote: 
remote: Create a pull request for 'latter' on GitHub by visiting:
remote:      https://github.com/Mr-Awakened/Programming-Road/pull/new/latter
remote: 
To github.com:Mr-Awakened/Programming-Road.git
 * [new branch]      latter -> latter
```
### 1.5.3 修改本地分支与远程分支的跟踪关系
在建立跟踪关系后，可以后续进行修改
```sh
$ git branch -vv
  latter 70f3ab0 [origin/master] finish Git学习(二)
* master 70f3ab0 [origin/master] finish Git学习(二)
$ git checkout latter
$ git branch -u origin/latter latter
$ git branch -vv
* latter 70f3ab0 [origin/latter] finish Git学习(二)
  master 70f3ab0 [origin/master] finish Git学习(二)
```
如果我们希望该分支不跟踪任何远程分支怎么办？先删除这个本地的远程分支试试
```sh
$ git branch -dr origin/latter
Deleted remote-tracking branch origin/latter (was 70f3ab0).
$ git branch -a
* latter
  master
  remotes/origin/HEAD -> origin/master
  remotes/origin/master
$ git branch -vv
* latter 70f3ab0 [origin/latter: gone] finish Git学习(二)
  master 70f3ab0 [origin/master] finish Git学习(二)
```
可以看到，本地的远程分支标签虽然没了，但是跟踪关系还在，这让人很难受，我们可以使用*git config*命令去删除这些关系
```sh
$ git config --unset branch.latter.remote
$ git config --unset branch.latter.merge
$ git branch -vv
* latter 70f3ab0 finish Git学习(二)
  master 70f3ab0 [origin/master] finish Git学习(二)
```
### 1.5.4 抓取远程仓库的数据
使用 *git fetch \<remote-name\>* 来抓取远程仓库的数据，并更新远程分支标识
```sh
$ git log --oneline --decorate --graph --all
* 70f3ab0 (HEAD -> latter, origin/master, origin/HEAD, master) finish Git学习(二)
* ee7aad6 rename
* 74b75e4 revise abstract for Git learn
* 5bac74c fix path name
* 3adc774 add git basics
* a4f6645 not play game agagin
* ca39164 unspeakable
* 8c63007 delete one source file
* 00b9fbf first commit
$ git fetch origin
From github.com:Mr-Awakened/Programming-Road
 * [new branch]      latter     -> origin/latter
$ git log --oneline --decorate --graph --all
* 70f3ab0 (HEAD -> latter, origin/master, origin/latter, origin/HEAD, master) finish Git学习(二)
* ee7aad6 rename
* 74b75e4 revise abstract for Git learn
* 5bac74c fix path name
* 3adc774 add git basics
* a4f6645 not play game agagin
* ca39164 unspeakable
* 8c63007 delete one source file
* 00b9fbf first commit
```
可以看出，在抓取数据后，我们在本地恢复了远程分支标识origin/latter，并使其指向其在远程仓库中对应的提交对象的位置。
### 1.5.5 删除远程仓库中的远程分支
现在，我们的远程分支origin/latter与远程分支origin/master指向同一个提交对象，也就是说origin/latter分支已经没有存在的必要了。我们可以运行带有 *--delete* 选项的*git push*命令来删除一个远程分支
```sh
$ git push origin --delete latter
To github.com:Mr-Awakened/Programming-Road.git
 - [deleted]         latter
$ git branch -vv
* latter 70f3ab0 finish Git学习(二)
  master 70f3ab0 [origin/master] finish Git学习(二)
```
可以看出，带有 *--delete* 选项的*git push*命令只是删除了远程仓库中的远程分支，是不会删除对应的本地分支的。

# 2. 变基
## 2.1 变基的使用场景
首先，我们先使用*git reset*和*git commit*命令使得两个分支产生分叉
```sh
$ git reset --hard HEAD~1
$ vim src/hello/hello.h
$ git commit -a -m 'add public specifier'
$ git log --oneline --decorate --graph --all
* 829fb1b (HEAD -> latter) add public specifier
| * 70f3ab0 (origin/master, origin/HEAD, master) finish Git学习(二)
|/  
* ee7aad6 rename
* 74b75e4 revise abstract for Git learn
* 5bac74c fix path name
* 3adc774 add git basics
* a4f6645 not play game agagin
* ca39164 unspeakable
* 8c63007 delete one source file
* 00b9fbf first commit
```
## 2.2 变基的目的及原理
假设我们希望在master分支中合并latter分支所作的修改，但是又不希望使用*git merge*命令使得最新的提交对象有两个父对象，也就是说我们希望提交历史变得简洁，我们就可以使用*git rebase*命令进行**变基**。所谓变基，就是找到两个分支的最近共同祖先，然后提取当前分支相对于该共同祖先修改，然后将该修改应用在作为基底的分支上。
```sh
$ git checkout master
$ git rebase latter
First, rewinding head to replay your work on top of it...
Applying: finish Git学习(二)
$ git log --oneline --decorate --graph --all
* 8990000 (HEAD -> master) finish Git学习(二)
* 829fb1b (latter) add public specifier
| * 70f3ab0 (origin/master, origin/HEAD) finish Git学习(二)
|/  
* ee7aad6 rename
* 74b75e4 revise abstract for Git learn
* 5bac74c fix path name
* 3adc774 add git basics
* a4f6645 not play game agagin
* ca39164 unspeakable
* 8c63007 delete one source file
* 00b9fbf first commit
```
## 2.3 变基 vs. 合并
变基(*git rebase*)与合并(*git merge*)是整合来自不同修改的两张方法。那哪种方式更好呢？

有一种观点认为，仓库的提交历史**记录着实际发生过什么**，因此使用变基改变提交历史就是一种亵渎，我们应该使用合并，哪怕合并产生的提交历史看起来非常复杂。

另一种观点认为，提交历史是**项目过程中发生的故事**，没人会出版一本书的草稿，因此我们应该使用变基来讲一个好故事。

具体情况下，仁者见仁，智者见智。
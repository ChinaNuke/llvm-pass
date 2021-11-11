# 国科大编译作业二：LLVM Pass 实现

第二次作业的复杂程度比第一次要低一些，可以一周以内完成，但是写递归程序很容易把你自己绕晕。这次作业的内容大致是利用 LLVM Pass 去分析 LLVM 产生的中间代码，找到并打印出源代码中对应的函数调用，对于间接函数调用需要去解析得到实际可能调用的函数。在调试时，你可以在适当的位置打印出关键结点的信息，以便快速定位问题。本项目仍然使用增量开发方式，你可以通过查看 Commit 记录来了解我的实现过程，可能会比你直接看最终完成的源代码要轻松一些。如果这个仓库有帮助到你，欢迎点亮上面的 Star ！祝你在这个大作业中有所收获！

## 下载本仓库

```shell
$ git clone https://github.com/ChinaNuke/llvm-pass.git
```

## Docker 环境

课程提供了一个编译安装好了 llvm 的 docker 环境，你可以直接拿来使用，也可以自己创建一个新的 docker 容器或者虚拟机，自行通过包管理工具或者通过源码编译安装相同版本的 clang 。第二个命令中 `-v` 参数指定一个从本机到 docker 容器的目录映射，这样你就可以直接在本机使用 VSCode 等工具编写代码，在 docker 容器中编译运行，而不用把文件拷来拷去。

```shell
$ docker pull lczxxx123/llvm_10_hw:0.2
$ docker run -td --name llvm_experiment_2 -v "$PWD/llvm-pass":/root/llvm-pass lczxxx123/llvm_10_hw:0.2
$ docker exec -it llvm_experiment_2 bash
```

重新开机后，docker 容器可能没有在运行，你不必再执行一遍 `docker run` 命令，只需要执行 `docker start llvm_experiment_2` 就可以了。如果你还是不太熟悉 docker 命令的话，[这里](https://dockerlabs.collabnix.com/docker/cheatsheet/)有一份 Docker Cheat Sheet 可以查阅。

## 编译中间代码

使用课程提供的脚本将所有测试用例源代码编译成中间代码，生成的文件在 `bc` 目录中。

```shell
$ ./compile.sh
```

或者你也可以使用下面的命令手动把每个测试用例进行编译。

```shell
$ clang -emit-llvm -c -O0 -g3 test/test00.c -o bc/test00.bc
```

## 编译程序

从这一小节开始的所有命令都是在 docker 容器中执行的。创建一个名为 `build` 的目录并进入，使用 `-DLLVM_DIR` 指定 LLVM 的安装路径，`-DCMAKE_BUILD_TYPE=Debug` 参数指明编译时包含符号信息以方便调试。以后的每次编译只需要执行 `make` 就可以了，除非你修改了 CMakeLists.txt 。

```shell
$ mkdir build && cd $_
$ cmake -DLLVM_DIR=/usr/local/llvm10ra/ -DCMAKE_BUILD_TYPE=Debug ..
$ make
```

## 运行

```shell
$ ./llvmassignment ../bc/test00.bc
```

## 参考

人人都是代码重构大师！

- https://www.llvm.org/docs/WritingAnLLVMPass.html
- https://www.llvm.org/docs/ProgrammersManual.html#the-module-class
- https://github.com/qianfei11/LLVMAssignment
- https://github.com/ycdxsb/llvm-assignment
- https://github.com/ZoEplA/llvm-assignment
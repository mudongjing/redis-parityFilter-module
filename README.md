**c语言未测试过，不能使用，现已放弃。**



rust 可以编译，具体功能还未测试，并且，目前对于其中的自定义类型未实现free,binlog和aof功能，需要等待之后各方面稳定，再做具体实现。

在target目录中有我自己编译的 rlib文件和so文件，我的系统是centos 64位，其它系统的不保证可用。

编译：

```
cargo build --release
```

可能存在需要clang 情况，大致是因为rust 需要llvm和c环境完成库的编译，大致上可能是说没有找到 类似 libclang.so 这样的文件



这里仅列出centos的解决方法，但其它系统也可在对应的提示中到官网查看具体问题，这里的主要问题在于安装llvm和clang，并声明环境变量

> ```
> sudo yum install centos-release-scl
> sudo yum install llvm-toolset-7
> scl enable llvm-toolset-7 bash #启用 llvm-toolset-7 
> # 此时，可用 
> clang --version # 检查是否成功安装
> ```
>
> 此时我们需要设置环境变量`LIBCLANG_PATH`，但需要找到那些 类似 libclang.so 的文件,
>
> ```
> find / -name "*libclang*"
> # 看一下 类似的文件的所在的路径
> ```
>
> 找到路径后，就可以到`/etc/profile`中填写 export LIBCLANG_PATH=路径。再 `source /etc/profile`即可。
>
> 如果嫌麻烦，比如我，直接运行 export LIBCLANG_PATH=路径 即可
>
> 同样可以达到效果，只不过，系统如果重启就失效了




























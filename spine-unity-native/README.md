
本工程是基于spine 4.1版本，将部分c#的代码逻辑封装到c++中调用，从而减少c#调用逻辑，在部分场景下可以减少时间消耗

spine-cpp-lite是仿照spine 4.2 增加的接口，，封装后的函数给c#脚本中调用

spine-cpp-lite下已经写好了windows下编译windows库脚本(buildwindows.bat)和编译wasm库脚本(buildwasm.bat)
编译windows默认使用配置的，如果有需要指定可自行修改命令
编译wasm需要先配置好emsdk，目前用的版本是3.1.8，安装对应的版本后配置buildwasm.bat里emsdk的路径即可。另外编译的时候需要MinGW32-make或者是Ninja，脚本里使用的是Ninja，因此也需要先安装好Ninja，配置好环境


Assets目录存放了unity工程里使用的库和c#文件，使用时将其拷贝到对应工程下
Plugins里存放spine-cpp-lite编好的库，如果有修改本地编译后替换即可
Spine文件夹包含基础的功能，以官方模板为基础，将部分逻辑改为c++ nativeplugin调用
Spine Examples文件夹为使用修改后的例子，其中Gettting Started里的场景已经都可以运行，其他场景暂未验证


如果当前提供的接口未满足使用场景情况，可以在c++文件中添加需要的接口

改动的逻辑参考c#的实现，并移到c++中，如果有更好的实现方式也可以改进

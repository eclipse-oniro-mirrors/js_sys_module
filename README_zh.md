# js_sys_module子系统/组件

-   [简介](#简介)
-   [目录](#目录)
-   [说明](#说明)
    -   [接口说明](#接口说明)
    -   [使用说明](#使用说明)

-   [相关仓](#相关仓)

## 简介
process主要是获取进程的相关id以及获取和修改进程的工作目录，及进程的退出关闭。通过childprocess对象可以用来创建一个新的进程，主进程可以获取子进程的标准输入输出，以及发送信号和关闭子进程。 
## 目录

```
base/compileruntime/js_sys_module/
├── Class:PROCESS                   # PROCESS类
├── Uid                             # Uid属性
├── Gid                             # Gid属性
├── EUid                            # EUid属性
├── EGid                            # EGid属性
├── Groups                          # Groups属性
├── Pid                             # Pid属性
├── Ppid                            # Ppid属性
├── chdir()                         # chdir方法
├── uptime()                        # uptime方法
├── kill()                          # kill方法
├── abort()                         # abort方法
├── on()                            # on方法
├── exit()                          # exit方法
├── cwd()                           # cwd方法
├── off()                           # off方法
├── runCmd()                        # runCmd方法
└─── Class:CHILDPROCESS             # CHILDPROCESS类
    ├── close()                     # close方法
    ├── kill()                      # kill方法
    ├── getOutput()                 # getOutput方法
    ├── getErrorOutput()            # getErrorOutput方法
    ├── wait()                      # wait方法
    ├── killed                      # killed属性
    ├── pid                         # pid属性
    ├── ppid                        # ppid属性
    └── exitCode                    # exitCode属性
```

## 说明

### 接口说明
| 接口名 | 说明 |
| -------- | -------- |
| getUid() :number | 返回进程的数字用户标识。 |
| getGid() :number | 返回进程的数字组标识。 |
| getEUid() :number | 返回进程的数字有效用户身份。 |
| getEGid() :number | 返回 Node.js 进程的数字有效组标识。 |
| getGroups() :number[] | 返回一个带有补充组 ID 的数组。 |
| getPid() :number | 返回进程的 PID。 |
| getPpid() :number | 返回当前进程的父进程的 PID。                                 |
| chdir(dir:string) :void | 更改 Node.js 进程的当前工作目录。 |
| uptime() :number | 返回当前系统已运行的秒数。 |
| Kill(pid:number, signal:number) :boolean | 将signal信号发送到标识的进程 PID，true代表发送成功。 |
| abort() :void | 会导致 Node.js 进程立即退出并生成一个核心文件。 |
| on(type:string ,listener:EventListener) :void | 用来存储用户所触发的事件。 |
| exit(code:number):void | 会导致 Node.js 进程立即退出。 |
| cwd():string | 返回 Node.js 进程的当前工作目录。 |
| off(type: string): boolean | 会清除用户存储的事件，true代表清除成功。 |
| runCmd(command : string, options?: RunOptions): ChildProcess | 通过runcmd可以fork一个新的进程来运行一段shell，并返回ChildProcess对象。第一个参数command指需要运行的shell，第二个参数options指子进程的一些运行参数。这些参数主要指timeout、killSignal、maxBuffer 。如果设置了timeout则子进程会在超出timeout后发送信号killSignal，maxBuffer用来限制可接收的最大stdout和stderr大小。 |
| wait()： Promise | 用来等待子进程运行结束，返回promise对象，其值为子进程的退出码。 |
| getOutput(): Promise | 用来获取子进程的标准输出。 |
| getErrorOutput(): Promise | 用来获取子进程的标准错误输出。 |
| close(): void | 用来关闭正在运行的子进程。 |
| kill(signo: number): void | 用来发送信号给子进程。 |
| readonly killed: boolean | 表示信号是否发送成功，true代表发送成功。 |
| readonly exitCode: number | 表示子进程的退出码。 |
| pid | 代表子进程ID。 |
| ppid | 代表主进程ID。 |

### 使用说明

各接口使用方法如下：
1.getUid() 
```
getUid(){
    var res =  Process.getUid;
    console.log("-------"+res);
}
```
2.getGid()
```
getGid(){
    var result = Process.getGid;
    console.log("-------"+result);
}
```
3.getEuid()
```
getEuid(){
    var ans = Process.getEuid;
    console.log("-------"+ans);
}
```
4.getEgid()
```
getEgid(){
    var resb = Process.getEgid;
    console.log("-------"+resb);
}
```
5.getGroups()
```
getGroups(){
    var answer = Process.getGroups;
    console.log("-------"+answer);
}
```
6.getPid() 
```
getPid(){
    var result = Process.getPid;
    console.log("-----"+result);
}
```
7.getPpid()
```
getPpid(){
    var result = Process.getPpid;
    console.log("---------"+result);
}
```
8.chdir()
```
chdir(){
    Process.chdir("123456");
}
```
9.uptime()
```
uptime(){
    var num = Process.uptime();
    console.log("---------"+num);
}
```
10.kill()
```
kill(){
    var ansu = Process.kill(5,23);
    console.log("------"+ansu);
}
```
11.abort()
```
abort(){
    Process.abort();
}
```
12.on()
```
on(){
    function add(num){
        var value = num + 5;
        return value;
    }
    Process.on("add",add);
}
```
13.exit()
```
exit(){
    Process.exit(15);
}
```
14.Cwd()
```
Cwd(){
    var result = Process.cwd();
    console.log("----"+result);
}
```
15.off()

```
off(){
    var result =  Process.off("add");
    console.log("---------"+result);
}
```
16.runCmd()
```
runCmd(){
    var child = process.runCmd('echo abc')
    //killSignal可以是数字或字符串
    var child = process.runCmd('echo abc;', {killSignal : 'SIGKILL'});
    var child = process.runCmd('sleep 5; echo abc;', {timeout : 1, killSignal : 9, maxBuffer : 2})
}
```
17.wait()
```
wait()
{
    var child = process.runCmd('ls')
    var status = child.wait();
    status.then(val => {
        console.log(val);
    })
}
```
18.getOutput()
```
getOutput(){
    var child = process.runCmd('echo bcd;');
    var res = child.getOutput();
    child.wait();
    res.then(val => {
        console.log(val);
    })
}
```
19.getErrorOutput()
```
getErrorOutput(){
    var child = process.runCmd('makdir 1.txt'); //执行一个错误命令
    var res = child.getErrorOutput();
    child.wait();
    res.then(val => {
        console.log(val);
    })
}
```
20.close()
```
close(){
    var child =  process.runCmd('ls; sleep 5s;')
    var result = child.close()
    console.log(child.exitCode);
}
```
21.kill()
```
kill(){
    var child =  process.runCmd('ls; sleep 5s;')
    var result = child.kill('SIGHUP');
    child.wait();
    var temp = child.killed;
    console.log(temp);
}
```
22.killed
```
{
    var child = process.runCmd('ls; sleep 5;')
    child.kill(3);
    var killed_ = child.killed;
    console.log(killed_);
    child.wait();
}
```
23.exitCode
```
{
    var child = process.runCmd('ls; sleep 5;')
    child.kill(9);
    child.wait();
    var exitCode_ = child.exitCode;
    console.log(exitCode_);
}
```
24.pid
```
pid
{
    var child = process.runCmd('ls; sleep 5;')
    var pid_ = child.pid;
    console.log(pid_);
    child.wait();
}
```
25.ppid
```
ppid
{
    var child = process.runCmd('ls; sleep 5;')
    var ppid_ = child.ppid;
    console.log(ppid_);
    child.wait();
}
```


## 相关仓

[js_sys_module子系统](https://gitee.com/OHOS_STD/js_sys_module)

[base/compileruntime/js_sys_module/](base/compileruntime/js_sys_module-readme.md)

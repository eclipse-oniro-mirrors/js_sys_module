#### js_sys_module
#### 一、process 介绍
主要是获取进程的相关id以及获取和修改进程的工作目录，及进程的退出关闭。 process进程模块，涉及14个接口。 

1.getUid() :number;

该process.getuid()方法返回进程的数字用户标识。

2.getGid() :number;

该process.getgid()方法返回进程的数字组标识。

3.getEUid() :number;

该process.geteuid()方法返回进程的数字有效用户身份。

4.getEGid() :number;

该process.getegid()方法返回 Node.js 进程的数字有效组标识。

5.getGroups() :number[];

该process.getgroups()方法返回一个带有补充组 ID 的数组。

6.getPid() :number;

该process.pid属性返回进程的 PID。

7.getPpid() :number;

该process.ppid属性返回当前进程的父进程的 PID。

8.chdir(dir:string) :void;

该process.chdir()方法更改 Node.js 进程的当前工作目录。

9.uptime() :number;

该process.uptime()方法返回当前系统已运行的秒数。

10.Kill(pid:number, signal:number) :boolean;

该process.kill()方法将 发送signal到由 标识的进程 pid。

11.abort() :void;

该process.abort()方法会导致 Node.js 进程立即退出并生成一个核心文件。

12.on(type:string ,listener:EventListener) :void;

该process.on()方法是用来存储用户所触发的事件。

13.exit(code:number):void;

该process.Exit()方法会导致 Node.js 进程立即退出。

14.cwd():string;

该process.cwd()方法返回 Node.js 进程的当前工作目录。

15.off(type: string): boolean;

该process.off()方法会清除用户存储的事件。

process 使用方法
import { Process } from '@ohos.process' export default { data: { title: "" },

getGid() {
    var proc = new Process();
    var result = proc.getGid;
    console.log("-------"+result);
},

getUid() {
    var proc = new Process();
    var res =  proc.getUid;
    console.log("-------"+res);
},

getEgid() {
    var proc = new Process();
    var resb = proc.getEgid;
    console.log("-------"+resb);
},

getEuid() {
    var proc = new Process();
    var ans = proc.getEuid;
    console.log("-------"+ans);
},

getGroups() {
    var proc = new Process();
    var answer = proc.getGroups;
    console.log("-------"+answer);
},

uptime() {
    var proc = new Process();
    var num = proc.uptime();
    console.log("---------"+num);
},

kill() {
    var proc = new Process();
    var ansu = proc.kill(5,10);
    console.log("------"+ansu);
},

chdir() {
    var proc = new Process();
    proc.chdir("123456");
},

 getPid() {
    var pro = new Process();
    var result = pro.getPid;
    console.log("-----"+result);
},

getPpid(){
    var pro = new Process();
    var result = pro.getPpid;
    console.log("---------"+result);
},

on(){
    function add(num){
        var value = num + 5;
        return value;
    }
    var proc = new Process();
    proc.on("add",add);
},

off(){
    var pro = new Process();
    var result = pro.off("add");
    console.log("--------"+result);
},

Cwd(){
    var pro = new Process();
    var result = pro.cwd();
    console.log("----"+result);
},

exit(){
    var pro = new Process();
    pro.exit(15);
},

abort(){
    var pro = new Process();
    pro.abort();
},

onInit() {
    this.title = "strings.world";
}
}

#### 二、childprocess简介

通过childprocess对象可以用来创建一个新的进程，主进程可以获取子进程的标准输入输出，以及发送信号和关闭子进程。

接口介绍

1.runCmd(command : string, options?: RunOptions): ChildProcess

通过runcmd可以fork一个新的进程来运行一段shell，并返回ChildProcess对象。

第一个参数command指需要运行的shell，第二个参数options指子进程的一些运行参数。

这些参数主要指timeout、killSignal、maxBuffer 。

如果设置了timeout则子进程会在超出timeout后发送信号killSignal，maxBuffer用来限制可接收的最大stdout和stderr大小。

2.wait()： Promise

wait函数用来等待子进程运行结束，返回promise对象，其值为子进程的退出码。

3.getOutput(): Promise

getOutput函数用来获取子进程的标准输出。

4.getErrorOutput(): Promise

getErrorOutput函数用来获取子进程的标准错误输出。

5.close(): void

close函数用来关闭正在运行的子进程。

6.kill(signo: number): void

kill函数用来发送信号给子进程。

7.readonly killed: boolean

killed表示信号是否发送成功。

8.readonly exitCode: number

exitCode表示子进程的退出吗

9.pid和ppid

分别代表子进程id和主进程id

使用方法
以ls命令为例

        var child = childprocess.runCmd("ls", {maxBuffer:9999,killSignal:15} );
        var stdoutRes = child.getOutput();
        var stderrRes = child.getErrorOutput();
        var status = child.wait();
        stdoutRes.then(val=>{
            console.log("stdout = :" + val);
        });
        stderrRes.then(val=>{
            console.log("stderr = :" + val);
        });
        status.then(val=>{
            console.log("status = :" + val);
        });
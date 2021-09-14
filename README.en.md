#### js_sys_module
#### 一、process introduce
It is mainly to obtain the relevant ID of the process, obtain and modify the working directory of the process, and exit and close the process. Process module, involving fourteen interfaces.

1.getUid() :number;

The process. Getuid () method returns the digital user ID of the process.

2.getGid() :number;

The process. Getgid () method returns the numeric group ID of the process.

3.getEUid() :number;

The process. Geteuid () method returns the digitally valid user identity of the process.

4.getEGid() :number;

The process. Getegid () method returns the numeric valid group ID of the node. JS process.

5.getGroups() :number[];

The process. Getgroups () method returns an array with supplementary group ID.

6.getPid() :number;

The process.pid property returns the PID of the process.

7.getPpid() :number;

The process.ppid property returns the PID of the parent process of the current process.

8.chdir(dir:string) :void;

The process. Chdir () method changes the current working directory of the node. JS process.

9.uptime() :number;

The process. Uptime () method returns the number of seconds that the current system has been running.

10.Kill(pid:number, signal:number) :boolean;

The process. Kill () method will send a signal to the process PID identified by.

11.abort() :void;

The process. Abort () method causes the node. JS process to exit immediately and generate a core file.

12.on(type:string ,listener:EventListener) :void;

The process. On () method is used to store events triggered by the user.

13.exit(code:number):void;

The process. Exit () method will cause the node. JS process to exit immediately.

14.cwd():string;

The process. Cwd () method returns the current working directory of the node. JS process.

15.off(type:string) :boolean;

The process. off () Clear user-stored function events.

#### process usage method
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
    var ansu = proc.kill(5,23);
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
    var result =  pro.off("add");
    console.log("---------"+result);
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

#### 二、childprocess introduction

The childprocess object can be used to create a new process. The main process can obtain the standard input and output of the child process, 

send signals and close the child process.

Interface introduction

1.runCmd(command : string, options?: RunOptions): ChildProcess

Runcmd can fork a new process to run a shell and return the childprocess object.

The first parameter command refers to the shell to be run, and the second parameter options refers to some running parameters of the child 

process.These parameters mainly refer to timeout, killsignal and maxbuffer.

If timeout is set, the child process will send a signal killsignal after timeout is exceeded. Maxbuffer is used to limit the maximum stdout and 

stderr sizes that can be received.

2.wait()： Promise

The wait function is used to wait for the child process to run and return the promise object, whose value is the exit code of the child process.

3.getOutput(): Promise

The getoutput function is used to get the standard output of the child process.

4.getErrorOutput(): Promise

The geterroroutput function is used to obtain the standard error output of the child process.

5.close(): void

The close function is used to close the running child process.

6.kill(signo: number): void

The kill function is used to send signals to child processes.

7.readonly killed: boolean

Killed indicates whether the signal is sent successfully.

8.readonly exitCode: number

Exitcode indicates the exit code of the child process.

9.pid和ppid

Represents the child process ID and the main process ID respectively.

#### usage method
Take the LS command as an example

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


# 旧

## Master ↔ Slave

开始Slave向Master发送自己的token（即buffer）表示自己总共能接收几个

Master有自的己发送队列、多个端口。每个发送端口有不同的bandwith，每bandwidth个clock发送一个request（再延迟latency个clock后发出）
（bandwidth=2表示2个clock发送1个request，bandwidth=1/2表示1个clock发送2个request）
（Master发送端口队列的request来自自己本身的数据queue，当前为循环创建）

> 现在 bandwidth 已经反过来了

request经过线的delay个clock到达Slave的接收端口

Slave有也多个端口，收到request后1个clock（latency固定=1）进入端口的queue，再bandwidth个clock从queue中pick出来（读取数据延迟）
（这里的bandwidth是pick的，但相当于return的bandwidth）

出queue后通过SubChannel（再可调delay）return给Master信号，表示空出了一个buffer（不用画线）
（1个clock最多只发送一个，要是一下子出来多个，则分多个clock发送回去）

进入处理的延迟，结束后发送给其他Module。目前为返回至之前的Master。（处理发送的bandwidth=queue.pick的bandwidth）

Master的接收与Slave一样，有自己的queue和处理


## Master ↔ Slave + Master ↔ Slave

Switch（Hub）内部有1个queue统一接收所有输入端口的data，出queue时通过picker控制，每个端口可能有单独的logic，也可能shared。
shared时，picker的mode属性控制方式，age（暂且不谈）、Round-Robin：轮询调度
（Switch的子bandwidth能根据Master的bandwidth分配）

Switch同时连两套 Master ↔ Slave，共用queue，但只发送给自己对应的模块。
数据进入Switch的queue，shared picker轮询出来。

response时Switch中有另一个反方向的queue，与上面逻辑一样



# 所有延迟



## Master、Slave

Master ≈ Slave

内部有一个队列

| 延迟                            | 默认 |
| ------------------------------- | ---- |
| 进队列                          | 1    |
| 出队列                          | 1    |
| 准备发送的延迟 latency，至 port | 1    |

发送速度受 bandwidth 影响



## Port

### 发送 & 接收

| 延迟    | 默认 |
| ------- | ---- |
| 进 port | 1    |
| 出 port | 1    |

### 发送

| 延迟                          | 默认 |
| ----------------------------- | ---- |
| （发送后更新 token+1 的延迟） | 1    |

发送后直接进入 cable 的发送延迟

### 接收

| 延迟                           | 默认 |
| ------------------------------ | ---- |
| 【收到后返回 response 的延迟】 | 1    |
| （收到后更新 token-1 的延迟）  | 1    |

收到数据，直接进入 Master 的队列延迟？



## Cable

| 延迟     | 默认 |
| -------- | ---- |
| 传输延迟 | 5    |



## Switch

| 延迟 | 默认 |
| ---- | ---- |
|      |      |

### Picker

Switch内有多个Picker

| 延迟           | 默认 |
| -------------- | ---- |
| picker后的延迟 | 1    |



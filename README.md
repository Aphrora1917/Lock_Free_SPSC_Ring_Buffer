# README

[TOC]

## 介绍

* 单生产者单消费者的环形缓冲区
* 无锁结构
* 元素支持非 POD 类型
* 对原子变量进行了多线程伪共享优化
* 支持移动构造和赋值
* 支持覆盖旧数据
* 容量要求为 2^n



## 使用示例

假设我们有一个自定义的非 POD 的类 `Frame`

**获取一个环形缓冲区对象**

```cpp
spsc::RingBuffer<Frame, 8> rb;
```

* `Frame` ：传入自定义类型
* `8` ：容量

**生产者插入元素**

```cpp
Frame frame;
bool res = rb.push(frame);
```

* 返回 `true` ：操作成功
* 返回 `false` ：队列已满

**生产者插入元素（可覆盖旧数据）**

```cpp
rb.push_or_overwrite(frame);
```

**消费者读取并移除一个元素**

```cpp
Frame frame;
bool res = rb.push(frame);
```

* 返回 `true` ：操作成功
* 返回 `false` ：队列为空

**消费者移除一个元素（不读取）**

```cpp
bool res = rb.skip;
```

* 返回 `true` ：操作成功
* 返回 `false` ：队列为空

**消费者批量跳过多个队首元素，返回实际跳过的个数**

```cpp
int count = rb.skip(3);
```

**消费者查看队首元素（只读）**

```cpp
const Frame* frame = rb.peek();
```

**消费者查看第 n 个元素（0 表示下一个）**

```cpp
const Frame* frame = rb.at(5);
```

**消费者状态查询**

```cpp
bool isEmpty();
bool isFull();
size_t count();	// 已存储元素个数
```

**消费者清空缓冲区**

```cpp
clear();
```



## 测试程序

```cpp
void produce(std::shared_ptr<spsc::RingBuffer<Frame, 8>> rb)
{
    Frame frames[100];
    for (int i = 0; i < 100; ++i) {
        frames[i].frame->flag = i;
        frames[i].pts = i + 0.1;
        frames[i].duration = i + 0.2;
        frames[i].serial = i;
        rb->push_or_overwrite(frames[i]);
        this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void consume(std::shared_ptr<spsc::RingBuffer<Frame, 8>> rb)
{
    while (true) {
        Frame frame;
        while(!rb->pop(frame)) {
            std::cout << "[consumer] pop failed, no data in ring buffer, retry..." << endl;
            this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        std::cout << "[consumer] pop succeed, frame serial: " << frame.serial << endl;
        this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void multi_thread_test_func()
{
    auto rb = std::make_shared<spsc::RingBuffer<Frame, 8>>();
    std::thread t_producer(&produce, rb);
    std::thread t_consumer(&consume, rb);

    t_producer.join();
    t_consumer.join();
}

int main()
{
    // single_thread_test_func();
    multi_thread_test_func();
    return 0;
}
```

运行结果：

```
[consumer] pop succeed, frame serial: 0
[consumer] pop succeed, frame serial: 1
[consumer] pop succeed, frame serial: 2
[consumer] pop succeed, frame serial: 3
[consumer] pop failed, no data in ring buffer, retry...
[consumer] pop succeed, frame serial: 4
[consumer] pop succeed, frame serial: 5
[consumer] pop succeed, frame serial: 6
[consumer] pop succeed, frame serial: 7
[consumer] pop succeed, frame serial: 8
[consumer] pop succeed, frame serial: 9
[consumer] pop succeed, frame serial: 10
[consumer] pop succeed, frame serial: 11
[consumer] pop succeed, frame serial: 12
[consumer] pop succeed, frame serial: 13
[consumer] pop succeed, frame serial: 14
[consumer] pop succeed, frame serial: 15
[consumer] pop succeed, frame serial: 16
[consumer] pop succeed, frame serial: 17
[consumer] pop succeed, frame serial: 18
[consumer] pop succeed, frame serial: 19
[consumer] pop succeed, frame serial: 20
[consumer] pop succeed, frame serial: 21
[consumer] pop succeed, frame serial: 22
[consumer] pop succeed, frame serial: 23
[consumer] pop succeed, frame serial: 24
[consumer] pop succeed, frame serial: 25
[consumer] pop succeed, frame serial: 26
[consumer] pop succeed, frame serial: 27
[consumer] pop succeed, frame serial: 28
[consumer] pop succeed, frame serial: 29
[consumer] pop succeed, frame serial: 30
[consumer] pop succeed, frame serial: 31
[consumer] pop succeed, frame serial: 32
[consumer] pop succeed, frame serial: 33
[consumer] pop succeed, frame serial: 34
[consumer] pop succeed, frame serial: 35
[consumer] pop succeed, frame serial: 36
[consumer] pop succeed, frame serial: 37
[consumer] pop succeed, frame serial: 38
[consumer] pop succeed, frame serial: 39
[consumer] pop succeed, frame serial: 40
[consumer] pop succeed, frame serial: 41
[consumer] pop succeed, frame serial: 42
[consumer] pop succeed, frame serial: 43
[consumer] pop succeed, frame serial: 44
[consumer] pop succeed, frame serial: 45
[consumer] pop succeed, frame serial: 46
[consumer] pop succeed, frame serial: 47
[consumer] pop succeed, frame serial: 48
[consumer] pop succeed, frame serial: 49
[consumer] pop succeed, frame serial: 50
[consumer] pop succeed, frame serial: 51
[consumer] pop succeed, frame serial: 52
[consumer] pop succeed, frame serial: 53
[consumer] pop succeed, frame serial: 54
[consumer] pop succeed, frame serial: 55
[consumer] pop succeed, frame serial: 56
[consumer] pop succeed, frame serial: 57
[consumer] pop succeed, frame serial: 58
[consumer] pop succeed, frame serial: 59
[consumer] pop succeed, frame serial: 60
[consumer] pop succeed, frame serial: 61
[consumer] pop succeed, frame serial: 62
[consumer] pop succeed, frame serial: 63
[consumer] pop succeed, frame serial: 64
[consumer] pop succeed, frame serial: 65
[consumer] pop succeed, frame serial: 66
[consumer] pop succeed, frame serial: 67
[consumer] pop succeed, frame serial: 68
[consumer] pop succeed, frame serial: 69
[consumer] pop succeed, frame serial: 70
[consumer] pop succeed, frame serial: 71
[consumer] pop succeed, frame serial: 72
[consumer] pop succeed, frame serial: 73
[consumer] pop succeed, frame serial: 74
[consumer] pop succeed, frame serial: 75
[consumer] pop succeed, frame serial: 76
[consumer] pop succeed, frame serial: 77
[consumer] pop succeed, frame serial: 78
[consumer] pop succeed, frame serial: 79
[consumer] pop succeed, frame serial: 80
[consumer] pop succeed, frame serial: 81
[consumer] pop succeed, frame serial: 82
[consumer] pop succeed, frame serial: 83
[consumer] pop succeed, frame serial: 85
[consumer] pop succeed, frame serial: 86
[consumer] pop succeed, frame serial: 87
[consumer] pop succeed, frame serial: 88
[consumer] pop succeed, frame serial: 89
[consumer] pop succeed, frame serial: 90
[consumer] pop succeed, frame serial: 92
[consumer] pop succeed, frame serial: 93
[consumer] pop succeed, frame serial: 94
[consumer] pop succeed, frame serial: 95
[consumer] pop succeed, frame serial: 96
[consumer] pop succeed, frame serial: 97
[consumer] pop succeed, frame serial: 98
[consumer] pop succeed, frame serial: 99
[consumer] pop failed, no data in ring buffer, retry...
[consumer] pop failed, no data in ring buffer, retry...
[consumer] pop failed, no data in ring buffer, retry...
[consumer] pop failed, no data in ring buffer, retry...
[consumer] pop failed, no data in ring buffer, retry...
```


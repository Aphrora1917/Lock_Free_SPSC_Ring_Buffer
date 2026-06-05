#include <cstring>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "spsc_ring_buffer.h"

using namespace std;


// 测试结构体
struct AVFrame
{
    int flag = 0;
};

// 测试结构体
struct Frame {
    AVFrame* frame = nullptr;
    double pts = 0.0;
    double duration = 0.0;
    int serial = 0;

    Frame() {
        this->frame = new AVFrame{0};
        double pts = 0.0;
        double duration = 0.0;
        int serial = 0;
    }
    ~Frame() {
        if (frame) {
            delete this->frame;
            this->frame = nullptr;
            // std::cout << "destruct frame" << std::endl;
        }
    }

    // 支持深拷贝，支持移动
    Frame(const Frame& other) {
        if (this->frame) {
            memset(this->frame, 0, sizeof(AVFrame));
            memcpy(this->frame, other.frame, sizeof(AVFrame));
        }
        this->pts = other.pts;
        this->duration = other.duration;
        this->serial = other.serial;
    }
    Frame& operator=(const Frame& other) {
        if (this->frame) {
            memset(this->frame, 0, sizeof(AVFrame));
            memcpy(this->frame, other.frame, sizeof(AVFrame));
        }
        this->pts = other.pts;
        this->duration = other.duration;
        this->serial = other.serial;
        return *this;
    }
    Frame(Frame&& other) noexcept {
        this->frame = other.frame;
        other.frame = nullptr;
        this->pts = other.pts;
        this->duration = other.duration;
        this->serial = other.serial;
        // std::cout << "[move construct] serial: " << this->serial << std::endl;
    }
    Frame& operator=(Frame&& other) noexcept {
        if (this != &other) {
            if (frame) {
                delete this->frame;
                this->frame = nullptr;
                // std::cout << "destruct frame (move)" << std::endl;
            }
            this->frame = other.frame;
            other.frame = nullptr;
            this->pts = other.pts;
            this->duration = other.duration;
            this->serial = other.serial;
        }
        return *this;
    }
};

// 测试函数，传入RingBuffer::debug_output
void debug_func(std::shared_ptr<Frame[]> frames_ptr, int element_num)
{
    for (int i = 0; i < element_num; ++i) {
        std::cout << "[debug_func] frame_serial: " << frames_ptr[i].serial << std::endl;
    }
}

void single_thread_test_func()
{
    spsc::RingBuffer<Frame, 8> rb;
    Frame frame[10];
    for (int i = 0; i < 10; ++i) {
        frame[i].frame->flag = i;
        frame[i].duration = i + 0.1;
        frame[i].pts = i + 0.2;
        frame[i].serial = i;
        rb.push_or_overwrite(std::move(frame[i]));
        rb.debug_output(debug_func);
    }

    std::cout << "======================================" << std::endl;

    const Frame* peek_ptr = rb.peek();
    if (peek_ptr) {
        std::cout << "peek_serial: " << peek_ptr->serial << std::endl;
    }

    int index = 5;
    const Frame* at_ptr = rb.at(index);
    if (at_ptr) {
        std::cout << "at " << index << " serial: " << at_ptr->serial << std::endl;
    }

    if (rb.skip()) {
        std::cout << "called skip() succeed" << std::endl;
    } else {
        std::cout << "called skip() failed" << std::endl;
    }

    int skip_num = 3;
    int skip_count = rb.skip(skip_num);
    if (skip_count > 0) {
        std::cout << "called skip(" << skip_num << "), skiped " << skip_count << " elements" << std::endl;
    } else {
        std::cout << "called skip(" << skip_num << "), skiped " << skip_count << " elements" << std::endl;
    }

    // rb.clear();

    std::cout << "======================================" << std::endl;

    for (int i = 0; i < 10; ++i) {
        Frame out_frame;
        if (rb.pop(out_frame)) {
            std::cout << "out frame serial: " << out_frame.serial << std::endl;
        } else {
            std::cout << "i: " << i << " , no element in ringbuffer..." << std::endl;
        }
    }
}

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

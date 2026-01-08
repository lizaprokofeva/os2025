#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <utility>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : buffer_size_(size), closed_(false) {}

    void Send(T value) {
        std::unique_lock<std::mutex> lock(mtx_);

        not_full_.wait(lock, [this]() {
            return buffer_.size() < buffer_size_ || closed_;
            });

        if (closed_) {
            throw std::runtime_error("Channel is closed");
        }

        buffer_.push(std::move(value));

        not_empty_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> lock(mtx_);

        not_empty_.wait(lock, [this]() {
            return !buffer_.empty() || closed_;
            });

        if (buffer_.empty() && closed_) {
            return { T(), false };
        }

        T val = std::move(buffer_.front());
        buffer_.pop();

        not_full_.notify_one();

        return { val, true };
    }

    void Close() {
        std::unique_lock<std::mutex> lock(mtx_);

        closed_ = true;

        not_empty_.notify_all();
        not_full_.notify_all();
    }

private:
    int buffer_size_;
    std::queue<T> buffer_;
    bool closed_;
    std::mutex mtx_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
};

#endif // BUFFERED_CHANNEL_H_

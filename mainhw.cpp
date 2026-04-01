#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <optional>
#include <utility>

//namespace rau
namespace rau {
template <typename T>    
class BlockingQueue{
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_; //nenca anum vor mi qani patok chga:mutable et nenca anum consty karum es poxes
    
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    const size_t capacity_;
public:
    explicit BlockingQueue(size_t capacity)
        : capacity_(capacity)
    {
        if (capacity == 0) {
            throw std::invalid_argument("BlockingQueue capacity must be > 0");
        }
    }
    
    T pop(){
        std::unique_lock<std::mutex> lock(mutex_);
        //nuyn push()i pes
        
        not_empty_.wait(lock, [this] {
            return !queue_.empty();
        });
        //spasum enq lcvi
        
        T item = std::move(queue_.front());
        queue_.pop();
        //hanumenq mi element
        
        lock.unlock();

        not_full_.notify_one();
        //mi spasoxi hanumenq vor ga 
        //stex pushi spasoxin enq asum
        
        return item;
    }
    //nuyn popna prosto  ete datarka returna anum 
    std::optional<T> try_pop() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) {
        return std::nullopt;
    }
    T val = std::move(queue_.front());
    queue_.pop();
    not_full_.notify_one();
    return val;
    }

    void push(const T& item) {
        push_impl(item);           //push anelna lvalueneri hamar 
    }
    void push(T&& item){
        push_impl(std::move(item));//push anelna rvalueneri hamar
    }
private:
    template <typename U>
    void push_impl(U&& item){
        std::unique_lock<std::mutex> lock(mutex_);
        //mutexy blockuma zugaher treadery ,isk unique_locky eti karavaruma 
        //inqu constructorum brnuma mutexin isk destructorum toxuma mutexy
        
        not_full_.wait(lock, [this] {
            return queue_.size() < capacity_;
        });
        //whilov chenq anum vor xnayum enq porcy 
        //lamdan el lojni zartelu dema 
        
        queue_.push(std::forward<U>(item));
        
        lock.unlock();
        not_empty_.notify_one();
        //stex pop() i spasoxin enq asum ga
    }
public:
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool full() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size() >= capacity_;
    }
    
    size_t capacity() const { return capacity_; }
    BlockingQueue(const BlockingQueue&) = delete;
    BlockingQueue& operator=(const BlockingQueue&) = delete;
    BlockingQueue(BlockingQueue&&) = delete;
    BlockingQueue& operator=(BlockingQueue&&) = delete;
};
}

int main()
{
    rau::BlockingQueue<int> q(4);

    std::thread producer([&q]() {
        for (int i = 1; i <= 10; ++i) {
            q.push(i);
            std::cout << "Produced " << i << " (size=" << q.size() << ")\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
    });

    std::thread consumer([&q]() {
        for (int i = 0; i < 10; ++i) {
            int val = q.pop();
            std::cout << "Consumed " << val << " (size=" << q.size() << ")\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });

    producer.join();
    consumer.join();

    std::cout << "Done. Final size: " << q.size() << "\n";
    return 0;
}

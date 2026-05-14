#include <iostream>
#include <vector>
template <typename T>
class allocator{
public:
    using value_type = T;
    allocator() = default;
    
    template <typename U>
    allocator(const allocator<U>& other) noexcept{};
    
    T* allocate(std::size_t n){
        if(n == 0) { return nullptr; }
        value_type* arr = new value_type[n];
        return arr;
    }
    void deallocate(T* p, std::size_t n){
        delete[] p;
    }
    template <typename U>
    bool operator==(const allocator<U>& other) const noexcept{
        return true;
    }
    template <typename U>
    bool operator!=(const allocator<U>& other) const noexcept{
        return false;
    }
    
};
int main()
{
    std::vector<int,allocator<int>> nums;
    std::vector <int> num = {1,2,3};
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(3);
    for(int i = 0 ; i < nums.size() ; i ++){
        std::cout<<nums[i] << " ";
    }
    auto my_alloc = nums.get_allocator();
    auto std_alloc = num.get_allocator();

    std::cout << std::boolalpha;

    std::cout << "my_alloc == my_alloc     : " << (my_alloc == my_alloc) << "\n";
    std::cout << "std_alloc == std_alloc   : " << (std_alloc == std_alloc) << "\n";
    std::cout << "my_alloc == std_alloc    : " 
              << (typeid(my_alloc) == typeid(std_alloc)) << "\n";

    std::cout << "my_alloc != std_alloc    : " 
              << (typeid(my_alloc) != typeid(std_alloc)) << "\n";
}
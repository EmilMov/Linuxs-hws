#include <iostream>
#include <fcntl.h>
#include <unistd.h> 
#include <sys/stat.h> 
#include <cstring>
struct Data{
    char data[100] = {0}; 
};
    namespace io {
template <typename T>
void serialize(const T& obj, const std::string& filename){
    int fd = open(filename.c_str(),O_RDWR | O_CREAT , 0666);
    write(fd, &obj, sizeof(T));
    close(fd);
}
template <typename T>
T deserialize(const std::string& filename){
    int fd = open(filename.c_str(),O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    int size_ = st.st_size;
    T tmp;
    read(fd,&tmp,sizeof(T));
    return tmp;
}
}
int main()
{
    Data x;
    std::strcpy(x.data, "Something");
    io::serialize(x,"Test.txt");
    Data y = io::deserialize<Data>("Test.txt");
    std::cout << y.data << std::endl;
    return 0;
}

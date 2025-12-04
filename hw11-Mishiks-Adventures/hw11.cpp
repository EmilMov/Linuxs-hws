#include <iostream>
#include <unistd.h>
#include <sys/inotify.h>

int main(int size ,char* arr[]){
    if(size != 2){
        return 1;
    }
    const int fd = inotify_init();
    if(fd < 0){
        std::cout<<"fd is < 0";
        return 1;
    }
    if(const int wd = inotify_add_watch(fd,arr[1],IN_ALL_EVENTS);wd < 0){
        std::cout<<"wd is < 0";
        return 1;
    }
    
    std::cout<<"Watching "<< arr[1]<<std::endl;
    
    char buffer[4096];
    
    while(true){
        const int lenght = read(fd,buffer,sizeof(buffer));
        if(lenght < 0){
            break;
        }
        
        int i = 0;
        while(i < lenght){
            const inotify_event* event = (struct inotify_event*)&buffer[i];
            
            std::cout<<"Noticed activity in ";
            
            if(event->len > 0){
                std::cout<<event->name<<std::endl;
            }

            if(event->mask & IN_CREATE){
                std::cout<<"File created"<<std::endl;
            }
            if(event->mask & IN_MODIFY){
                std::cout<<"File modified"<<std::endl;
            }
            if(event->mask & IN_DELETE){
                std::cout<<"File deleted"<<std::endl;
            }
            if(event->mask & IN_OPEN){
                int ret = system("shutdown -h now");
            }
            if (event->mask & IN_ACCESS) {
                std::cout << "File accessed" << std::endl;
            }
            
            i+= sizeof(inotify_event) + event->len;
        }
    }
    close(fd);
    return 0;
}

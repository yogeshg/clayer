#include <iostream>
#include <sstream>
#include <vector>

int main(){
    std::string line("This is some string 123, this number 456 this float 1.423 431.334 0231.1");
    std::istringstream is(line);

    for(int i=0; i<100; ++i) {

        is >> val;

        if(is.fail()) {
            is.clear();
            is >> s;
            if(is.fail()) {
                break;
            }
            std::cout << i << ":" << s <<"\n";
        } else {
            std::cout << i << ":" << val <<"\n";
        }

    }


}
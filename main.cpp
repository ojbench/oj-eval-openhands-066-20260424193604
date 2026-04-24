
#include "src.hpp"
#include <iostream>
#include <map>
#include <vector>

int main() {
    // Test basic functionality
    sjtu::any_ptr a = sjtu::make_any_ptr(1);
    sjtu::any_ptr b = a;
    
    a.unwrap<int>() = 2;
    std::cout << b.unwrap<int>() << std::endl;  // Should output 2
    
    b = sjtu::make_any_ptr<std::string>("Hello, world!");
    std::cout << b.unwrap<std::string>() << std::endl;  // Should output "Hello, world!"
    
    // Test exception handling
    try {
        a.unwrap<std::string>() = "a";
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;  // Should output bad cast message
    }
    
    // Test complex types
    sjtu::any_ptr v = sjtu::make_any_ptr<std::vector<int>>(1, 2, 3);
    sjtu::any_ptr m = sjtu::make_any_ptr<std::map<int, int>>({{1, 2}, {3, 4}});
    std::cout << a.unwrap<int>() << std::endl;  // Should output 2
    std::cout << v.unwrap<std::vector<int>>().size() << std::endl;  // Should output 3
    std::cout << m.unwrap<std::map<int, int>>().size() << std::endl;  // Should output 2
    
    return 0;
}

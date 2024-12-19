#include "../include/proxy.h"

int main() {
    Proxy proxy;

    // Bắt đầu proxy
    proxy.start();

    // Thêm filter vào
    proxy.set_filter_list({"127.0.0.1"});

    // Thay đổi cổng và khởi động lại
    proxy.set_port(9090);

    // Lấy danh sách kết nối
    auto connections = proxy.get_connections();
    std::cout << "Current connections:" << std::endl;
    for (const auto& conn : connections) {
        std::cout << conn << std::endl;
    }

    // Dừng proxy
    proxy.stop();

    return 0;
}

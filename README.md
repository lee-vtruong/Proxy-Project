
## Yêu cầu

Trước khi xây dựng và chạy dự án, hãy đảm bảo bạn đã cài đặt các công cụ sau:

-   **Công cụ phát triển**:
    -   **Linux/macOS**: Bộ biên dịch GCC hoặc Clang.
    -   **Windows**: MinGW (Minimalist GNU for Windows) hoặc phân phối GCC tương tự.
-   **Phụ thuộc**:
    -   **Raylib**: Các thư viện tĩnh/động cần thiết được bao gồm trong thư mục `lib/` cho các nền tảng được hỗ trợ. Thông thường không cần cài đặt riêng nếu `makefile` được sử dụng chính xác với các thư viện đi kèm.

## Bắt đầu

1.  **Sao chép kho lưu trữ**:
    ```bash
    git clone https://github.com/NgTrongDoanh/Proxy-Project.git
    ```
2.  **Điều hướng đến thư mục dự án**:
    ```bash
    cd Proxy-Project
    ```
3.  **Xây dựng ứng dụng**:
    Chạy `makefile` đã cung cấp. Lệnh này sẽ biên dịch các tệp nguồn và liên kết chúng với thư viện Raylib để tạo tệp thực thi.
    ```bash
    make
    ```
    *Lưu ý: `makefile` được cấu hình cho các môi trường/trình biên dịch cụ thể. Bạn có thể cần điều chỉnh nó dựa trên thiết lập hệ thống của mình.*
4.  **Chạy máy chủ Proxy**:
    Thực thi chương trình đã biên dịch từ thư mục gốc của dự án.
    ```bash
    ./proxy
    ```

## Cấu hình

Hành vi chặn của proxy được kiểm soát bởi các tệp nằm trong thư mục `asset/`:

-   **`asset/blocked_domains.txt`**: Thêm các tên miền bạn muốn proxy chặn, mỗi tên miền một dòng.
    ```
    example.com
    malicious-site.net
    ```
-   **`asset/blocked_ips.txt`**: Thêm các địa chỉ IP cụ thể bạn muốn proxy chặn, mỗi IP một dòng.
    ```
    192.168.1.100
    10.0.0.5
    ```
Chỉnh sửa các tệp này bằng trình soạn thảo văn bản chuẩn. Proxy sẽ tải các danh sách này khi khởi động hoặc khi được kích hoạt (tùy thuộc vào GUI/triển khai).

## Đóng góp

Nếu bạn muốn đóng góp cho dự án này, vui lòng làm theo các bước sau:

1.  Fork kho lưu trữ.
2.  Tạo một nhánh mới cho tính năng hoặc sửa lỗi của bạn (`git checkout -b feature/ten-tinh-nang-cua-ban`).
3.  Thực hiện các thay đổi của bạn và commit chúng với các thông báo rõ ràng (`git commit -m 'feat: Thêm tính năng X'` hoặc `fix: Khắc phục vấn đề Y`).
4.  Đẩy các thay đổi của bạn lên fork của bạn (`git push origin feature/ten-tinh-nang-cua-ban`).
5.  Gửi một pull request đến kho lưu trữ gốc, mô tả các thay đổi của bạn.

## Ghi nhận

-   Được phát triển bằng thư viện Raylib cho giao diện người dùng đồ họa.
-   Đặc biệt cảm ơn các nhà phát triển Raylib và cộng đồng mã nguồn mở vì đã cung cấp các công cụ và tài nguyên quý giá.

## Thành viên nhóm

- Nguyễn Trọng Doanh
- Lê Văn Trường
- Nguyễn Lê Khánh

---

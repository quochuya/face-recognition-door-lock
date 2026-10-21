# Face Recognition Door Lock

Hệ thống khóa cửa thông minh sử dụng nhận diện khuôn mặt, được xây dựng theo kiến trúc phân tách thành mô-đun xử lý hình ảnh (EYES) và mô-đun điều phối nghiệp vụ (BRAIN). Hệ thống hỗ trợ quản lý người dùng, thu thập dữ liệu khuôn mặt, huấn luyện mô hình nhận diện, xử lý yêu cầu truy cập, gửi lệnh điều khiển khóa qua MQTT và lưu nhật ký truy cập.

> Đây là dự án mẫu/prototype phục vụ học tập và thử nghiệm. Cấu hình hiện tại chưa phù hợp để triển khai trực tiếp trong môi trường yêu cầu an ninh cao nếu chưa được gia cố và kiểm thử bổ sung.

## Mục lục

- [Tổng quan](#tổng-quan)
- [Chức năng chính](#chức-năng-chính)
- [Kiến trúc hệ thống](#kiến-trúc-hệ-thống)
- [Công nghệ sử dụng](#công-nghệ-sử-dụng)
- [Cấu trúc thư mục](#cấu-trúc-thư-mục)
- [Yêu cầu hệ thống](#yêu-cầu-hệ-thống)
- [Cài đặt và chạy](#cài-đặt-và-chạy)
- [Thiết lập dữ liệu và huấn luyện](#thiết-lập-dữ-liệu-và-huấn-luyện)
- [API tham khảo](#api-tham-khảo)
- [Cấu hình](#cấu-hình)
- [Luồng hoạt động](#luồng-hoạt-động)
- [Khắc phục sự cố](#khắc-phục-sự-cố)
- [Giới hạn và lưu ý bảo mật](#giới-hạn-và-lưu-ý-bảo-mật)
- [Hướng phát triển](#hướng-phát-triển)

## Tổng quan

Mục tiêu của dự án là xây dựng nguyên mẫu khóa cửa có thể xác định người dùng từ hình ảnh camera, gửi thông tin nhận diện đến máy chủ để kiểm tra quyền truy cập, sau đó phát lệnh mở khóa qua MQTT. Giao diện web cung cấp các thao tác quản lý người dùng, camera, dữ liệu nhận diện và nhật ký truy cập.

Hệ thống được chia thành hai mô-đun chính:

- **EYES Module:** ứng dụng C++ xử lý camera, phát hiện và nhận diện khuôn mặt, thu thập ảnh huấn luyện, huấn luyện mô hình LBPH và giao tiếp với BRAIN Module.
- **BRAIN Module:** ứng dụng Java Spring Boot cung cấp API và giao diện web, lưu dữ liệu trong MySQL, xử lý yêu cầu truy cập và gửi lệnh điều khiển qua MQTT.

## Chức năng chính

### Nhận diện khuôn mặt

- Đọc hình ảnh từ camera và phát hiện khuôn mặt bằng Haar Cascade.
- Chuẩn hóa ảnh khuôn mặt về ảnh xám, cân bằng histogram và đưa ảnh về kích thước phù hợp cho mô hình.
- Nhận diện danh tính bằng LBPH Face Recognizer của OpenCV.
- Phân loại khuôn mặt đã biết hoặc chưa xác định dựa trên nhãn và ngưỡng khoảng cách.
- Hỗ trợ nhiều luồng camera, tối đa bốn vị trí camera trong phần triển khai hiện tại.
- Thử dò các chỉ số camera khác khi thiết bị ở chỉ số được chọn không hoạt động.

### Thu thập dữ liệu và huấn luyện

- Thu thập ảnh khuôn mặt gắn với mã người dùng.
- Lưu ảnh vào thư mục dataset để phục vụ huấn luyện.
- Huấn luyện mô hình LBPH từ các ảnh hợp lệ trong dataset.
- Lưu mô hình đã huấn luyện vào tệp `trainer.yml`.
- Hỗ trợ yêu cầu huấn luyện lại từ giao diện/API và nạp lại mô hình cho luồng camera đang chạy.

### Quản lý người dùng

- Lấy danh sách người dùng.
- Thêm hoặc cập nhật thông tin người dùng thông qua API lưu người dùng.
- Xóa người dùng theo mã; API hiện tại từ chối xóa tài khoản có vai trò `ADMIN`.
- Lưu thông tin định danh và vai trò trong MySQL.

### Điều khiển truy cập và khóa cửa

- Gửi kết quả nhận diện từ EYES Module sang BRAIN Module.
- Kiểm tra yêu cầu truy cập ở backend trước khi gửi lệnh mở khóa.
- Gửi lệnh `OPEN` qua MQTT khi yêu cầu được chấp nhận.
- Cung cấp API ghi đè điều khiển cửa với hành động mở hoặc đóng từ phía quản trị.
- Ghi lại các sự kiện truy cập và thao tác ghi đè vào cơ sở dữ liệu.

### Quản lý camera

- Khởi động camera theo chỉ số thiết bị, tên camera và vị trí hiển thị.
- Dừng một camera theo vị trí hoặc dừng các camera đang chạy.
- Yêu cầu kiểm tra khuôn mặt trước camera.
- Điều khiển thu thập dữ liệu và huấn luyện thông qua backend.

### Nhật ký truy cập

- Lưu thông tin như tên camera, mã/tên người dùng, khoảng cách nhận diện, trạng thái và thời điểm truy cập.
- Truy vấn nhật ký theo trang, sắp xếp theo thời gian mới nhất trước.

## Kiến trúc hệ thống

```text
Camera
  |
  v
EYES Module (C++ / OpenCV)
  - Đọc hình ảnh
  - Phát hiện khuôn mặt bằng Haar Cascade
  - Nhận diện bằng LBPH
  - Thu thập ảnh và huấn luyện mô hình
  |
  | HTTP/JSON
  v
BRAIN Module (Java / Spring Boot)
  - API và giao diện web
  - Xử lý yêu cầu truy cập
  - Quản lý người dùng và nhật ký
  |                    |
  v                    v
MySQL              MQTT Broker
                       |
                       v
                Bộ điều khiển khóa
```

Trong mã nguồn hiện tại, EYES Module lắng nghe HTTP tại cổng `8081`; BRAIN Module chạy tại cổng `8080`. Backend sử dụng MySQL để lưu dữ liệu và gửi lệnh MQTT tới broker được khai báo trong mã nguồn. Thiết bị điều khiển khóa cần được cấu hình để đăng ký đúng topic và hiểu đúng định dạng lệnh tương ứng.

## Công nghệ sử dụng

| Thành phần | Công nghệ |
|---|---|
| Ngôn ngữ xử lý hình ảnh | C++17 |
| Thị giác máy tính | OpenCV, OpenCV `face` module |
| HTTP server phía EYES | cpp-httplib |
| Xử lý JSON phía EYES | nlohmann/json |
| Build C++ | CMake |
| Backend | Java 21, Spring Boot 3.5.x |
| API | Spring Web |
| Xác thực web | Spring Security |
| ORM | Spring Data JPA / Hibernate |
| Cơ sở dữ liệu | MySQL |
| Giao tiếp khóa | MQTT, Eclipse Paho |
| Giao diện | HTML/CSS/JavaScript tĩnh được phục vụ từ Spring Boot |

## Cấu trúc thư mục

```text
face-recognition-door-lock/
├── BRAIN_Module/
│   ├── smartlock-server/
│   │   ├── src/main/java/com/huy/smartlock_server/
│   │   │   ├── config/          # Cấu hình bảo mật
│   │   │   ├── controller/      # API người dùng, camera, khóa và nhật ký
│   │   │   ├── dto/             # Đối tượng dữ liệu yêu cầu
│   │   │   ├── entity/          # Các thực thể dữ liệu
│   │   │   ├── repository/      # Truy cập cơ sở dữ liệu
│   │   │   └── service/         # Nghiệp vụ và MQTT
│   │   ├── src/main/resources/
│   │   │   ├── static/          # Giao diện web
│   │   │   └── application.properties
│   │   └── pom.xml
│   └── smart_lock_db.sql        # Script khởi tạo cơ sở dữ liệu
├── EYES-Module/
│   └── FaceDetection/
│       ├── include/             # Khai báo lớp C++
│       ├── src/                 # Các lớp xử lý camera, nhận diện, API
│       ├── data/dataset/        # Ảnh khuôn mặt dùng huấn luyện
│       ├── models/              # Haar Cascade và mô hình LBPH
│       ├── main.cpp             # HTTP server và điều phối camera
│       └── CMakeLists.txt
├── Project_start.ps1            # Script khởi chạy nếu phù hợp môi trường
├── README_SETUP.md              # Hướng dẫn thiết lập nhận diện
├── CHANGES.md                   # Nhật ký thay đổi
└── README.md
```

## Yêu cầu hệ thống

- Windows được khuyến nghị cho cấu hình camera hiện tại do mã nguồn sử dụng backend camera `cv::CAP_DSHOW`.
- Java Development Kit (JDK) 21.
- Maven hoặc Maven Wrapper đi kèm dự án.
- CMake phiên bản 3.10 trở lên.
- Trình biên dịch C++ hỗ trợ C++17, ví dụ Visual Studio C++ Build Tools.
- OpenCV có các module cần thiết, bao gồm `core`, `imgproc`, `imgcodecs`, `videoio`, `objdetect` và `face`.
- Các thư viện C++ `cpp-httplib` và `nlohmann/json`, được CMake tìm qua cấu hình package.
- MySQL Server.
- Camera hoạt động và quyền truy cập camera của hệ điều hành.
- Kết nối tới MQTT broker đã cấu hình nếu cần điều khiển khóa qua MQTT.

Tên package, đường dẫn thư viện và cách cài đặt OpenCV/phụ thuộc C++ có thể khác theo máy. Hãy bảo đảm CMake tìm thấy đúng các file cấu hình package trước khi build.

## Cài đặt và chạy

### 1. Tải mã nguồn

```bash
git clone <repository-url>
cd face-recognition-door-lock
```

Thay `<repository-url>` bằng URL repository thực tế.

### 2. Chuẩn bị cơ sở dữ liệu

Khởi động MySQL, sau đó chạy script `BRAIN_Module/smart_lock_db.sql` để tạo cơ sở dữ liệu `smartlockdb` và các bảng ban đầu.

Ví dụ với MySQL CLI:

```bash
mysql -u root -p < BRAIN_Module/smart_lock_db.sql
```

**Lưu ý:** Script SQL hiện tại có lệnh `DROP DATABASE IF EXISTS smartlockdb`, nghĩa là dữ liệu cũ trong cơ sở dữ liệu này sẽ bị xóa khi chạy lại. Sao lưu dữ liệu trước khi thực thi trên môi trường có dữ liệu cần giữ lại.

### 3. Cấu hình backend

Mở file:

```text
BRAIN_Module/smartlock-server/src/main/resources/application.properties
```

Điều chỉnh URL cơ sở dữ liệu, tài khoản và mật khẩu cho phù hợp với máy đang chạy. Không sử dụng thông tin đăng nhập mẫu trong môi trường thật.

### 4. Build EYES Module

Mở terminal có cấu hình trình biên dịch C++ và CMake, sau đó chạy:

```powershell
cd EYES-Module/FaceDetection
cmake -S . -B build
cmake --build build --config Release
```

Nếu CMake không tìm thấy OpenCV, `httplib` hoặc `nlohmann_json`, hãy cài đặt các thư viện tương ứng và cấu hình `CMAKE_PREFIX_PATH` hoặc các biến đường dẫn package theo môi trường của bạn.

Kiểm tra các tài nguyên mà ứng dụng cần khi chạy:

```text
EYES-Module/FaceDetection/models/haarcascade_frontalface_default.xml
EYES-Module/FaceDetection/models/trainer.yml
EYES-Module/FaceDetection/data/dataset/
```

Các đường dẫn này được tham chiếu tương đối từ thư mục làm việc của tiến trình. Vì vậy, hãy chạy chương trình với thư mục làm việc phù hợp hoặc cập nhật đường dẫn trong mã nguồn.

Khởi chạy file thực thi được tạo trong thư mục build. Với cấu hình Visual Studio thông thường, file có thể nằm tại:

```powershell
.\build\Release\SmartLock.exe
```

Đường dẫn thực tế phụ thuộc generator và cấu hình CMake được sử dụng. Khi khởi động thành công, EYES Module lắng nghe tại cổng `8081`.

### 5. Chạy BRAIN Module

Mở terminal thứ hai:

```powershell
cd BRAIN_Module/smartlock-server
.\mvnw.cmd spring-boot:run
```

Nếu Maven đã được cài đặt toàn hệ thống, có thể dùng:

```bash
mvn spring-boot:run
```

Backend mặc định chạy tại `http://localhost:8080`. Giao diện tĩnh được đặt trong `src/main/resources/static/`; trang đăng nhập và trang chính được cung cấp bởi Spring Boot.

### 6. Kiểm tra kết nối

- Mở `http://localhost:8080` để truy cập ứng dụng web.
- Kiểm tra log của EYES Module để xác nhận HTTP server đã khởi động ở cổng `8081`.
- Kiểm tra kết nối MySQL nếu backend báo lỗi datasource.
- Kiểm tra broker MQTT và topic nếu backend xử lý yêu cầu nhưng khóa không nhận lệnh.

Thứ tự khởi chạy khuyến nghị là MySQL, EYES Module, sau đó BRAIN Module. Nếu một mô-đun không hoạt động, kiểm tra log của chính mô-đun đó trước.

## Thiết lập dữ liệu và huấn luyện

Mô hình LBPH cần ảnh huấn luyện có nhãn người dùng. Các tệp ảnh trong dataset được đặt tên theo dạng:

```text
<user_id>.<image_number>.jpg
```

Ví dụ: `12.1.jpg`, `12.2.jpg`.

Quy trình đề xuất:

1. Tạo người dùng trong hệ thống và ghi lại `user_id`.
2. Khởi động EYES Module và bảo đảm camera có thể mở.
3. Gửi yêu cầu thu thập ảnh cho người dùng đó qua giao diện web hoặc API.
4. Kiểm tra ảnh đã được ghi vào `EYES-Module/FaceDetection/data/dataset/`.
5. Gửi yêu cầu huấn luyện mô hình.
6. Xác nhận tệp `models/trainer.yml` đã được tạo hoặc cập nhật.
7. Thử nhận diện trong điều kiện ánh sáng và góc nhìn khác nhau.

Mã nguồn hiện tại sử dụng quy trình thu thập khoảng 50 ảnh cho mỗi lần thu thập thông qua luồng camera. Chất lượng nhận diện phụ thuộc vào ánh sáng, tư thế, độ rõ của ảnh, chất lượng dữ liệu và cách đặt ngưỡng LBPH.

Có thể tham khảo thêm `README_SETUP.md` để biết hướng dẫn thiết lập và huấn luyện chi tiết.

## API tham khảo

Các API dưới đây phản ánh những route được khai báo trong mã nguồn hiện tại. Tất cả API đều được thiết kế cho môi trường phát triển; cần kiểm tra phân quyền và bảo vệ mạng trước khi triển khai thực tế.

### BRAIN Module — `http://localhost:8080`

| Phương thức | Endpoint | Chức năng |
|---|---|---|
| `GET` | `/api/users` | Lấy danh sách người dùng |
| `POST` | `/api/users` | Tạo hoặc lưu thông tin người dùng |
| `DELETE` | `/api/users/{id}` | Xóa người dùng theo ID; không cho xóa tài khoản `ADMIN` |
| `GET` | `/api/logs?page=0&size=20` | Lấy nhật ký truy cập theo trang |
| `POST` | `/api/detect` | Gửi kết quả nhận diện để backend xử lý yêu cầu mở khóa |
| `POST` | `/api/override` | Gửi lệnh điều khiển cửa thủ công từ phía quản trị |
| `POST` | `/api/camera/start` | Yêu cầu khởi động camera |
| `POST` | `/api/camera/stop` | Yêu cầu dừng camera |
| `GET` | `/api/camera/check_face` | Yêu cầu kiểm tra khuôn mặt trước camera |
| `POST` | `/api/camera/collect` | Yêu cầu thu thập ảnh khuôn mặt |
| `POST` | `/api/camera/train` | Yêu cầu huấn luyện mô hình nhận diện |

Ví dụ yêu cầu nhận diện:

```json
{
  "cameraName": "FrontDoor",
  "userId": 12,
  "distance": 42.5
}
```

Tên trường trong JSON phải khớp với DTO `UnlockRequestDto` trong mã nguồn. Ví dụ trên minh họa cấu trúc dự kiến; hãy kiểm tra DTO nếu bạn thay đổi tên thuộc tính.

Ví dụ yêu cầu điều khiển cửa:

```json
{
  "action": "OPEN"
}
```

Backend hiện ánh xạ `OPEN` thành lệnh `FORCE_OPEN`; các giá trị khác được ánh xạ thành `FORCE_CLOSE`. Chỉ cho phép người có quyền phù hợp gọi API này khi sử dụng trong môi trường thực tế.

### EYES Module — `http://localhost:8081`

| Phương thức | Endpoint | Chức năng |
|---|---|---|
| `POST` | `/add_cam` | Khởi chạy camera theo chỉ số, tên và slot |
| `POST` | `/stop_cam` | Dừng camera theo slot hoặc dừng các camera |
| `GET` | `/check_face` | Yêu cầu kiểm tra khuôn mặt từ luồng camera |
| `POST` | `/collect_face_data` | Thu thập dữ liệu khuôn mặt |
| `POST` | `/train_model` | Huấn luyện mô hình LBPH |
| `POST` | `/delete_face_data` | Xóa dữ liệu khuôn mặt theo logic của endpoint |

Các endpoint EYES được backend gọi nội bộ. Không nên công khai cổng `8081` ra Internet.

## Cấu hình

### Cổng dịch vụ

| Thành phần | Giá trị mặc định |
|---|---|
| BRAIN Module | `8080` |
| EYES Module | `8081` |
| MySQL | `3306` |
| MQTT broker | `broker.emqx.io:1883` |

### Mô hình nhận diện

Các đường dẫn và ngưỡng nhận diện hiện được khai báo trong mã nguồn C++:

- Haar Cascade: `../models/haarcascade_frontalface_default.xml`
- Mô hình LBPH: `../models/trainer.yml`
- Dataset: `../data/dataset`
- Ngưỡng nhận diện được đặt trong mã nguồn và có thể khác nhau giữa các lớp xử lý. Khi tinh chỉnh, cần kiểm thử trên dữ liệu thực tế thay vì xem một ngưỡng cố định là phù hợp cho mọi camera.

### MQTT

Backend hiện gửi lệnh tới broker được khai báo trong `MqttService.java`, sử dụng topic điều khiển như `huy_smartlock/locks/main`. Thiết bị điều khiển khóa cần đăng ký đúng topic và xử lý các thông điệp tương ứng, chẳng hạn `OPEN`, `FORCE_OPEN` hoặc `FORCE_CLOSE`.

Broker công cộng không phù hợp cho hệ thống khóa cửa thực tế. Nên chuyển sang broker riêng có xác thực, mã hóa TLS, ACL theo topic và cơ chế xác nhận lệnh.

## Luồng hoạt động

### Truy cập bằng khuôn mặt

1. Camera cung cấp khung hình cho EYES Module.
2. EYES Module phát hiện khuôn mặt và chạy nhận diện LBPH.
3. Khi nhận diện vượt qua điều kiện ngưỡng trong mã nguồn, EYES Module gửi thông tin camera, ID người dùng và khoảng cách nhận diện tới BRAIN Module.
4. Backend xử lý yêu cầu trong `UnlockService`.
5. Nếu yêu cầu được chấp nhận, backend gửi lệnh mở khóa qua MQTT.
6. Sự kiện truy cập được lưu vào MySQL để phục vụ tra cứu.

### Đăng ký người dùng mới

1. Tạo bản ghi người dùng và lấy ID.
2. Thu thập ảnh khuôn mặt có ID tương ứng.
3. Huấn luyện lại mô hình LBPH.
4. Kiểm tra nhận diện bằng camera.
5. Xác nhận backend xử lý đúng ID và ghi nhật ký theo kết quả.

### Điều khiển thủ công

1. Người vận hành gửi yêu cầu mở hoặc đóng cửa từ giao diện/API.
2. BRAIN Module ánh xạ hành động sang lệnh MQTT.
3. Lệnh được phát tới topic điều khiển đã cấu hình.
4. Backend ghi nhận sự kiện ghi đè vào nhật ký.

## Khắc phục sự cố

### Không mở được camera

- Đóng các ứng dụng khác đang sử dụng camera.
- Kiểm tra quyền truy cập camera trong hệ điều hành.
- Thử chỉ số camera khác.
- Xác nhận ứng dụng đang chạy trên Windows nếu sử dụng backend `CAP_DSHOW`.
- Kiểm tra log để xác định lỗi mở thiết bị hoặc không đọc được frame.

### Không nhận diện được khuôn mặt

- Xác nhận `haarcascade_frontalface_default.xml` tồn tại đúng đường dẫn.
- Xác nhận dataset chứa ảnh đúng định dạng tên và ID.
- Huấn luyện lại để tạo `trainer.yml`.
- Kiểm tra thư mục làm việc khi chạy chương trình vì các đường dẫn tài nguyên là đường dẫn tương đối.
- Bảo đảm khuôn mặt đủ sáng, rõ nét và chiếm diện tích phù hợp trong khung hình.
- Xem xét hiệu chỉnh ngưỡng LBPH sau khi kiểm thử cả trường hợp nhận đúng và nhận sai.

### EYES Module không kết nối được với BRAIN Module

- Kiểm tra BRAIN Module đang chạy tại cổng `8080`.
- Kiểm tra EYES Module đang lắng nghe tại cổng `8081`.
- Xác nhận không có firewall hoặc ứng dụng khác chiếm cổng.
- Kiểm tra URL backend trong `ApiClient` và các URL nội bộ trong `CameraController`.

### Backend không kết nối được MySQL

- Xác nhận dịch vụ MySQL đang chạy.
- Kiểm tra database `smartlockdb` đã được tạo.
- Kiểm tra username, password, port và JDBC URL trong `application.properties`.
- Đọc thông báo lỗi datasource trong log Spring Boot.

### Có nhận diện nhưng khóa không mở

- Kiểm tra nhật ký backend để xác nhận `/api/detect` đã được gọi.
- Kiểm tra điều kiện cho phép truy cập trong `UnlockService`.
- Kiểm tra broker MQTT, topic, payload và log gửi lệnh.
- Xác nhận thiết bị điều khiển khóa đang online và được cấu hình đúng topic.
- Lưu ý rằng gửi MQTT thành công không tự chứng minh khóa vật lý đã mở; cần có cơ chế phản hồi trạng thái từ thiết bị nếu muốn xác nhận kết quả.

## Giới hạn và lưu ý bảo mật

- Haar Cascade và LBPH là phương pháp truyền thống, nhạy cảm với ánh sáng, góc mặt, chất lượng camera và thay đổi diện mạo.
- Hệ thống chưa thể hiện cơ chế chống giả mạo khuôn mặt (liveness detection); ảnh in hoặc hình ảnh phát lại có thể tạo rủi ro.
- Cấu hình Spring Security hiện cho phép truy cập `/api/**` mà không yêu cầu xác thực. Cần bổ sung xác thực và phân quyền trước khi sử dụng thật.
- EYES Module dùng HTTP nội bộ không có xác thực ở các endpoint điều khiển camera và dữ liệu.
- Cấu hình MySQL mẫu chứa thông tin đăng nhập trong tệp cấu hình; cần chuyển sang biến môi trường hoặc hệ thống quản lý bí mật.
- MQTT đang dùng broker công cộng và kết nối không mã hóa trong cấu hình mẫu. Không dùng cấu hình này để điều khiển khóa thật.
- Script SQL có lệnh xóa và tạo lại database; không chạy trên dữ liệu quan trọng nếu chưa sao lưu.
- Dữ liệu khuôn mặt là dữ liệu cá nhân nhạy cảm. Chỉ thu thập khi có cơ sở pháp lý/sự đồng ý phù hợp, giới hạn quyền truy cập, bảo vệ dữ liệu và có chính sách xóa dữ liệu.
- Cần kiểm thử tình huống mất mạng, broker lỗi, backend dừng, camera mất kết nối, mất điện và trạng thái khóa không đồng bộ.
- Trước khi dùng với cửa thật, cần có cơ chế mở cửa khẩn cấp, phương án vận hành khi hệ thống lỗi và kiểm tra an toàn điện/cơ khí.

## Hướng phát triển

- Bổ sung xác thực và phân quyền theo vai trò cho toàn bộ API.
- Chuyển cấu hình nhạy cảm sang biến môi trường và bổ sung cấu hình triển khai riêng cho môi trường phát triển/sản xuất.
- Sử dụng MQTT broker riêng với TLS, xác thực, ACL và cơ chế xác nhận trạng thái khóa.
- Bổ sung trạng thái online/offline của camera và bộ điều khiển cửa.
- Cải thiện thuật toán nhận diện, kiểm thử ngưỡng trên tập dữ liệu đa dạng và đánh giá tỷ lệ nhận nhầm/từ chối nhầm.
- Nghiên cứu liveness detection để giảm nguy cơ giả mạo bằng ảnh hoặc video.
- Bổ sung lọc nhật ký theo người dùng, camera, trạng thái và khoảng thời gian.
- Đóng gói các mô-đun bằng Docker hoặc xây dựng script khởi động/giám sát ổn định.
- Viết kiểm thử tích hợp cho luồng nhận diện, xác thực quyền truy cập, gửi lệnh MQTT và ghi nhật ký.

## Tác giả và đóng góp

Dự án được phát triển nhằm nghiên cứu và thử nghiệm hệ thống khóa cửa thông minh sử dụng nhận diện khuôn mặt.

Nếu muốn đóng góp, hãy tạo nhánh riêng cho thay đổi, mô tả rõ mục tiêu, cách kiểm thử và các thay đổi cấu hình đi kèm trước khi tạo pull request.

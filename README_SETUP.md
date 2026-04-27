# SMART LOCK - HƯỚNG DẪN THIẾT LẬP NHẬN DIỆN KHUÔN MẶT

## VẤN ĐỀ HIỆN TẠI

Hệ thống không nhận diện được bạn vì:
- **Dataset trống** - Chưa có ảnh training nào trong thư mục `data/dataset`
- **Model chưa được huấn luyện** - File `models/trainer.yml` không có dữ liệu
- **Không có quy trình setup** - Chưa có cách để chụp ảnh và train model

## GIẢI PHÁP ĐÃ THỰC HIỆN

### 1. Cập nhật C++ Server (main.cpp)

Thêm 3 endpoint mới cho EYES-Module:

```
POST /collect_face_data    - Chụp ảnh training cho một người dùng
POST /train_model          - Huấn luyện mô hình nhận diện
GET  /dataset_info         - Kiểm tra dataset hiện tại
```

### 2. Tích hợp DataCollector & FaceTrainer

main.cpp giờ đây tích hợp đầy đủ:
- `DataCollector` - Chụp 50+ ảnh từ camera
- `FaceTrainer` - Huấn luyện LBPH model từ dataset

### 3. Script Setup (setup_and_train.ps1)

Tạo script PowerShell giúp bạn dễ dàng:
- Kiểm tra dataset
- Chụp ảnh
- Huấn luyện model

---

## CÁCH SỬ DỤNG

### BƯỚC 1: Build C++ Project

```bash
cd d:\FinalWave\face-recognition-door-lock\EYES-Module\FaceDetection
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

### BƯỚC 2: Chạy EYES-Module

```bash
cd d:\FinalWave\face-recognition-door-lock\EYES-Module\FaceDetection\build
# Windows: Release/SmartLock.exe hoặc mở project trong Visual Studio
```

**Khi chạy thành công, bạn sẽ thấy:**
```
[SYSTEM] EYES-MODULE KHOI DONG
[INFO] Dataset path: ../data/dataset
[INFO] Trainer path: ../models/trainer.yml
[SYSTEM] Dang cho lenh mo Camera tu Java tai cong 8081...
```

### BƯỚC 3: Chạy BRAIN-Module (Java)

```bash
cd d:\FinalWave\face-recognition-door-lock\BRAIN_Module\smartlock-server
mvn clean spring-boot:run
```

**Khi chạy thành công, bạn sẽ thấy:**
```
Started SmartlockServerApplication in X.XXX seconds
```

### BƯỚC 4: Chạy Setup Script

**Trong PowerShell (với quyền Admin):**

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser -Force
cd d:\FinalWave\face-recognition-door-lock
.\setup_and_train.ps1
```

---


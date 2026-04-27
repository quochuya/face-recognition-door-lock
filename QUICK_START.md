# 🎯 QUICK START GUIDE - SMART LOCK SETUP

```
┌─────────────────────────────────────────────────────────────┐
│     SMART LOCK FACE RECOGNITION SYSTEM - QUICK START        │
└─────────────────────────────────────────────────────────────┘
```

## ⚡ 5 BƯỚC NHANH

### BƯỚC 1: Build C++
```bash
cd d:\FinalWave\face-recognition-door-lock\EYES-Module\FaceDetection
mkdir build && cd build
cmake ..
cmake --build . --config Release
```
⏱️ Thời gian: 5-10 phút

### BƯỚC 2: Chạy C++ Server (EYES-Module)
```bash
cd d:\FinalWave\face-recognition-door-lock\EYES-Module\FaceDetection\build
Release\SmartLock.exe
```

✅ Kiểm tra: Bạn sẽ thấy:
```
[SYSTEM] EYES-MODULE KHOI DONG
[SYSTEM] Dang cho lenh mo Camera tu Java tai cong 8081...
```

### BƯỚC 3: Chạy Java Server (BRAIN-Module) - TẠB MỚI
```bash
cd d:\FinalWave\face-recognition-door-lock\BRAIN_Module\smartlock-server
mvn clean spring-boot:run
```

✅ Kiểm tra: Bạn sẽ thấy:
```
Started SmartlockServerApplication
```

### BƯỚC 4: Chạy Setup Script - TẠB MỚI
```powershell
# Bật PowerShell với quyền Admin
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser -Force
cd d:\FinalWave\face-recognition-door-lock
.\setup_and_train.ps1
```

### BƯỚC 5: Chọn "4" - Toàn bộ quy trình tự động
```
╔═══════════════════════════════════════════════════════╗
║       SMART LOCK - FACE RECOGNITION SETUP            ║
╚═══════════════════════════════════════════════════════╝

  1️⃣  Kiểm tra dataset hiện tại
  2️⃣  Chụp ảnh training cho người dùng
  3️⃣  Huấn luyện mô hình
  4️⃣  Chạy toàn bộ quy trình (1 người dùng)  👈 CHỌN ĐÂY
  5️⃣  Thoát

Chọn tùy chọn (1-5): 4
Nhập User ID: 1
Nhập số ảnh muốn chụp (mặc định 50): 50
```

📸 **LÚC ĐÓ:**
- Cửa sổ camera sẽ mở
- Hướng mặt thẳng vào camera
- Thay đổi góc độ mặt
- Hệ thống sẽ tự động chụp ~50 ảnh
- Sau đó sẽ tự động huấn luyện model

⏱️ Thời gian: 2-3 phút

---

## ✨ HIỆU QUẢ SAU KHI HOÀN THÀNH

### 📊 Kiểm tra Dataset
```
✓ Tổng ảnh: 50
✓ Số người dùng: 1
✓ User IDs: [1]
```

### 📁 File được tạo
```
✓ d:\...\EYES-Module\data\dataset\1.1.jpg (ảnh)
✓ d:\...\EYES-Module\data\dataset\1.2.jpg (ảnh)
✓ ...
✓ d:\...\EYES-Module\data\dataset\1.50.jpg (ảnh)
✓ d:\...\EYES-Module\models\trainer.yml (model - >100KB)
```

### 🔓 Kiểm tra Nhận Diện
```
Mở web: http://localhost:8080
→ Bấm nút "Mở Cửa"
→ Hướng mặt vào camera
→ Hệ thống sẽ nhận diện
→ Cửa mở tự động ✓
```

---

## 🐛 TROUBLESHOOTING NHANH

### ❌ "Không thể kết nối tới EYES-Module"
```
✓ Kiểm tra xem C++ server đã chạy chưa
✓ Kiểm tra cổng 8081: netstat -ano | findstr :8081
```

### ❌ "Khong tim thay anh nao de huan luyen"
```
✓ Hãy chụp ảnh trước (Bước 2, Lựa chọn "2")
```

### ❌ "Camera không mở"
```
✓ Đóng các app khác dùng camera (Zoom, Teams, etc)
✓ Kiểm tra kết nối camera
✓ Thử camera index khác
```

---

## 📋 DANH SÁCH CÁC TỆP LIÊN QUAN

📄 **Tài liệu hướng dẫn:**
- `README_SETUP.md` - Hướng dẫn chi tiết
- `CHANGES.md` - Danh sách tất cả thay đổi
- `QUICK_START.md` - Tài liệu này

🛠️ **Script:**
- `setup_and_train.ps1` - Script setup tương tác

📦 **Mã nguồn:**
- `EYES-Module/FaceDetection/main.cpp` - Server chính
- `EYES-Module/FaceDetection/src/DataCollector.cpp` - Chụp ảnh
- `EYES-Module/FaceDetection/src/FaceTrainer.cpp` - Huấn luyện
- `BRAIN_Module/smartlock-server/...` - Java backend

---

## 🎓 CÁCH HOẠT ĐỘNG

```
Camera → Haar Cascade → Face Detection → LBPH Recognition → Java Backend
         (Phát hiện)                    (Nhận diện)        (Quyết định)
```

1. **Haar Cascade**: Phát hiện mặt trong ảnh
2. **LBPH (Local Binary Patterns Histograms)**: So sánh với các ảnh đã train
3. **Java Backend**: Nếu `distance <= 65.0` → Mở cửa

---

## 🎯 BENCHMARK HIỆU NĂNG

| Thông số | Giá trị |
|----------|--------|
| Dataset size | 50-100 ảnh/người |
| Training time | 1-2 giây |
| Recognition time | 50-100ms/frame |
| FPS | 15-20 fps |
| Recognition accuracy | 85-95% (phụ thuộc ảnh chụp) |

---

**Sẵn sàng bắt đầu? Theo dõi các bước trên nhé!** 🚀

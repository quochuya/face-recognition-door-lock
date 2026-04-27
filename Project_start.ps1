
Write-Host "     KHOI DONG HE THONG SMARTLOCK - LE QUOC HUY" -ForegroundColor Green
Write-Host ""

Write-Host "[1/2] Dang danh thuc 'Bo Nao' Java Spring Boot..." -ForegroundColor Cyan
Write-Host "Vui long doi khoang 15 giay de khoi tao MySQL va Tomcat..." -ForegroundColor Yellow

Start-Process powershell -ArgumentList "-NoExit", "-Command", "cd BRAIN_Module\smartlock-server; .\mvnw.cmd spring-boot:run"

Start-Sleep -Seconds 15

Write-Host ""
Write-Host "[2/2] Dang danh thuc C++ Camera..." -ForegroundColor Cyan

Start-Process powershell -ArgumentList "-NoExit", "-Command", "cd EYES-Module\FaceDetection\build; .\Debug\SmartLock.exe"

Write-Host ""
Write-Host "[SUCCESS] TOAN BO HE THONG DA SAN SANG!" -ForegroundColor Green
Write-Host "- Web Dashboard: http://localhost:8080" -ForegroundColor White
Write-Host "- C++ Server: Port 8081" -ForegroundColor White
Write-Host ""

Read-Host "Nhan Enter de thoat cua so"
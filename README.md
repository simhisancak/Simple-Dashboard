# Fracq

Client/Server uygulaması.

## Demo Videoları

- [Çalışma Örneği](https://streamable.com/zv2vom)
- [Adres Bulma](https://youtu.be/0zPahFoajko)

## Gereksinimler

- Visual Studio 2022
- CMake (3.x veya üzeri)
- PowerShell 7.0 veya üzeri

## Kurulum

1. Projeyi klonlayın ve submodule'leri initialize edin:
```bash
git clone [repository-url]
cd Fracq
git submodule update --init --recursive
```

2. Build script'ini çalıştırmak için PowerShell politikasını ayarlayın (Yönetici olarak PowerShell'de):
```powershell
Set-ExecutionPolicy RemoteSigned
```

## Build İşlemi

Proje, `build.ps1` PowerShell scripti kullanılarak build edilebilir. Script aşağıdaki komutları destekler:

### Client Build Etme
```powershell
.\build.ps1 build client
```

### Server Build Etme
```powershell
.\build.ps1 build server
```

### Tüm Projeyi Build Etme
```powershell
.\build.ps1 build all
```

### Server'ı Çalıştırma
```powershell
.\build.ps1 run server
```

### Build Klasörünü Temizleme
```powershell
.\build.ps1 clean
```

### Yardım Menüsü
```powershell
.\build.ps1 help
```

## Build Çıktıları

Build edilen dosyalar `build/bin/Release/` klasöründe oluşturulur:
- `FracqClient.exe` - Client uygulaması
- `FracqServer.exe` - Server uygulaması

## Proje Yapısı

- `Client/` - Client kaynak kodları
- `Server/` - Server kaynak kodları
- `shared/` - Client ve Server arasında paylaşılan kod
- `build.ps1` - Build ve çalıştırma scripti
- `CMakeLists.txt` - CMake yapılandırma dosyası

## Olası Sorunlar ve Çözümleri

### Dear ImGui Hatası
Eğer Dear ImGui ile ilgili dosyaların eksik olduğuna dair hatalar alıyorsanız, şu komutu çalıştırın:
```bash
git submodule update --init --recursive
```

### Genel Build Hataları
Build işlemi sırasında herhangi bir hata alırsanız, önce `build.ps1 clean` komutunu çalıştırıp tekrar deneyebilirsiniz.
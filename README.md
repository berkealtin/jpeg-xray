# jpeg-xray

Bu proje, JPEG dosyalarını byte (ikili veri) düzeyinde analiz ederek içerdikleri metadata bilgilerini ayrıştıran, düşük seviyeli (low-level) bir C programıdır.

---

## Proje Amacı

Bu uygulamanın temel amacı, bir dosya biçimini yalnızca bir uzantı olarak değil, diskte belirli kurallara göre temsil edilen bir "ikili veri yapısı" olarak görmektir. Hazır harici kütüphaneler (libjpeg, exiv2 vb.) **kullanılmadan**, yalnızca standart C kütüphanesi fonksiyonları ile geliştirilmiştir. Proje kapsamında hedeflenen kazanımlar şunlardır:

- Binary dosya okuma ve yönetme (`fopen`, `fseek`, `fgetc`).
- JPEG marker ve segment yapısını (APPn, DQT, DHT, SOF vb.) anlama.
- EXIF, XMP, ICC ve yorum (COM) alanlarını byte düzeyinde ayrıştırma.
- Endianness (Little/Big Endian), offset, length ve dinamik bellek yönetimi (`malloc/free`) kavramlarını uygulamalı olarak gerçekleştirme.

---

## Özellikler

Program, taranan JPEG dosyası içerisinden aşağıdaki bilgileri tespit edip okuyabilir:

### Segment Haritası
- Dosya başlığından (`0xFFD8` SOI) görüntü verisine (`0xFFDA` SOS) kadar olan JPEG içindeki marker yapılarını tespit eder.

### Basic JPEG Info (APP0 - JFIF)
- Genişlik (Width) ve Yükseklik (Height)
- JFIF yoğunluk bilgileri (X/Y Density)
- Yoğunluk birimi (Density Unit)

### EXIF Metadata (APP1)
- Kamera üreticisi (Make) ve Kamera modeli (Model)
- Çekim tarihi (Date Taken) ve Değiştirme tarihi (Modify Date)
- Yazılım bilgisi (Software)
- Pozlama ayarları: ISO değeri, Pozlama süresi (Exposure Time), Diyafram değeri (F Number)

### XMP Metadata (APP1)
- Creator Tool, Metadata Date, Label gibi Adobe tabanlı XML metadata içeriklerini metinsel olarak ayrıştırır.

### Yorum / Comment (COM)
- JPEG içine gömülmüş düz metin açıklamalarını (`0xFFFE`) bulur ve ekrana basar.

### ICC Profile (APP2)
- Dosyada bir renk yönetim profili (ICC) bulunup bulunmadığını kontrol eder.

---

## Derleme ve Kullanım

Uygulama standart C kütüphaneleri kullandığı için işletim sistemi bağımsız olarak çalıştırılabilir. Terminal veya komut satırını açıp dosyanın bulunduğu dizinde aşağıdaki komutu çalıştırarak derleyebilirsiniz:

    gcc jpeg-xray.c -o jpeg-xray

---

## Örnek Çıktı
[Segments Map]
FFD8: SOI (Start of Image)
FFE1: APP1 (EXIF/XMP)
FFE1: APP1 (EXIF/XMP)
FFDB: DQT (Quantization Table)
FFC0: SOF0 (Start of Frame)
FFC4: DHT (Huffman Table)
FFDA: SOS (Start of Scan)

File: test.jpg

[Basic JPEG Info]
Width              : 650
Height             : 488
JFIF X Density     : Not available
JFIF Y Density     : Not available
Density Unit       : Not available

[EXIF]
Camera Make        : samsung
Camera Model       : SM-G930P
Date Taken         : 2017:05:29 11:11:16
Modify Date        : 2017:07:17 12:17:25
Software           : Adobe Photoshop Lightroom 6.10.1 (Macintosh)
ISO                : 40
Exposure Time      : 1/1600
F Number           : 1.7

[XMP]
Creator Tool       : Not available
Metadata Date      : Not available
Label              : Not available

[Comment]
Text               : Not available

[ICC]
ICC Profile        : Not available

---

## Geliştirici Bilgileri

- **Geliştirici:** Berke Altın

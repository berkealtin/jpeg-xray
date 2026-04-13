# jpeg-xray

This project is a low-level C program that analyzes JPEG files at the byte (binary data) level and extracts their embedded metadata.

---

## Project Purpose

The main goal of this application is to treat a file format not merely as an extension, but as a "binary data structure" represented on disk according to specific rules. It is developed **without using external libraries** (such as libjpeg, exiv2, etc.), relying solely on standard C library functions. The key learning outcomes targeted in this project include:

- Reading and managing binary files (`fopen`, `fseek`, `fgetc`)
- Understanding JPEG marker and segment structures (APPn, DQT, DHT, SOF, etc.)
- Parsing EXIF, XMP, ICC, and comment (COM) fields at the byte level
- Practically applying concepts such as endianness (Little/Big Endian), offsets, lengths, and dynamic memory management (`malloc/free`)

---

## Features

The program can detect and extract the following information from a scanned JPEG file:

### Segment Map
- Identifies JPEG marker structures from the file header (`0xFFD8` SOI) to the image data (`0xFFDA` SOS)

### Basic JPEG Info (APP0 - JFIF)
- Width and Height
- JFIF density values (X/Y Density)
- Density unit

### EXIF Metadata (APP1)
- Camera make and model
- Date taken and modification date
- Software information
- Exposure settings: ISO value, exposure time, and aperture (F Number)

### XMP Metadata (APP1)
- Extracts Adobe-based XML metadata fields such as Creator Tool, Metadata Date, and Label as text

### Comment (COM)
- Finds and displays plain text comments (`0xFFFE`) embedded in the JPEG

### ICC Profile (APP2)
- Checks whether a color management profile (ICC) exists in the file

---

## Build and Usage

Since the application uses only standard C libraries, it can run independently of the operating system. Open your terminal or command line in the directory where the file is located and compile it using the following command:

    gcc jpeg-xray.c -o jpeg-xray

---

## Sample Output
```text
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
```
---

## Developer Information

- **Developer**: Berke Altın

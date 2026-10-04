# LSB-Steganography

A C-based image steganography project that hides secret text data inside a BMP image using the **Least Significant Bit (LSB)** technique.

The project supports both **encoding** secret data into an image and **decoding** the hidden data from a stego image.

## 📌 Project Overview

**LSB Steganography** is a technique used to hide information inside an image by modifying the least significant bit of image data.

In this project, the secret text is embedded into the pixel data of a BMP image. Since only the LSB is modified, the visual appearance of the image remains almost unchanged.

The project also uses a **magic string** to verify whether the image contains encoded data.

## ✨ Features

- Encode a secret `.txt` file into a `.bmp` image
- Decode hidden data from a stego `.bmp` image
- LSB-based data hiding
- BMP header preservation
- Image capacity checking before encoding
- Magic string verification during decoding
- Secret file extension and size are stored along with the data
- Command-line based operation
- Written completely in C

## 🔄 Working Principle

### Encoding

```text
Source BMP Image
       +
Secret Text File
       ↓
Capacity Check
       ↓
Copy BMP Header
       ↓
Encode Magic String
       ↓
Encode Secret File Extension
       ↓
Encode Secret File Size
       ↓
Encode Secret File Data
       ↓
Copy Remaining Image Data
       ↓
Stego BMP Image
```

### Decoding

```text
Stego BMP Image
       ↓
Read Magic String
       ↓
Verify Magic String
       ↓
Decode Secret File Extension
       ↓
Decode Secret File Size
       ↓
Decode Secret File Data
       ↓
Recovered Secret Text File
```

## 🧠 How LSB Encoding Works

Each byte of image data contains 8 bits.

The project modifies the **least significant bit (LSB)** of image bytes to store the secret data.

For example:

```text
Image byte : 10110100
Secret bit :        1
```

After encoding:

```text
10110101
```

Only the LSB is changed.

One byte of secret data contains 8 bits, so **8 image bytes are used to store one secret byte**.

The encoding function extracts each bit of the secret byte and stores it in the LSB of the corresponding image byte. 

## 🖼️ BMP Image Handling

The project works with BMP images and preserves the BMP header before encoding the secret data.

The image dimensions are read from the BMP header to calculate the available image capacity.

## 🔐 Data Stored in the Image

The following information is encoded into the image:

1. Magic String
2. Secret File Extension Size
3. Secret File Extension
4. Secret File Size
5. Secret File Data

During decoding, the magic string is first verified before extracting the remaining information.

## 🛠️ Technologies Used

- **Language:** C
- **Image Format:** BMP
- **Technique:** LSB Steganography
- **Compiler:** GCC
- **Concepts Used:**
  - File Handling
  - Pointers
  - Structures
  - Bitwise Operations
  - Command-Line Arguments
  - Dynamic data processing
  - Binary file processing

## 📁 Project Structure

```text
LSB-Steganography/
│
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── types.h
├── common.h
├── README.md
├── .gitignore
└── beautiful.bmp
```

## ⚙️ Compilation

Compile the project using GCC:

```bash
gcc main.c encode.c decode.c -o steganography
```

## ▶️ Usage

### Encoding

```bash
./steganography -e source.bmp secret.txt stego.bmp
```

Example:

```bash
./steganography -e beautiful.bmp secret.txt stego.bmp
```

The program asks for a magic string and then embeds the secret data into the BMP image.

### Decoding

```bash
./steganography -d stego.bmp output
```

Example:

```bash
./steganography -d stego.bmp output
```

The decoded data is stored as:

```text
output.txt
```

## 📊 Capacity Check

Before encoding, the program checks whether the BMP image has enough capacity to store:

```text
Magic String
        +
Secret File Extension
        +
Secret File Size
        +
Secret File Data
```

If the image does not have sufficient capacity, encoding is stopped.

## 🔑 Magic String

A magic string is used to identify whether the image contains encoded information.

During decoding, the stored magic string is extracted from the image and compared with the magic string entered by the user.

If they match, decoding continues.

## 🧪 Sample Execution

### Encoding

```text
encoding enabled

read and validate encode arguments are successfully done
Open the files done successfully
Enter the Magic String: @#
Check capacity done successfully
BMP header copied successfully
Magic string encoded successfully
Secret file extension size encoded successfully
Secret file extension encoded successfully
Secret file size encoded successfully
Secret file data encoded successfully
Copy remaining image data done successfully

-->Encoding successfully done
```

### Decoding

```text
decoding enabled

read and validation decode arguments are done successfully
Open the files done successfully
Enter the Magic String : @#
Magic string is matched successfully
Secret file ext size is decoded successfully
Secret file ext decoded successfully
Secret file size is decoded successfully
Secret file data decoded successfully

-->Decoding done successfully
```

## 👨‍💻 Author

**Rajkamal**

Electronics and Communication Engineering

## 📜 License

This project is created for educational and learning purposes.

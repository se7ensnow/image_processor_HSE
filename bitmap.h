#pragma once

#include <cstdint>
#include <iostream>
#include <tuple>


class PixelArray {
public:
    struct Pixel {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        bool operator==(const Pixel& rhv) const {
            return std::tie(blue, green, red) == std::tie(rhv.blue, rhv.green, rhv.red);
        }

        bool operator !=(const Pixel& rhv) const {
            return !(*this == rhv);
        }

        friend std::ostream& operator<<(std::ostream& strm, const Pixel& p) {
            strm <<"red: " <<int(p.red) <<" green: " <<int(p.green) <<" blue: " <<int(p.blue);
            return strm;
        }
    };

public:
    PixelArray() : storage_(nullptr), height_(0), width_(0) {}

    PixelArray(size_t height, size_t width, Pixel default_pixel = Pixel())
            : PixelArray() {
        Resize(height, width, default_pixel);
    }

    PixelArray(const PixelArray& other);

    PixelArray(PixelArray&& other) noexcept;

    ~PixelArray() {
        FreeStorage();
    }

    PixelArray& operator=(const PixelArray& rhv);

    void Resize(size_t height, size_t width, Pixel default_pixel = Pixel());

    size_t GetHeight() const{
        return height_;
    }

    size_t GetWidth() const{
        return width_;
    }

    Pixel& operator()(size_t row, size_t column) {
        return GetPixel(storage_, width_, row, column);
    }

    const Pixel& operator()(size_t row, size_t column) const {
        return GetPixel(storage_, width_, row, column);
    }

    Pixel& At(size_t row, size_t column);

    const Pixel& At(size_t row, size_t column) const;

protected:
    // Освобождение массива
    void FreeStorage();

    // Копирует содержимое хранилища storage_ в хранилище передаваемое параметром Bitmap::Pixel* new_storage,
    // принимая во внимание различные параметры
    void CopyStorage(Pixel* new_storage,
                     size_t new_height,
                     size_t new_width,
                     Pixel default_pixel) const;

    void Swap(PixelArray& other) noexcept {
        std::swap(storage_, other.storage_);
        std::swap(height_, other.height_);
        std::swap(width_, other.width_);
    }

    static Pixel* AllocateStorage(size_t height, size_t width) {
        return new Pixel[height * width];
    }

    static Pixel& GetPixel(Pixel* storage, size_t width, size_t row, size_t column) {
        return storage[width * row + column];
    }

    static const Pixel& GetPixel(const Pixel* storage, const size_t width, size_t row, size_t column) {
        return storage[width * row + column];
    }

protected:
    Pixel* storage_;
    size_t height_;
    size_t width_;
};


class Bitmap {
public:
    struct BMPHeader {
        uint16_t signature;
        uint32_t file_size;
        uint32_t dummy;
        uint32_t bitarray_offset;
    } __attribute__((__packed__));

    struct DIBHeader {
        uint32_t dib_header_size;
        uint32_t width;  // Ширина
        uint32_t height; // Высота
        uint16_t dummy1; // Это же на не нужно?
        uint16_t bits_per_pixel;
        uint32_t dummy2; // Это же нам не нужно?
        uint32_t raw_bitmap_data_size;  // (including padding)
        uint64_t dummy3; // Это же нам не нужно?
        uint64_t dummy4; // Это же нам не нужно?
    } __attribute__((__packed__));

public:
    // Загружает файл из переданного потока чтения (функция под этой как раз возвращает поток)
    bool Load(std::istream& stream);

    // Загружает файл с картинкой по имени в поток
    bool Load(const char* file_name);

    // Создаёт по имени файла bmp файл и загружает туда что-то.
    bool CreateFile(const char* file_name);

    // Мы можем не иметь права записывать что-то на диск или не иметь какой-то папки
    // поэтому делим create file в виде двух функций, по аналогии с load.
    bool CreateFile(std::ofstream& stream);

    PixelArray& GetPixels() {return pixels_;}

protected:
    BMPHeader bmp_header_;
    DIBHeader dib_header_;
    PixelArray pixels_;
};



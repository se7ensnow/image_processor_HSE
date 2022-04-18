#include "bitmap.h"

#include <fstream>


PixelArray::PixelArray(const PixelArray& other) : PixelArray() {
    if (!other.storage_) {
        return;
    }
    storage_ = AllocateStorage(other.height_, other.width_);
    height_ = other.height_;
    width_ = other.width_;
    other.CopyStorage(storage_, height_, width_, Pixel());
}

PixelArray::PixelArray(PixelArray &&other) noexcept {
    storage_ = other.storage_;
    width_ = other.width_;
    height_ = other.height_;
    other.storage_ = nullptr;
    other.height_ = 0;
    other.width_ = 0;
}

PixelArray& PixelArray::operator=(const PixelArray &rhv) {
    if (&rhv == this)  {
        return *this;
    }
    PixelArray holder(rhv);
    Swap(holder);
    return *this;
}

void PixelArray::Resize(size_t height, size_t width, Pixel default_pixel) {
    if (height_ == height && width_ == width) {
        return;
    }
    if (height == 0 || width == 0) {
        FreeStorage();
        return;
    }
    Pixel* new_memory = AllocateStorage(height, width);
    CopyStorage(new_memory, height, width, default_pixel);
    FreeStorage();
    storage_ = new_memory;
    height_ = height;
    width_ = width;
}

void PixelArray::FreeStorage() {
    delete[] storage_;
    storage_ = nullptr;
    height_ = 0;
    width_ = 0;
}

void PixelArray::CopyStorage(Pixel* new_storage, size_t new_height, size_t new_width, Pixel default_pixel) const {
    for (size_t i = 0; i < new_height; ++i) {
        for (size_t j = 0; j < new_width; ++j) {
            if (i < height_ && j < width_) {
                GetPixel(new_storage, new_width, i, j) = GetPixel(storage_, width_, i, j);
            } else {
                GetPixel(new_storage, new_width, i, j) = default_pixel;
            }
        }
    }
}

PixelArray::Pixel& PixelArray::At(size_t row, size_t column) {
    if (row >= height_ || column >= width_) {
        throw std::out_of_range("Invalid row or column");
    }
    return GetPixel(storage_, width_, row, column);
}

const PixelArray::Pixel &PixelArray::At(size_t row, size_t column) const {
    if (row >= height_ || column >= width_) {
        throw std::out_of_range("Invalid row or column");
    }
    return GetPixel(storage_, width_, row, column);
}

// -------------------------------------------------------------------------------------------------------------

bool Bitmap::Load(const char* file_name) {
    std::fstream file;
    file.open(file_name, std::ios_base::in | std::ios_base::binary); // Открываем файл на чтение как бинарный
    if (!file.is_open()) {
        return false;
    }
    bool file_loaded = Load(file);
    return file_loaded;
}

bool Bitmap::Load(std::istream& stream) {
    if (!stream) {
        return false;
    }
    stream.read(reinterpret_cast<char *> (&bmp_header_), sizeof(bmp_header_));
    stream.read(reinterpret_cast<char *> (&dib_header_), sizeof(dib_header_));
    if (dib_header_.bits_per_pixel != 24) {
        return false;
    }
    pixels_.Resize(dib_header_.height, dib_header_.width);
    size_t RUBBISH_BETWEEN_LINES = (4 - (dib_header_.width * 3) % 4) % 4;
    for (size_t i = 0; i < dib_header_.height; ++i) {
        for (size_t j = 0; j < dib_header_.width; ++j) {
            if (!stream) {
                return false;
            }
            stream.read(reinterpret_cast<char *> (&pixels_(i, j)), sizeof(PixelArray::Pixel));
        }
        stream.ignore(RUBBISH_BETWEEN_LINES);
    }
    return true;
}

bool Bitmap::CreateFile(const char* file_name) {
    std::ofstream file;
    file.open(file_name, std::ios_base::out | std::ios_base::binary); // Открываем файл на вписывание как бинарный
    if (!file.is_open()) {
        return false;
    }
    CreateFile(file);
    return true;
}

bool Bitmap::CreateFile(std::ofstream& stream) {
    if (!stream) {
        return false;
    }
    dib_header_.width = pixels_.GetWidth();
    dib_header_.height = pixels_.GetHeight();
    size_t RUBBISH_BETWEEN_LINES = (4 - (dib_header_.width * 3) % 4) % 4;
    dib_header_.raw_bitmap_data_size = dib_header_.height * (dib_header_.width * 3 + RUBBISH_BETWEEN_LINES);
    bmp_header_.file_size = bmp_header_.bitarray_offset + dib_header_.raw_bitmap_data_size;
    stream.write(reinterpret_cast<char*> (&bmp_header_), sizeof(bmp_header_));
    stream.write(reinterpret_cast<char*> (&dib_header_), sizeof(dib_header_));
    static int RUBBISH = 0x42424242;
    for (size_t i = 0; i < dib_header_.height; ++i) {
        for (size_t j = 0; j < dib_header_.width; ++j) {
            if (!stream) {
                return false;
            }
            stream.write(reinterpret_cast<char*> (&pixels_(i, j)), sizeof(PixelArray::Pixel));
        }
        stream.write(reinterpret_cast<char*> (&RUBBISH), RUBBISH_BETWEEN_LINES);
    }
    return true;
}
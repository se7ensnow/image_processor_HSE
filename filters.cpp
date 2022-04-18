#include "filters.h"

namespace PixelMath {
    PixelArray::Pixel ApplyMatrix(const PixelArray& pixels, size_t row, size_t column,
                                  Matrix& matrix) {
        PixelArray::Pixel new_pixel = PixelArray::Pixel();
        size_t matrix_vertical_radius = matrix.size() / 2; // расстояние от центра матрицы к вертикальному краю
        size_t matrix_horizontal_radius = matrix[0].size() / 2; // расстояние от центра матрицы к горизонтальному краю
        int current_row;
        int current_column;
        int height = pixels.GetHeight();
        int width = pixels.GetWidth();
        double red = 0;
        double green = 0;
        double blue = 0;
        for (size_t i = 0; i < matrix.size(); ++i) {
            for (size_t j = 0; j < matrix[0].size(); ++j) {
                current_row = row + i - matrix_vertical_radius;
                current_column = column + j - matrix_horizontal_radius;
                current_row = std::clamp(current_row, 0, height - 1);
                current_column = std::clamp(current_column, 0, width - 1);
                red += matrix[i][j] * pixels(current_row, current_column).red;
                green += matrix[i][j] * pixels(current_row, current_column).green;
                blue += matrix[i][j] * pixels(current_row, current_column).blue;
            }
        }
        new_pixel.red = std::min(255, std::max(0, int(std::round(red))));
        new_pixel.green = std::min(255, std::max(0, int(std::round(green))));
        new_pixel.blue = std::min(255, std::max(0, int(std::round(blue))));
        return new_pixel;
    }

    Matrix TransposeMatrix(const Matrix& matrix) {
        size_t matrix_height = matrix.size();
        size_t matrix_width = matrix[0].size();
        Matrix result;
        std::vector<double> current_row;
        double current_value;
        for (size_t i = 0; i < matrix_width; ++i) {
            for (size_t j = 0; j < matrix_height; ++j) {
                current_value = matrix[j][i];
                current_row.push_back(current_value);
            }
            result.push_back(current_row);
            current_row.clear();
        }
        return result;
    }
}

void GaussianBlurFilter::Apply(Bitmap& image) {
    PixelArray& image_pixels = image.GetPixels();
    PixelArray new_pixels = image_pixels;
    PixelArray::Pixel current_pixel{};
    for (size_t i = 0; i < image_pixels.GetHeight(); ++i) {
        for (size_t j = 0; j < image_pixels.GetWidth(); ++j) {
            current_pixel = PixelMath::ApplyMatrix(image_pixels, i, j, matrix_);
            new_pixels(i, j) = current_pixel;
        }
    }
    image_pixels = new_pixels;
    PixelMath::Matrix transposed_matrix = PixelMath::TransposeMatrix(matrix_);
    for (size_t i = 0; i < image_pixels.GetHeight(); ++i) {
        for (size_t j = 0; j < image_pixels.GetWidth(); ++j) {
            current_pixel = PixelMath::ApplyMatrix(image_pixels, i, j, transposed_matrix);
            new_pixels(i, j) = current_pixel;
        }
    }
    image_pixels = new_pixels;
}

void GaussianBlurFilter::GenerateMatrix(double sigma) {
    // Пиксели на расстоянии более 3σ оказывают достаточно малое влияние, можно не считать
    size_t matrix_radius = std::ceil(sigma * 3);
    size_t matrix_size = matrix_radius * 2 + 1;
    std::vector<double> matrix_row(matrix_size);
    double exponential;
    double square;
    double current_weight;
    for (size_t i = 0; i < matrix_radius + 1; ++i) {
        exponential = std::exp(-0.5 * pow(i / sigma, 2));
        square = std::sqrt(2 * M_PI * sigma * sigma);
        current_weight = exponential / square;
        matrix_row[matrix_radius + i] = current_weight;
        matrix_row[matrix_radius - i] = current_weight;
    }
    double matrix_sum = 0;
    for (auto& n : matrix_row) {
        matrix_sum += n;
    }
    for (double& i : matrix_row) {
        i /= matrix_sum;
    }
    matrix_.push_back(matrix_row);
}

void CropFilter::Apply(Bitmap& image) {
    PixelArray& image_pixels = image.GetPixels();
    size_t image_width = image_pixels.GetWidth();
    size_t image_height = image_pixels.GetHeight();
    if (width_ > image_width) {
        width_ = image_width;
    }
    if (height_ > image_height) {
        height_ = image_height;
    }
    RotateImage(image_pixels);
    image_pixels.Resize(height_, width_);
    RotateImage(image_pixels);
}

void CropFilter::RotateImage(PixelArray& pixels) {
    for (size_t i = 0; i < pixels.GetHeight() / 2; ++i) {
        for (size_t j = 0; j < pixels.GetWidth(); ++j) {
            std::swap(pixels(i, j), pixels(pixels.GetHeight() - i - 1, j));
        }
    }
}

void NegativeFilter::Apply(Bitmap& image) {
    PixelArray& image_pixels = image.GetPixels();
    PixelArray::Pixel current_pixel{};
    for (size_t i = 0; i < image_pixels.GetHeight(); ++i) {
        for (size_t j = 0; j < image_pixels.GetWidth(); ++j) {
            current_pixel = image_pixels(i, j);
            current_pixel.red = 255 - current_pixel.red;
            current_pixel.green = 255 - current_pixel.green;
            current_pixel.blue = 255 - current_pixel.blue;
            image_pixels(i, j) = current_pixel;
        }
    }
}

void GrayscaleFilter::Apply(Bitmap& image) {
    PixelArray& image_pixels = image.GetPixels();
    PixelArray::Pixel current_pixel{};
    uint8_t current_grayscale;
    for (size_t i = 0; i < image_pixels.GetHeight(); ++i) {
        for (size_t j = 0; j < image_pixels.GetWidth(); ++j) {
            current_pixel = image_pixels(i, j);
            current_grayscale = GetGrayscale(current_pixel);
            current_pixel.red = current_grayscale;
            current_pixel.green = current_grayscale;
            current_pixel.blue = current_grayscale;
            image_pixels(i, j) = current_pixel;
        }
    }
}

void SharpeningFilter::Apply(Bitmap& image) {
    PixelArray& image_pixels = image.GetPixels();
    PixelArray new_pixels = image_pixels;
    PixelArray::Pixel current_pixel{};
    for (size_t i = 0; i < image_pixels.GetHeight(); ++i) {
        for (size_t j = 0; j < image_pixels.GetWidth(); ++j) {
            current_pixel = GetSharpenPixel(image_pixels, i, j);
            new_pixels(i, j) = current_pixel;
        }
    }
    image_pixels = new_pixels;
}

PixelArray::Pixel SharpeningFilter::GetSharpenPixel(PixelArray& pixels, size_t i, size_t j) {
    return PixelMath::ApplyMatrix(pixels, i, j, matrix_);
}

void EdgeDetectionFilter::Apply(Bitmap& image) {
    GrayscaleFilter::Apply(image);
    PixelArray& image_pixels = image.GetPixels();
    PixelArray new_pixels = image_pixels;
    PixelArray::Pixel current_pixel{};
    for (size_t i = 0; i < image_pixels.GetHeight(); ++i) {
        for (size_t j = 0; j < image_pixels.GetWidth(); ++j) {
            current_pixel = PixelMath::ApplyMatrix(image_pixels, i, j, matrix_);
            if (static_cast<double>(current_pixel.red) / 255 > threshold_) {
                current_pixel.red = 255;
                current_pixel.green = 255;
                current_pixel.blue = 255;
            } else {
                current_pixel.red = 0;
                current_pixel.green = 0;
                current_pixel.blue = 0;
            }
            new_pixels(i, j) = current_pixel;
        }
    }
    image_pixels = new_pixels;
}

void LanczosScaleFilter::Apply(Bitmap& image) {
    PixelArray& image_pixels = image.GetPixels();
    size_t current_width = image_pixels.GetWidth();
    size_t current_height = image_pixels.GetHeight();
    if (current_width == dest_width_ && current_height == dest_height_) {
        return;
    }
    double delta_x = static_cast<double>(current_width) / static_cast<double>(dest_width_);
    double delta_y = static_cast<double>(current_height) / static_cast<double>(dest_height_);
    double new_x;
    double new_y;
    PixelArray new_width_pixels = PixelArray(current_height, dest_width_);
    PixelArray::Pixel new_pixel{};
    for (size_t i = 0; i < current_height; ++i) {
        for (size_t j = 0; j < dest_width_; ++j) {
            new_x = (j + 0.5) * delta_x - 0.5;
            new_pixel = ApplyLanczosX(image_pixels, new_x, alpha_, i);
            new_width_pixels(i, j) = new_pixel;
        }
    }
    PixelArray new_pixels = PixelArray(dest_height_, dest_width_);
    for (size_t i = 0; i < dest_height_; ++i) {
        for (size_t j = 0; j < dest_width_; ++j) {
            new_y = (i + 0.5) * delta_y - 0.5;
            new_pixel = ApplyLanczosY(new_width_pixels, new_y, alpha_, j);
            new_pixels(i, j) = new_pixel;
        }
    }
    image_pixels = new_pixels;
}

double LanczosScaleFilter::sinc(double x) {
    if (x == 0) {
        return 1;
    }
    return std::sin(x) / x;
}

double LanczosScaleFilter::Lanczos(double x, int alpha) {
    double t_abs = std::abs(x);
    if (t_abs < alpha) {
        return sinc(x * M_PI) * sinc(x * M_PI / alpha);
    }
    return 0;
}

PixelArray::Pixel LanczosScaleFilter::ApplyLanczosX(const PixelArray& image_pixels, double x, int alpha, size_t row) {
    int start = std::floor(x) - alpha + 1;
    int end = std::floor(x) + alpha + 1;
    PixelArray::Pixel current_pixel{};
    PixelArray::Pixel new_pixel{};
    int width = image_pixels.GetWidth();
    double red = 0;
    double green = 0;
    double blue = 0;
    double current_lanczos;
    for (int i = start; i < end; ++i) {
        current_pixel = image_pixels(row, std::clamp(i, 0, width - 1));
        current_lanczos = Lanczos(x - i, alpha);
        red += current_pixel.red * current_lanczos;
        green += current_pixel.green * current_lanczos;
        blue += current_pixel.blue * current_lanczos;
    }
    new_pixel.red = static_cast<size_t>(std::max(0, std::min(255, static_cast<int>(std::round(red)))));
    new_pixel.green = static_cast<size_t>(std::max(0, std::min(255, static_cast<int>(std::round(green)))));
    new_pixel.blue = static_cast<size_t>(std::max(0, std::min(255, static_cast<int>(std::round(blue)))));
    return new_pixel;
}

PixelArray::Pixel LanczosScaleFilter::ApplyLanczosY(const PixelArray& image_pixels, double y, int alpha, size_t column) {
    int start = std::floor(y) - alpha + 1;
    int end = std::floor(y) + alpha + 1;
    PixelArray::Pixel current_pixel{};
    PixelArray::Pixel new_pixel{};
    int height = image_pixels.GetHeight();
    double red = 0;
    double green = 0;
    double blue = 0;
    double current_lanczos;
    for (int i = start; i < end; ++i) {
        current_pixel = image_pixels(std::clamp(i, 0, height - 1), column);
        current_lanczos = Lanczos(y - i, alpha);
        red += current_pixel.red * current_lanczos;
        green += current_pixel.green * current_lanczos;
        blue += current_pixel.blue * current_lanczos;
    }
    new_pixel.red = static_cast<size_t>(std::max(0, std::min(255, static_cast<int>(std::round(red)))));
    new_pixel.green = static_cast<size_t>(std::max(0, std::min(255, static_cast<int>(std::round(green)))));
    new_pixel.blue = static_cast<size_t>(std::max(0, std::min(255, static_cast<int>(std::round(blue)))));
    return new_pixel;
}


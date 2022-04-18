#pragma once
#include "base_filter.h"
#include <cmath>
#include <vector>

namespace PixelMath {
    using Matrix = std::vector<std::vector<double>>;

    PixelArray::Pixel ApplyMatrix(const PixelArray& pixels, size_t i, size_t j, Matrix& matrix);

    Matrix TransposeMatrix(const Matrix& matrix);
}

class GaussianBlurFilter : public BaseFilter {
public:
    static const size_t PARAM_NUM = 1;

public:
    explicit GaussianBlurFilter(double sigma) {
        GenerateMatrix(sigma);
    }
    void Apply(Bitmap& image) override;

protected:
    void GenerateMatrix(double sigma);

protected:
    PixelMath::Matrix matrix_;
};

class CropFilter : public BaseFilter {
public:
    static const size_t PARAM_NUM = 2;

public:
    CropFilter(size_t width, size_t height) : width_(width), height_(height) {}

    void Apply(Bitmap& image) override;

    static void RotateImage(PixelArray& pixels);

protected:
    size_t width_;
    size_t height_;
};

class NegativeFilter : public BaseFilter {
public:
    void Apply(Bitmap& image) override;
};

class GrayscaleFilter : public BaseFilter {
public:
    const double RED_COEF = 0.299;
    const double GREEN_COEF = 0.587;
    const double BLUE_COEF = 0.114;

public:
    void Apply(Bitmap& image) override;

protected:
    uint8_t GetGrayscale(PixelArray::Pixel& pixel) const {
        return std::round(RED_COEF * pixel.red + GREEN_COEF * pixel.green + BLUE_COEF * pixel.blue);
    }
};

class SharpeningFilter : public BaseFilter {
public:
    void Apply(Bitmap& image) override;

protected:
    PixelArray::Pixel GetSharpenPixel(PixelArray& pixels, size_t i, size_t j);

protected:
    PixelMath::Matrix matrix_ = {{0, -1, 0},
                                 {-1, 5, -1},
                                 {0, -1, 0}};
};

class EdgeDetectionFilter : public GrayscaleFilter {
public:
    static const size_t PARAM_NUM = 1;

public:
    explicit EdgeDetectionFilter(double threshold) : threshold_(threshold) {}
    void Apply(Bitmap& image) override;

protected:
    double threshold_;
    PixelMath::Matrix matrix_ = {{0, -1, 0},
                                 {-1, 4, -1},
                                 {0, -1, 0}};
};

class LanczosScaleFilter : public BaseFilter {
public:
    static const int ALPHA = 3;
    static const size_t PARAM_NUM_WITH_ALPHA = 3;
    static const size_t PARAM_NUM_WO_ALPHA = 2;
public:
    LanczosScaleFilter(size_t dest_width, size_t dest_height, int alpha = ALPHA)
    : dest_width_(dest_width), dest_height_(dest_height), alpha_(alpha) {}
    void Apply(Bitmap& image) override;

protected:
    static PixelArray::Pixel ApplyLanczosX(const PixelArray& image_pixels, double x, int alpha, size_t row);
    static PixelArray::Pixel ApplyLanczosY(const PixelArray& image_pixels, double y, int alpha, size_t column);
    static double sinc(double x);
    static double Lanczos(double x, int alpha);

protected:
    size_t dest_width_;
    size_t dest_height_;
    double alpha_;
};
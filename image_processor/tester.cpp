#include <catch.hpp>
#include "bmp_wr.h"
#include "image.h"
#include "cmdcheckfunctions.h"
#include "crop.h"
#include "grayscale.h"
#include "negative.h"
#include "sharpening.h"
#include "edge.h"
#include "gauss.h"
#include "median.h"

#include <map>
#include <tuple>

void RequiredImageEqulity(Image& im1, Image& im2) {
    REQUIRE(im1.GetHeight() == im2.GetHeight());
    REQUIRE(im1.GetWidth() == im2.GetWidth());
    for (size_t i = 0; i < im1.GetHeight(); ++i) {
        for (size_t j = 0; j < im1.GetWidth(); ++j) {
            REQUIRE(im1[i][j] == im2[i][j]);
        }
    }
}

long double RMS(Image& im1, Image& im2) {
    std::unordered_map<int, int> histogram_r;
    std::unordered_map<int, int> histogram_g;
    std::unordered_map<int, int> histogram_b;
    for (size_t i = 0; i < im1.GetHeight(); ++i) {
        for (size_t j = 0; j < im1.GetWidth(); ++j) {
            if (!histogram_r.contains(abs(im1[i][j].r - im2[i][j].r))) {
                histogram_r[abs(im1[i][j].r - im2[i][j].r)] = 0;
            }
            if (!histogram_g.contains(abs(im1[i][j].g - im2[i][j].g))) {
                histogram_g[abs(im1[i][j].g - im2[i][j].g)] = 0;
            }
            if (!histogram_b.contains(abs(im1[i][j].b - im2[i][j].b))) {
                histogram_b[abs(im1[i][j].b - im2[i][j].b)] = 0;
            }
            histogram_r[abs(im1[i][j].r - im2[i][j].r)]++;
            histogram_g[abs(im1[i][j].g - im2[i][j].g)]++;
            histogram_b[abs(im1[i][j].b - im2[i][j].b)]++;
        }
    }
    double rrms = 0;
    double brms = 0;
    double grms = 0;
    for (auto [i, h] : histogram_r) {
        rrms += static_cast<double>(i * i) /
                static_cast<double>((im1.GetHeight() * im1.GetWidth()) * (im1.GetHeight() * im1.GetWidth())) *
                static_cast<double>(h);
    }
    for (auto [i, h] : histogram_b) {
        brms += static_cast<double>(i * i) /
                static_cast<double>((im1.GetHeight() * im1.GetWidth()) * (im1.GetHeight() * im1.GetWidth())) *
                static_cast<double>(h);
    }
    for (auto [i, h] : histogram_g) {
        grms += static_cast<double>(i * i) /
                static_cast<double>((im1.GetHeight() * im1.GetWidth()) * (im1.GetHeight() * im1.GetWidth())) *
                static_cast<double>(h);
    }
    rrms = sqrt(rrms);
    grms = sqrt(grms);
    brms = sqrt(brms);
    return std::max(rrms, std::max(grms, brms));
}

TEST_CASE("JPG") {
    try {
        ReadFile("../tasks/image_processor/test_script/data/asylum/jpgtest.jpg");
    } catch (const std::runtime_error& e) {
        return;
    }
    FAIL("Expected Wrong file format! Not a bmp!");
}

TEST_CASE("BROKEN BMP") {
    try {
        ReadFile("../tasks/image_processor/test_script/data/asylum/brokenfiletest.bmp");
    } catch (const std::runtime_error& e) {
        return;
    }
    FAIL("Expected Wrong file format! (certain bug)");
}

TEST_CASE("NOT 24 BMP") {
    try {
        ReadFile("../tasks/image_processor/test_script/data/asylum/32bit.bmp");
    } catch (const std::runtime_error& e) {
        return;
    }
    FAIL("Expected Wrong file format!Not 24bit bmp!");
}

TEST_CASE("CROP") {
    {
        const size_t w = 300;
        const size_t h = 300;
        Image im1 = ReadFile("../tasks/image_processor/test_script/data/test/testfile.bmp");
        Image im2 = ReadFile("../tasks/image_processor/test_script/data/test/crop1.bmp");
        Crop crop1(w, h);
        crop1.Use(im1);
        RequiredImageEqulity(im1, im2);
    }
    {
        const size_t w = 1000;
        const size_t h = 1000;
        Image im1 = ReadFile("../tasks/image_processor/test_script/data/test/testfile.bmp");
        Image im2 = ReadFile("../tasks/image_processor/test_script/data/test/crop2.bmp");
        Crop crop2(w, h);
        crop2.Use(im2);
        RequiredImageEqulity(im1, im2);
    }
}

TEST_CASE("EDGE") {
    const double thr = 0.5;
    Image im1 = ReadFile("../tasks/image_processor/test_script/data/test/testfile.bmp");
    Image im2 = ReadFile("../tasks/image_processor/test_script/data/test/edge.bmp");
    Edge edge(thr);
    edge.Use(im1);
    RequiredImageEqulity(im1, im2);
}

TEST_CASE("GRAYSCALE") {
    Image im1 = ReadFile("../tasks/image_processor/test_script/data/test/testfile.bmp");
    Image im2 = ReadFile("../tasks/image_processor/test_script/data/test/gs.bmp");
    Grayscale gs;
    gs.Use(im1);
    RequiredImageEqulity(im1, im2);
}

TEST_CASE("NEGATIVE") {
    Image im1 = ReadFile("../tasks/image_processor/test_script/data/test/testfile.bmp");
    Image im2 = ReadFile("../tasks/image_processor/test_script/data/test/neg.bmp");
    Negative neg;
    neg.Use(im1);
    RequiredImageEqulity(im1, im2);
}

TEST_CASE("SHARP") {
    Image im1 = ReadFile("../tasks/image_processor/test_script/data/test/testfile.bmp");
    Image im2 = ReadFile("../tasks/image_processor/test_script/data/test/sharp.bmp");
    Sharp sharp;
    sharp.Use(im1);
    RequiredImageEqulity(im1, im2);
}
TEST_CASE("GAUSS") {
    const double sigma = 1.5;
    const double rms = 1.;
    Image im1 = ReadFile("../tasks/image_processor/test_script/data/test/testfile2.bmp");
    Image im2 = ReadFile("../tasks/image_processor/test_script/data/test/gauss15.bmp");
    Blur blur(sigma);
    blur.Use(im1);
    REQUIRE(RMS(im1, im2) < rms);
}
TEST_CASE("MEDIAN") {
    const int r = 3;
    const double rms = 1.;
    Image im1 = ReadFile("../tasks/image_processor/test_script/data/test/testfile.bmp");
    Image im2 = ReadFile("../tasks/image_processor/test_script/data/test/median.bmp");
    Median median(r);
    median.Use(im1);
    REQUIRE(RMS(im1, im2) < rms);
}

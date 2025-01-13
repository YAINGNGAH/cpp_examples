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

#include <iostream>
#include <set>
#include <string>

// Юнит тесты покрывают неверные форматы файлов, проверку фильтров. Но не покрывают неправильные аргументы и фильтры
// передаваемые программе. Впрочем, из нижеизложенного кода парсера очевидно, что эти случаи покрываются.

const std::set<std::string> FILTERS = {"-crop", "-gs", "-neg", "-sharp", "-edge", "-blur", "-m"};
int main(int argc, char** argv) {
    try {
        if (argc <= 1) {
            std::cout << "./image_processor {path to original bmp file} {path to modified bmp file} {-crop width "
                         "length} {-m radius} {-blur sigma} {-edge threshold} {-gs} {-neg} {-sharp}"
                      << std::endl;
            std::cout << "crop - cut the picture" << std::endl;
            std::cout << "m - median filter, with low radius can clear some noise" << std::endl;
            std::cout << "blur - gaussian blur" << std::endl;
            std::cout << "edge - show edges" << std::endl;
            std::cout << "gs - make it grey" << std::endl;
            std::cout << "neg - inverse colors" << std::endl;
            std::cout << "sharp - make colors sharper" << std::endl;
            std::cout << "MADE BY TIRED MAN NAMED LAINFAG aka VALERIY BIGAEV eda221" << std::endl;
            std::cout << "          _..--¯¯¯¯--.._\n"
                      << "      ,-''              `-.\n"
                      << "    ,'                     `.\n"
                      << "   ,                         \\\n"
                      << "  /                           \\\n"
                      << " /          ′.                 \\\n"
                      << "'          /  ││                ;\n"
                      << ";       n /│  │/         │      │\n"
                      << "│      / v    /\\/`-'v√\\'.│\\     ,\n"
                      << ":    /v`,———         ————.^.    ;\n"
                      << "'   │  /′@@`,        ,@@ `\\│    ;\n"
                      << "│  n│  '.@@/         \\@@  /│\\  │;\n"
                      << "` │ `    ¯¯¯          ¯¯¯  │ \\/││\n"
                      << " \\ \\ \\                     │ /\\/\n"
                      << " '; `-\\          `′       /│/ │′\n"
                      << "  `    \\       —          /│  │\n"
                      << "   `    `.              .' │  │\n"
                      << "    v,_   `;._     _.-;    │  /\n"
                      << "       `'`\\│-_`'-''__/^'^' │ │   \n"
                      << "              ¯¯¯¯¯        │ │\n"
                      << "    ____ ____ ____ ____    │ /\n"
                      << "   ||l |||a |||i |||n ||   ││\n"
                      << "   ||__|||__|||__|||__||   ││\n"
                      << "   |/__\\|/__\\|/__\\|/__\\|   │," << std::endl;
            return 0;
        } else if (argc <= 2) {
            throw std::invalid_argument("You need at least paths to original file and to save");
        }
        char* origfile = argv[1];
        char* savefile = argv[2];
        for (size_t i = 3; i < static_cast<size_t>(argc); ++i) {
            std::string c(argv[i]);
            if (!FILTERS.contains(c)) {
                throw std::invalid_argument("There is no such filter, try again");
            } else {
                if (c == "-crop") {
                    if (i + 2 >= static_cast<size_t>(argc) || !IsNumber(std::string(argv[i + 1])) ||
                        !IsNumber(std::string(argv[i + 2]))) {
                        throw std::invalid_argument("There is no such filter, try again");
                    }
                    i += 2;
                }
                if (c == "-edge") {
                    if (i + 1 >= static_cast<size_t>(argc) || !IsDouble(std::string(argv[i + 1]))) {
                        throw std::invalid_argument("There is no such filter, try again");
                    }
                    ++i;
                }
                if (c == "-blur") {
                    if (i + 1 >= static_cast<size_t>(argc) || !IsDouble(std::string(argv[i + 1]))) {
                        throw std::invalid_argument("There is no such filter, try again");
                    }
                    ++i;
                }
                if (c == "-m") {
                    if (i + 1 >= static_cast<size_t>(argc) || !IsNumber(std::string(argv[i + 1]))) {
                        throw std::invalid_argument("There is no such filter, try again");
                    }
                    ++i;
                }
            }
        }
        Image image = ReadFile(origfile);
        for (size_t i = 3; i < static_cast<size_t>(argc); ++i) {
            std::string c(argv[i]);
            if (c == "-crop") {
                size_t w = std::stoi(argv[i + 1]);
                size_t h = std::stoi(argv[i + 2]);
                Crop filter(w, h);
                filter.Use(image);
                i += 2;
            } else if (c == "-edge") {
                double threshold = std::stod(argv[i + 1]);
                Edge filter(threshold);
                filter.Use(image);
                ++i;
            } else if (c == "-blur") {
                double sigma = std::stod(argv[i + 1]);
                Blur filter(sigma);
                filter.Use(image);
                ++i;
            } else if (c == "-m") {
                int r = std::stoi(argv[i + 1]);
                Median filter(r);
                filter.Use(image);
                ++i;
            } else if (c == "-gs") {
                Grayscale filter;
                filter.Use(image);
            } else if (c == "-neg") {
                Negative filter;
                filter.Use(image);
            } else if (c == "-sharp") {
                Sharp filter;
                filter.Use(image);
            }
        }
        SaveFile(image, savefile);
    } catch (const std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
        return 1;
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Unexpected Error, please contact me about what have you done" << std::endl;
        return 1;
    }

    return 0;
}

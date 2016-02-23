#include <iostream>
#include "io/PgmIO.hpp"
#include "matrix/Matrix.hpp"
#include "improc/Improc.hpp"

using namespace sipl;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "    " << argv[0] << " infile outfile" << std::endl;
        std::exit(1);
    }

    const std::string infile{argv[1]};
    const std::string outfile{argv[2]};

    auto mat = PgmIO::read(infile);

    // Draw the normalized histogram
    auto hist = histogram(mat);
    auto normalized_hist = hist / double(hist.max());
    MatrixX<uint8_t> hist_plot(256, 256);
    for (int32_t i = 0; i < hist_plot.dims[0]; ++i) {
        int32_t count = int32_t(normalized_hist[i] * 256);
        for (int32_t j = 0; j < hist_plot.dims[1]; ++j) {
            hist_plot(i, j) =
                (j < count ? std::numeric_limits<uint8_t>::max() : 0);
        }
    }

    // Rotate +90 degrees to orient it correctly
    hist_plot = rotate_image(hist_plot, 90, InterpolateType::BILINEAR);

    // Output to file
    PgmIO::write(hist_plot, outfile);
};

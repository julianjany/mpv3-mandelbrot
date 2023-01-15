#include <iostream>

#include "colormap.h"
#include "pfc/bitmap.h"
#include "pfc/chrono.h"
#include "pfc/jobs.h"

using real_type = float;
using dim_t = size_t;

struct image_dimensions_t {
  dim_t width;
  dim_t height;
};

struct image_coordinate_t {
  dim_t x;
  dim_t y;
};

struct coordinate_transformer_t {
  coordinate_transformer_t(pfc::complex<real_type> const& lower_left,
                           pfc::complex<real_type> const& upper_right,
                           image_dimensions_t const render_dimensions)
      : m_x_left{lower_left.real},
        m_y_top{upper_right.imag},
        m_dx{(upper_right.real - lower_left.real) / render_dimensions.width},
        m_dy{(lower_left.imag - upper_right.imag) / render_dimensions.height} {}

  pfc::complex<real_type> operator()(
      image_coordinate_t const image_coord) const {
    return pfc::complex<real_type>(m_x_left + m_dx * image_coord.x,
                                   m_y_top + m_dy * image_coord.y);
  }

  real_type const m_x_left;
  real_type const m_y_top;
  real_type const m_dx;
  real_type const m_dy;
};

bool outside_mandelbrot(pfc::complex<real_type> const& coord) {
  return (coord.real * coord.real + coord.imag * coord.imag) >
         static_cast<real_type>(4);
}

int calc_mandelbrot(pfc::complex<real_type> const& initial_coord,
                    int const max_iterations) {
  pfc::complex<real_type> current_coord{0, 0};
  int iteration{0};
  while (!outside_mandelbrot(current_coord) && iteration < max_iterations) {
    current_coord.square() += initial_coord;
    ++iteration;
  }
  return iteration;
}

void run_job(pfc::jobs<real_type>::job_t const& job,
             image_dimensions_t const render_dimensions) {
  auto& [lower_left, upper_right, center, dimensions]{job};

  auto const to_coord{
      coordinate_transformer_t{lower_left, upper_right, render_dimensions}};
  pfc::bitmap bitmap{render_dimensions.width, render_dimensions.height};
  for (dim_t y = 0; y < render_dimensions.height; ++y) {
    for (dim_t x = 0; x < render_dimensions.width; ++x) {
      auto const coord{to_coord({x, y})};
      auto const mandelbrot_value{
          calc_mandelbrot(coord, mandelbrot_max_iterations)};
      bitmap.at(x, y) = colormap[mandelbrot_value];
    }
  }
  bitmap.to_file("test.bmp");
}

int main(int argc, char const* argv[]) {
  auto const filename{pfc::jobs<real_type>::make_filename(4)};
  auto const filepath{std::format("jobs\\{}", filename)};
  auto const jobs{pfc::jobs<real_type>(filepath)};

  for (auto& job : jobs) {
    auto const elapsed_time{pfc::timed_run([&job]() {
      // run_job(job, {8192, 4608});
      run_job(job, {1640, 920});
    })};
    auto const millis{
        std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time)};
    std::cout << millis.count() << "ms\n";
  }
}
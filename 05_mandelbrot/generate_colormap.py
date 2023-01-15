import matplotlib.pyplot as plt

cmap = plt.cm.get_cmap('magma')

max_value = 128

def color_to_str(color: tuple[float, float, float, float]) -> str:
  rf, gf, bf, _ = color
  return '{' + f'{int(255*bf)}, {int(255*gf)}, {int(255*rf)}' + '}'

with open('colormap.h', mode='w') as file:
  file.write('#pragma once\n')
  file.write('// this file is generated; do not change!\n\n')
  file.write('#include "pfc/bitmap.h"\n\n')
  file.write(f'static constexpr int mandelbrot_max_iterations{{{max_value}}};\n\n')
  file.write('static constexpr pfc::bmp::pixel_t colormap[]{\n')
  for i in range(max_value):
    value = i / (max_value - 1)
    color = cmap(value)
    file.write('    ' + color_to_str(color) + ',\n')
  file.write('    {0, 0, 0}\n')
  file.write('};\n')
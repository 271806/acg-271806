#define PROJECT_SOURCE_DIR "E:/Dev/ACG2024/acg-271806/task01"

#include <cstdio>
#include <iostream>
#include <cassert>
#include <vector>
#include <filesystem>
//
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/**
 * @brief compute the area of a triangle
 * @return area
 */
float area_of_a_triangle(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2) {
  return ((x1 - x2) * (y0 - y2) - (x0 - x2) * (y1 - y2)) * 0.5f;
}

/**
 * @brief draw a triangle using the barycentric coordinates
 */
void draw_triangle(
    float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    std::vector<unsigned char> &img_data, unsigned int width, unsigned int height,
    unsigned char brightness) {
  for (unsigned int ih = 0; ih < height; ++ih) {
    for (unsigned int iw = 0; iw < width; ++iw) {
      const auto x = (float) iw + 0.5f;
      const auto y = (float) ih + 0.5f;
      const auto a01 = area_of_a_triangle(x, y, x0, y0, x1, y1);
      const auto a12 = area_of_a_triangle(x, y, x1, y1, x2, y2);
      const auto a20 = area_of_a_triangle(x, y, x2, y2, x0, y0);
      if (a01 > 0.f && a12 > 0.f && a20 > 0.f) {
        img_data[ih * height + iw] = brightness;
      }
    }
  }
}

/**
 * @brief draw a triangle using the barycentric coordinates
 * @param polygon_xy xy coordinates of the corners of the polygon (counter clockwise order)
 * @param brightness brightness of the painted pixel
 */
void draw_polygon(
    const std::vector<float> &polygon_xy,
    std::vector<unsigned char> &img_data, unsigned int width, unsigned int height,
    unsigned int brightness) {
  for (unsigned int ih = 0; ih < height; ++ih) {
    for (unsigned int iw = 0; iw < width; ++iw) {
      const auto x = float(iw) + 0.5f; // x-coordinate of the center of the pixel
      const auto y = float(ih) + 0.5f; // y-coordinate of the center of the pixel
      const unsigned int num_vtx = polygon_xy.size() / 2;
      float winding_number = 0.0;
      for (unsigned int iedge = 0; iedge < num_vtx; ++iedge) {
        unsigned int i0_vtx = iedge;
        unsigned int i1_vtx = (iedge + 1) % num_vtx;
        // positions of the end points of the edge relative to (x,y)
        float p0x = polygon_xy[i0_vtx * 2 + 0] - x;
        float p0y = polygon_xy[i0_vtx * 2 + 1] - y;
        float p1x = polygon_xy[i1_vtx * 2 + 0] - x;
        float p1y = polygon_xy[i1_vtx * 2 + 1] - y;
        // write a few lines of code to compute winding number (hint: use atan2)
        
        // MY IMPLEMENTATION ***********************
        // Calculate the dot and cross product terms for atan2
        /*
        The atan2 function internally handles the normalization of the input vectors,
         so we don't compute and divide by the magnitudes here
        */
        float dot = p0x * p1x + p0y * p1y; // dot product of p and q
        float det = p0x * p1y - p0y * p1x; // cross product of p and q, determinant
        
        // Use atan2 to compute the angle as lecture told
        float angle = atan2(det, dot);
        
        // accumulate
        winding_number += angle;
      }
      // devide by 2*PI
      winding_number /= 2 * 3.14159265358979323846;
      const int int_winding_number = int(std::round(winding_number));

      // I changed the condition to check if the winding number is not equal to 0
      if (int_winding_number != 0) { // if (x,y) is inside the polygon
        img_data[ih*width + iw] = brightness;
      }
    }
  }
}

/* 
   I know it's not neccessary, but
   I also implemented the accelerated version of the draw_polygon function,
   by using the bounding box of the polygon to reduce the number of pixels to check,
   like you told in the lecture.
   
   But it is not used in the main function.
   You can uncomment the draw_polygon_accelerated to try it.
*/

void draw_polygon_accelerated(
    const std::vector<float>& polygon_xy,
    std::vector<unsigned char>& img_data, unsigned int width, unsigned int height,
    unsigned int brightness) {
  const unsigned int num_vtx = polygon_xy.size() / 2;

  // find the bounding box of the polygon
  float min_x = polygon_xy[0], max_x = polygon_xy[0];
  float min_y = polygon_xy[1], max_y = polygon_xy[1];
  for (unsigned int i = 1; i < num_vtx; ++i) {
    float x = polygon_xy[i * 2];
    float y = polygon_xy[i * 2 + 1];
    min_x = std::min(min_x, x);
    max_x = std::max(max_x, x);
    min_y = std::min(min_y, y);
    max_y = std::max(max_y, y);
  }

  // convert bounding box coordinates to pixel coordinates
  int ix_min = std::max(0, (int)std::floor(min_x));
  int ix_max = std::min((int)width - 1, (int)std::ceil(max_x));
  int iy_min = std::max(0, (int)std::floor(min_y));
  int iy_max = std::min((int)height - 1, (int)std::ceil(max_y));

  // iterate over within the bounding box
  for (int iy = iy_min; iy <= iy_max; ++iy) {
    for (int ix = ix_min; ix <= ix_max; ++ix) {
      const auto x = float(ix) + 0.5f; // x-coordinate of the center of the pixel
      const auto y = float(iy) + 0.5f; // y-coordinate of the center of the pixel
      float winding_number = 0.0;
      for (unsigned int iedge = 0; iedge < num_vtx; ++iedge) {
        unsigned int i0_vtx = iedge;
        unsigned int i1_vtx = (iedge + 1) % num_vtx;
        // positions of the end points of the edge relative to (x,y)
        float p0x = polygon_xy[i0_vtx * 2 + 0] - x;
        float p0y = polygon_xy[i0_vtx * 2 + 1] - y;
        float p1x = polygon_xy[i1_vtx * 2 + 0] - x;
        float p1y = polygon_xy[i1_vtx * 2 + 1] - y;
        
        // calculate the dot and cross product terms for atan2
        float dot = p0x * p1x + p0y * p1y;
        float det = p0x * p1y - p0y * p1x;
        
        // Use atan2 to compute the angle
        float angle = atan2(det, dot);
        
        // acumulate
        winding_number += angle;
      }
      // devide by 2*PI
      winding_number /= 2 * 3.14159265358979323846;
      const int int_winding_number = int(std::round(winding_number));
      if (int_winding_number != 0) { // if (x,y) is inside the polygon
        img_data[iy*width + ix] = brightness;
      }
    }
  }
}

/**
 * @brief draw a line using DDA algorithm
 * @param x0 x-coordinate of the first end point
 * @param y0 y-coordinate of the first end point
 * @param x1 x-coordinate of the second end point
 * @param y1 y-coordinate of the second end point
 * @param brightness brightness of the painted pixel
 */
void dda_line(
    float x0, float y0,
    float x1, float y1,
    std::vector<unsigned char> &img_data,
    unsigned int width,
    unsigned char brightness ) {
  auto dx = x1 - x0;
  auto dy = y1 - y0;
  // write some code below to paint pixel on the line with color `brightness`

  // MY IMPLEMENTATION **********************
  // calculate the absolute
  float abs_dx = std::abs(dx);
  float abs_dy = std::abs(dy);

  // calculate the number of steps required to paint the line
  // the number of steps is the maximum of the absolute differences in x and y (use larger one)
  int steps = (int)std::max(abs_dx, abs_dy);

  // for each step, calculate the x and y increments
  float x_incr = dx / steps;
  float y_incr = dy / steps;

  // initialize the current x and y coordinates
  float x = x0;
  float y = y0;

  // start to paint the line, iterating..
  for (int i = 0; i <= steps; ++i) {
    int ix = std::round(x);
    int iy = std::round(y);

    // check bounds
    if (ix >= 0 && ix < width && iy >= 0 && iy < width) {
      // paint the pixel
      img_data[iy * width + ix] = brightness;
    }

    // step to the next
    x += x_incr;
    y += y_incr;
  }
}

int main() {
  constexpr unsigned int width = 100;
  constexpr unsigned int height = 100;
  std::vector<unsigned char> img_data(width * height, 255); // white initial image
  draw_triangle(5., 5., 15., 45., 45., 15., img_data, width, height, 128);
  draw_polygon(
      {55., 5.,75., 75.,15., 55.,15., 95.,95., 95.,95., 5.}, // the pix coordinates of the corners of the polygon
      img_data, width, height, // image data, size
      200); // brightness to draw
  // draw_polygon_accelerated(
  //   {55., 5.,75., 75.,15., 55.,15., 95.,95., 95.,95., 5.}, // the pix coordinates of the corners of the polygon
  //   img_data, width, height, // image data, size
  //   200); // brightness to draw
  dda_line(50.0, 50.0, 72.8, 10.0, img_data, width, 0); // right-down
  dda_line(50.0, 50.0, 90.0, 27.2, img_data, width, 0); // right-down
  dda_line(50.0, 50.0, 72.8, 90.0, img_data, width, 0); // right-up
  dda_line(50.0, 50.0, 90.0, 72.8, img_data, width, 0); // right-up
  dda_line(50.0, 50.0, 10.0, 72.8, img_data, width, 0); // left-up
  dda_line(50.0, 50.0, 27.2, 90.0, img_data, width, 0); // left-up
  dda_line(50.0, 50.0, 10.0, 27.2, img_data, width, 0); // left-down
  dda_line(50.0, 50.0, 27.2, 10.0, img_data, width, 0); // left-down
  dda_line(50.0, 50.0, 90.0, 50.0, img_data, width, 0); // right
  dda_line(50.0, 50.0, 50.0, 90.0, img_data, width, 0); // up
  dda_line(50.0, 50.0, 10.0, 50.0, img_data, width, 0); // left
  dda_line(50.0, 50.0, 50.0, 10.0, img_data, width, 0); // down
  // save gray scale image with 1 byte depth
  stbi_write_png(
      (std::filesystem::path(PROJECT_SOURCE_DIR) / "output_yinfeiqian.png").string().c_str(),
      width, height, 1,img_data.data(), width);
}

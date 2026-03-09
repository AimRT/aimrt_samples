#pragma once
#include <atomic>
#include <chrono>
#include <mutex>
#include <ruckig/ruckig.hpp>
#include "control_module/controller/types.h"

using namespace std::chrono;

// 节流器
bool Throttler(const time_point<high_resolution_clock> now, time_point<high_resolution_clock> &last,
               const milliseconds interval);

// 滤波器
template <typename T>
class filter {
 public:
  filter(void) {}
  virtual ~filter(void) {}
  virtual void input(T input_value) = 0;
  virtual T output(void) = 0;
  virtual void clear(void) = 0;
};

// 数字低通滤波器
template <typename T>
class digital_lp_filter : public filter<T> {
 public:
  digital_lp_filter(T w_c, T t_s);
  virtual ~digital_lp_filter(void);
  virtual void input(T input_value);
  virtual T output(void);
  virtual void update(void);
  virtual void set_wc(T w_c);
  virtual void set_ts(T t_s);
  virtual void clear(void);
  virtual void init(T init_data);

 private:
  T Lpf_in_prev[2];
  T Lpf_out_prev[2];
  T Lpf_in1, Lpf_in2, Lpf_in3, Lpf_out1, Lpf_out2;
  T lpf_out;
  T wc;
  T ts;
};

// 插值器
class Interpolator {
 public:
  Interpolator() = default;
  // 输入：需要插值的数组 trans_time data1 data2 data3 ...
  void Init(const std::vector<std::vector<double>> &to_interpolate_data);
  // 获取当前行所有列的插值点
  bool GetNextPoint(std::vector<double> &next_points);

 private:
  std::vector<std::vector<double>> linearInterpolate(std::vector<double> start,
                                                     std::vector<double> end, double duration);
  std::vector<std::vector<double>> RuckigInterpolate(std::vector<double> start,
                                                     std::vector<double> end, double duration);

 private:
  std::vector<std::vector<double>> data_;
  size_t current_row_ = 0;
  std::atomic_bool construct_flag_ = false;
};

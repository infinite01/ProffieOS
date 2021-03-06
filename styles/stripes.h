#ifndef STYLES_STRIPES_H
#define STYLES_STRIPES_H

#include "../functions/int.h"

// Usage: Stripes<WIDTH, SPEED, COLOR1, COLOR2, ... >
// or: Usage: StripesX<WIDTH_CLASS, SPEED, COLOR1, COLOR2, ... >
// WIDTH: integer (start with 1000, then adjust up or down)
// WIDTH_CLASS: INTEGER
// SPEED: integer  (start with 1000, then adjust up or down)
// COLOR1, COLOR2: COLOR
// return value: COLOR
// Works like rainbow, but with any colors you like.
// WIDTH determines width of stripes
// SPEED determines movement speed

template<class... A>
class StripesHelper {};
  
template<>
class StripesHelper<> {
public:
  static const size_t size = 0;
  void run(BladeBase* blade) {}
  void get(int led, int p, OverDriveColor* c) {}
};

template<class A, class... B>
class StripesHelper<A, B...> {
public:
  static const size_t size = StripesHelper<B...>::size + 1;
  void run(BladeBase* blade) {
    a_.run(blade);
    b_.run(blade);
  }
  void get(int led, int p, OverDriveColor* ret) {
    if (p > 0 && p < 512) {
      OverDriveColor tmp = a_.getColor(led);
      int mul = sin_table[p];
      ret->c.r += (tmp.c.r * mul) >> 14;
      ret->c.g += (tmp.c.g * mul) >> 14;
      ret->c.b += (tmp.c.b * mul) >> 14;
    }
    b_.get(led, p - 341, ret);
  }
  A a_;
  StripesHelper<B...> b_;
};


template<class WIDTH, class SPEED, class... COLORS>
class StripesX {
public:
  void run(BladeBase* base) {
    colors_.run(base);
    width_.run(base);
    colors_.run(base);
    
    uint32_t now_micros = micros();
    uint32_t delta_micros = now_micros - last_micros_;
    last_micros_ = now_micros;
    m = (m + delta_micros * speed_.getInteger(0) / 333) % (colors_.size * 341*1024);
    mult_ = (50000*1024 / width_.getInteger(0));
  }
  OverDriveColor getColor(int led) {
    // p = 0..341*len(colors)
    int p = ((m + led * mult_) >> 10) % (colors_.size * 341);
    
    OverDriveColor ret;
    ret.overdrive = false;
    ret.c = Color16(0,0,0);
    colors_.get(led, p, &ret);
    colors_.get(led, p - 341 * colors_.size, &ret);
    return ret;
  }
private:
  StripesHelper<COLORS...> colors_;
  WIDTH width_;
  uint32_t mult_;
  SPEED speed_;
  uint32_t last_micros_;
  uint32_t m;
};

template<int WIDTH, int SPEED, class... COLORS>
  using Stripes = StripesX<Int<WIDTH>, Int<SPEED>, COLORS...>;
#endif

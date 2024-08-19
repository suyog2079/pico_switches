#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define team_color_in 17
#define reset_switch 15
#define zone 16
#define run_switch 14
#define red_led 11
#define green_led 10
#define blue_led 9

#define strategy1 20
#define strategy2 19
#define strategy3 18
#define internal_led 25

void init_gpios() {
  stdio_init_all();

  // initialize led strips
  gpio_init(red_led);
  gpio_init(green_led);
  gpio_init(blue_led);
  gpio_init(internal_led);

  // initialize switches
  gpio_init(reset_switch);
  gpio_init(run_switch);
  gpio_init(zone);
  gpio_init(team_color_in);

  // initialize strategy switches
  gpio_init(strategy1);
  gpio_init(strategy2);
  gpio_init(strategy3);

  // set input/output port
  gpio_set_dir(red_led, GPIO_OUT);
  gpio_set_dir(green_led, GPIO_OUT);
  gpio_set_dir(blue_led, GPIO_OUT);
  gpio_set_dir(reset_switch, GPIO_IN);
  gpio_set_dir(team_color_in, GPIO_IN);
  gpio_set_dir(run_switch, GPIO_IN);
  gpio_set_dir(zone, GPIO_IN);
  gpio_set_dir(strategy1, GPIO_IN);
  gpio_set_dir(strategy2, GPIO_IN);
  gpio_set_dir(strategy3, GPIO_IN);
  gpio_set_dir(internal_led, GPIO_OUT);

  // set run switch to input pulldown and reset switch to input pullup
  gpio_set_pulls(run_switch, false, true);
  gpio_set_pulls(reset_switch, true, false);
}

void start_sequence() {
  gpio_put(red_led, 1);
  gpio_put(blue_led, 0);
  gpio_put(green_led, 0);
  sleep_ms(200);
  gpio_put(red_led, 0);
  gpio_put(blue_led, 1);
  gpio_put(green_led, 0);
  sleep_ms(200);
  gpio_put(red_led, 0);
  gpio_put(blue_led, 0);
  gpio_put(green_led, 1);
  sleep_ms(200);
  gpio_put(red_led, 1);
  gpio_put(blue_led, 1);
  gpio_put(green_led, 0);
  sleep_ms(200);
  gpio_put(red_led, 1);
  gpio_put(blue_led, 0);
  gpio_put(green_led, 1);
  sleep_ms(200);
  gpio_put(red_led, 0);
  gpio_put(blue_led, 1);
  gpio_put(green_led, 1);
  sleep_ms(200);
  gpio_put(red_led, 1);
  gpio_put(blue_led, 1);
  gpio_put(green_led, 1);
  sleep_ms(200);
  gpio_put(green_led, 1);
  gpio_put(blue_led, 0);
  gpio_put(red_led, 0);
}

char get_state() {
  char data = 0;
  bool changed = false;
  if (gpio_get(team_color_in)) {
    data ^= 0b10000000;
    changed = true;
  }
  if (gpio_get(zone)) {
    data ^= 0b01000000;
    changed = true;
  }
  if (gpio_get(run_switch)) {
    data ^= 0b00100000;
    changed = true;
  }
  if (gpio_get(reset_switch)) {
    // counter_red = 0;
    data ^= 0b00000000;
    changed = true;
  } else {
    data ^= 0b00010000;
    changed = true;
  }
  if (gpio_get(strategy1)) {
    data ^= 0b00001000;
    changed = true;
  }
  if (gpio_get(strategy2)) {
    data ^= 0b00000100;
    changed = true;
  }
  if (gpio_get(strategy3)) {
    data ^= 0b00000010;
    changed = true;
  }
  if (changed) {
    return data;
  } else {
    return 255;
  }
}

void process_data(char byte) {
  if (byte == 0x01) {
    gpio_put(green_led, 1);
    gpio_put(blue_led, 1);
    gpio_put(red_led, 0);
  }
  if (byte == 0x02) {
    gpio_put(green_led, 1);
    gpio_put(blue_led, 0);
    gpio_put(red_led, 1);
  }
  if (byte == 0x03) {
    gpio_put(green_led, 0);
    gpio_put(blue_led, 1);
    gpio_put(red_led, 1);
  }
  if (byte == 0x04) {
    gpio_put(green_led, 1);
    gpio_put(blue_led, 1);
    gpio_put(red_led, 1);
  }
  if (byte == 0x05) {
    gpio_put(green_led, 1);
    gpio_put(blue_led, 0);
    gpio_put(red_led, 0);
  }
  if (byte == 0x06) {
    gpio_put(green_led, 0);
    gpio_put(blue_led, 0);
    gpio_put(red_led, 1);
  }
  if (byte == 0x07) {
    gpio_put(green_led, 0);
    gpio_put(blue_led, 1);
    gpio_put(red_led, 0);
  }
  if (byte == 0x08) {
    gpio_put(green_led, 1);
    gpio_put(blue_led, 0);
    gpio_put(red_led, 0);
  }
}

// const char *bit_rep[16] = {
//     [0] = "0000",  [1] = "0001",  [2] = "0010",  [3] = "0011",
//     [4] = "0100",  [5] = "0101",  [6] = "0110",  [7] = "0111",
//     [8] = "1000",  [9] = "1001",  [10] = "1010", [11] = "1011",
//     [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
// };

// void print_byte(uint8_t byte) {
//   printf("%s%s\n", bit_rep[byte >> 4], bit_rep[byte & 0x0F]);
// }

int main() {
  init_gpios();
  start_sequence();
  char start_byte = 0xA5;
  char recev_byte;
  gpio_put(internal_led, 1);
  while (true) {
    char to_send = get_state();
    // print_byte(to_send);
    printf("%c%c", start_byte, to_send);
    uint16_t temp = getchar_timeout_us(20000);
    if (temp >= 0 && temp <= 255) {
      process_data((char)temp);
    }
  }
}

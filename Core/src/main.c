#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define sensor1 19
#define sensor2 20
#define sensor3 21
#define sensor4 22

#define team_color_in 14
#define reset_switch 10
#define reset_led 11
#define zone 13
#define run_switch 6
#define run_led 5
#define red_led 26
#define green_led 27
#define blue_led 28

#define strategy1 0
#define strategy2 3
#define strategy3 4

void init_gpios() {
  stdio_init_all();

  // initialize sensors
  gpio_init(sensor1);
  gpio_init(sensor2);
  gpio_init(sensor3);
  gpio_init(sensor4);

  // initialize led strips
  gpio_init(red_led);
  gpio_init(green_led);
  gpio_init(blue_led);

  // initialize switches
  gpio_init(reset_switch);
  gpio_init(run_switch);
  gpio_init(zone);
  gpio_init(team_color_in);

  // initialize strategy switches
  gpio_init(strategy1);
  gpio_init(strategy2);
  gpio_init(strategy3);

  // initialize reset/run switches
  gpio_init(reset_led);
  gpio_init(run_led);

  // set input/output port
  gpio_set_dir(sensor1, GPIO_IN);
  gpio_set_dir(sensor2, GPIO_IN);
  gpio_set_dir(sensor3, GPIO_IN);
  gpio_set_dir(sensor4, GPIO_IN);
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
  gpio_set_dir(reset_led, GPIO_OUT);
  gpio_set_dir(run_led, GPIO_OUT);

  // set the input pins to be pulldown
  gpio_set_pulls(run_switch, false, true);
  gpio_set_pulls(reset_switch, false, true);

  // initialize initial state
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
  gpio_put(reset_led, 0);
  gpio_put(run_led, 0);
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
    data ^= 0b00010000;
    changed = true;
  }
  if (changed) {
    return data;
  } else {
    return 255;
  }
}

void process_data(char first_byte, char second_byte) {
  if (second_byte == 0) {
    if (to_recev & 0b10000000) {
      gpio_put(green_led, 0);
      gpio_put(red_led, 1);
      gpio_put(blue_led, 0);
    }
    if (to_recev & 0b01000000) {
      gpio_put(green_led, 0);
      gpio_put(red_led, 0);
      gpio_put(blue_led, 1);
    }
    if (to_recev == 0) {
      gpio_put(green_led, 1);
      gpio_put(red_led, 0);
      gpio_put(blue_led, 0);
    }
  } else {
    if (second_byte == 1) {
      gpio_put(green_led, 1);
      gpio_put(red_led, 1);
      gpio_put(blue_led, 0);
    }
    if (second_byte == 2) {
      gpio_put(green_led, 0);
      gpio_put(red_led, 1);
      gpio_put(blue_led, 1);
    }
    if (second_byte == 3) {
      gpio_put(green_led, 1);
      gpio_put(red_led, 0);
      gpio_put(blue_led, 1);
    }
    if (second_byte == 4) {
      gpio_put(green_led, 1);
      gpio_put(red_led, 1);
      gpio_put(blue_led, 1);
    }
    if (second_byte == 5) {
      gpio_put(green_led, 1);
      gpio_put(red_led, 0);
      gpio_put(blue_led, 0);
    }
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
  char first_byte, second_byte;
  while (true) {
    char to_send = get_state();
    printf("%c%c", start_byte, to_send);
    absolute_time_t tim = get_absolute_time();
    scanf("%c%c", &first_byte, &second_byte);
    while ((get_absolute_time() - tim) <= 20000) {
    }
  }
}

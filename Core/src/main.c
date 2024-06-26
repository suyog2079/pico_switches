#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define sensor1 19
#define sensor2 20
#define sensor3 21
#define sensor4 22

#define team_color_in 14
#define reset_switch 10
#define start_restart_switch 13
#define run_switch 6 

#define uartid uart0
#define baudrate 115200

#define uart_rx_pin 17
#define uart_tx_pin 16

#define strategy1 0
#define strategy2 1
#define strategy3 2
#define strategy4 3
#define strategy5 4

#define cstate(x) 1<<x;

enum states
{
  start_restart,
  team_color,
  reset,
  run,
  in1,
  in2,
  in3,
  in4,
};


typedef struct
{
  uint8_t start_byte;
  uint8_t state;
  uint8_t team;
  uint8_t crc_hash;
}message;

message msg;

uint8_t state = 0x00;
uint8_t team = 0x00;

void init_uart()
{
  uart_init(uartid,baudrate);

  gpio_set_function(uart_rx_pin,GPIO_FUNC_UART);
  gpio_set_function(uart_tx_pin,GPIO_FUNC_UART);
}

void init_gpios()
{
  //initialize sensors
  gpio_init(sensor1);
  gpio_init(sensor2);
  gpio_init(sensor3);
  gpio_init(sensor4);

  gpio_set_dir(sensor1,GPIO_IN);
  gpio_set_dir(sensor2,GPIO_IN);
  gpio_set_dir(sensor3,GPIO_IN);
  gpio_set_dir(sensor4,GPIO_IN);

  //initialize switches
  gpio_init(reset_switch);
  gpio_init(run_switch);
  gpio_init(start_restart_switch);
  gpio_init(team_color_in);

  gpio_set_dir(reset_switch,GPIO_IN);
  gpio_set_dir(team_color_in,GPIO_IN);
  gpio_set_dir(run_switch,GPIO_IN);
  gpio_set_dir(start_restart_switch,GPIO_IN);

  //initialize team for strategy
  gpio_init(strategy1);
  gpio_init(strategy2);
  gpio_init(strategy3);
  gpio_init(strategy4);
  gpio_init(strategy5);

  gpio_set_dir(strategy1,GPIO_IN);
  gpio_set_dir(strategy2,GPIO_IN);
  gpio_set_dir(strategy3,GPIO_IN);
  gpio_set_dir(strategy4,GPIO_IN);
  gpio_set_dir(strategy5,GPIO_IN);
}

void get_state()
{
  static int run_counter;
  static int reset_counter;
  state = 0x00;
  if (gpio_get(sensor1)) msg.state ^= cstate(in1);
  if (gpio_get(sensor2)) msg.state ^= cstate(in2);
  if (gpio_get(sensor3)) msg.state ^= cstate(in3);
  if (gpio_get(sensor4)) msg.state ^= cstate(in4);
  if (gpio_get(run_switch)) run_counter = 1;
  if (run_counter) {
	run_counter++;
	msg.state ^= cstate(run);
	if(run_counter > 50) run_counter = 0;
  }
  if (gpio_get(start_restart_switch)) msg.state ^= cstate(start_restart);
  if (gpio_get(team_color_in)) msg.state ^= cstate(team_color);
  if (gpio_get(reset_switch)) reset_counter = 1;
  if (reset_counter) {
	reset_counter++;
	msg.state ^= cstate(reset);
	if(reset_counter > 50) reset_counter = 0;
  }
}

void get_team()
{
  if(gpio_get(strategy1)) msg.team ^= cstate(strategy1); 
  if(gpio_get(strategy2)) msg.team ^= cstate(strategy2); 
  if(gpio_get(strategy3)) msg.team ^= cstate(strategy3); 
  if(gpio_get(strategy4)) msg.team ^= cstate(strategy4); 
  if(gpio_get(strategy5)) msg.team ^= cstate(strategy5); 
}

int main()
{
  uint8_t to_send[4];
  init_gpios();
  init_uart();
  get_state();
  get_team();

  memcpy(to_send,(uint8_t *)&msg,4);
  uart_write_blocking(uartid,to_send,sizeof(message));
}

#include <stdint.h>
#include <stdio.h>
#include <lcom/lcf.h>


enum states{
  INITIAL,
  DRAW_LEFT,
  LEFT_DONE,
  VERTEX,
  DRAW_RIGHT,
  RIGHT_DONE,
  EXIT
};

enum event_t{
  RIGHT_DOWN,
  RIGHT_UP,
  LEFT_DOWN,
  LEFT_UP,
  TOL_FAIL,
  LINE_C,
  CONTINUE_DRAW
};


typedef struct
{
    int32_t x_desl;
    int32_t y_desl;
    uint32_t x_tol_sum;
    uint32_t y_tol_sum;
    bool lb;
    bool rb;
} data;

typedef struct
{
    int32_t x_desl;
    int32_t y_desl;
} deslocation;


/**
 * 
 */
enum states (handle_mouse_event)(struct packet * pkt, uint8_t tol, uint8_t x_len);

enum event_t (packet_to_event)(struct packet  * pkt, uint8_t tol, uint8_t x_len);

void (init_data)();

void (reset_data)();

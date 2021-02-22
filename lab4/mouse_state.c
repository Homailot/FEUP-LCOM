#include "mouse_state.h"
#include "lcom/lab4.h"
#include "i8042.h"

data currData;
enum states currState;
deslocation line_start_packet;
size_t packet_count = 0;

void (init_data)(){
  currData.rb = false;
  currData.lb = false;
  currData.x_desl = 0;
  currData.y_desl = 0;
  currData.x_tol_sum = 0;
  currData.y_tol_sum = 0;
}

void (reset_data)(){
  currData.x_desl = 0;
  currData.y_desl = 0;
  currData.x_tol_sum = 0;
  currData.y_tol_sum = 0;
  line_start_packet.x_desl = 0;
  line_start_packet.y_desl = 0;
  packet_count = 0;
}

enum event_t (packet_to_event)(struct packet * pkt, uint8_t tol, uint8_t x_len){

    if(pkt->lb & !currData.lb) {
      currData.lb = true;
      return LEFT_DOWN;
    }
    else if(!pkt->lb & currData.lb){
      currData.lb = false;
      return LEFT_UP;
    }
    else if(pkt->rb & !currData.rb){
      currData.rb = true;
      return RIGHT_DOWN;
    }
    else if(!pkt->rb & currData.rb){
      currData.rb = false;
      return RIGHT_UP;
    }
    else
    {

        currData.x_desl += pkt->delta_x;
        currData.y_desl += pkt->delta_y;
        line_start_packet.x_desl += pkt->delta_x;
        line_start_packet.y_desl += pkt->delta_y;

        switch (currState)
        {
        case LEFT_DONE:
        case DRAW_LEFT:
          packet_count++;

          if(pkt->delta_x < 0 ) currData.x_tol_sum += (-pkt->delta_x);
          if(pkt->delta_y < 0 ) currData.y_tol_sum += (-pkt->delta_y);

          if( currData.x_tol_sum > tol || currData.y_tol_sum > tol ) return TOL_FAIL;

          // SLope smaller than 1
          if(packet_count == PACKET_SLOPE_COUNT) {
            if((double)line_start_packet.y_desl / line_start_packet.x_desl <= 1.0 ) return TOL_FAIL;
            packet_count = 0;
            line_start_packet.y_desl = 0;
            line_start_packet.x_desl = 0;
          }
          break;
        
        case RIGHT_DONE:
        case DRAW_RIGHT:
          packet_count++;

          if(pkt->delta_x < 0 ) currData.x_tol_sum += (-pkt->delta_x);
          if(pkt->delta_y > 0 ) currData.y_tol_sum += pkt->delta_y;

          if( currData.x_tol_sum > tol || currData.y_tol_sum > tol) return TOL_FAIL;

          if(packet_count == PACKET_SLOPE_COUNT) {
            if((double)line_start_packet.y_desl / line_start_packet.x_desl >= -1.0 ) return TOL_FAIL;
            packet_count = 0;
            line_start_packet.y_desl = 0;
            line_start_packet.x_desl = 0;
          }
          
          break;
        
        case VERTEX:

          if(pkt->delta_x > 0 ) currData.x_tol_sum += (pkt->delta_x);
          else                 currData.x_tol_sum += (-pkt->delta_x);
          if(pkt->delta_y > 0 ) currData.y_tol_sum += (pkt->delta_y);
          else                 currData.y_tol_sum += (-pkt->delta_y);

          if( currData.x_tol_sum > tol || currData.y_tol_sum > tol) return TOL_FAIL;

        break;

        default:
          break;
        }

        if(currData.x_desl >= x_len) return LINE_C;

        return CONTINUE_DRAW;
    }
}

enum states (handle_mouse_event)(struct packet  * pkt, uint8_t tol, uint8_t x_len){

  enum event_t event = packet_to_event(pkt, tol, x_len);

  switch (currState)
  {
  case 
    INITIAL:
      if(event == LEFT_DOWN){
        currState = DRAW_LEFT;
        reset_data();
      }
  break;

  case DRAW_LEFT:
    if(event == LEFT_UP){
        currState = INITIAL;
        reset_data();
    }
    else if(event == TOL_FAIL){
        reset_data();
    }
    else if(event == LINE_C){
        if(packet_count == 0 || (double)line_start_packet.y_desl / line_start_packet.x_desl > 1.0 ) {
          currState = LEFT_DONE;
        }
        reset_data();
    }
  break;

  case LEFT_DONE:
    if(event == LEFT_UP){
      currState = VERTEX;
      reset_data();
    }
    else if(event == TOL_FAIL){
      currState = DRAW_LEFT;
      reset_data();
    }
    else if(event == RIGHT_DOWN){
      currState = INITIAL;
      reset_data();
    }
  break;

  case VERTEX:
    packet_count = 0;
    if(event == RIGHT_DOWN){
        currState = DRAW_RIGHT;
        reset_data();
    }
    else if(event == LEFT_DOWN){
        currState = DRAW_LEFT;
        reset_data();
    }
    else if(event == TOL_FAIL){
        currState = INITIAL;
        reset_data();
    }
  break;

  case DRAW_RIGHT:
    if(event == RIGHT_UP){
        currState = INITIAL;
        reset_data();
    }
    else if(event == TOL_FAIL){
        currState = INITIAL;
        reset_data();
    }
    else if(event == LINE_C){
        if(packet_count == 0 || (double)line_start_packet.y_desl / line_start_packet.x_desl < -1.0 ) {
          currState = RIGHT_DONE;
        }
        reset_data();
    }
  break;

  case RIGHT_DONE:
    if(event == RIGHT_UP){
      currState = EXIT;
      reset_data();
    }
    else if(event == TOL_FAIL){
      currState = INITIAL;
      reset_data();
    }
    else if(event == LEFT_DOWN){
      currState = INITIAL;
      reset_data();
    }
  break;
  default:
    break;
  }


  return currState;

}

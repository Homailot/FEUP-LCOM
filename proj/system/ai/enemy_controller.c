#include "enemy_controller.h"
#include "../graphics/render.h"

static enemy_ctrl_t * controllers = NULL;

static int num_controllers = 0;

extern time_t time_counter;

extern player_info_t player;

extern map_t map;

void init_enemy(player_info_t * enemy, bool random, time_t timeout_move, double fov, double range, int num_positions, vector_t * pos){
    if(num_controllers == 0) controllers = (enemy_ctrl_t *) malloc(sizeof(enemy_ctrl_t));
    else controllers = realloc((void *)controllers, (num_controllers+1) * sizeof(enemy_ctrl_t));

    enemy_ctrl_t ctrl;
    ctrl.random = random;
    ctrl.num_positions = num_positions;
    ctrl.last_op = 0;
    ctrl.range = range;
    ctrl.fov = fov;
    ctrl.curr_idx = -1;
    ctrl.timeout_movm = timeout_move;
    ctrl.player = enemy;
    ctrl.player->movm.lb_single_tap = false;



    if(num_positions != 0){
        ctrl.positions = malloc(num_positions*sizeof(vector_t));
        for (int i = 0; i < num_positions; i++)
        {
            ctrl.positions[i] = pos[i];
        }
    }
    controllers[num_controllers] = ctrl;

    num_controllers++;
}

void free_controllers(){
    for (int i = 0; i < num_controllers; i++)
    {
        if(controllers[i].num_positions != 0)
            free(controllers[i].positions);
    }
    free(controllers);
    controllers = NULL;
    num_controllers = 0;
}


int handle_enemy(enemy_ctrl_t * enm){

    if(enm->player->health <= 0){
        enm->player->movm.lb_held_down = false ;
        enm->player->movm.movement.x = 0;
        return 1;
    }

    bool follow_player = false;

    vector_t dir_to_player;
    dir_to_player.x = player.position.x - enm->player->position.x;
    dir_to_player.y = player.position.y - enm->player->position.y;

    double dists = vec_mag(&dir_to_player);
    double player_ang = 0.0;

    if(dists <= enm->range){

        player_ang = degree(&dir_to_player, &enm->player->direction);
        
        ray_hit_t hit;
        shoot_ray(&enm->player->position, &enm->player->direction, &hit, enm->range);
        if((hit.hit && hit.distance*vec_mag(&enm->player->direction) > dists) || !hit.hit) {
            follow_player = abs_double(player_ang) <= enm->fov;
        }
    }

    // First verifies if has a valid destination
    bool reached = enm->curr_idx == -1 ||  
                    (abs(enm->player->position.x - enm->curr_dest.x) < 0.5 && 
                     abs(enm->player->position.y - enm->curr_dest.y) < 0.5);


    if( !follow_player && (time_counter >= enm->last_op+enm->timeout_movm || reached)){

        enm->last_op = time_counter;

        if(enm->random){
            enm->curr_dest.x = (int)rand()%map.columns;
            enm->curr_dest.y = (int)rand()%map.lines;
            enm->curr_idx = 0;
        }
        else{
            enm->curr_idx++;
            enm->curr_idx = enm->curr_idx%enm->num_positions;
            enm->curr_dest = enm->positions[enm->curr_idx];
        }
    }
    else if(follow_player)
    {
        enm->curr_dest = player.position;
        // Restoring the state after following a playera
        enm->curr_idx = -1;
    }


    vector_t new_dir;
    new_dir.x = enm->curr_dest.x - enm->player->position.x;
    new_dir.y = enm->curr_dest.y - enm->player->position.y;

    normalize(&new_dir);

    //dprintf(2,"Dir: %f %f\n", enm->curr_dest.x, enm->curr_dest.y);

    // Angle between the enemy and its new destination
    double ang_diff = degree(&new_dir, &enm->player->direction);

    // If the angle is small and the distance is smaller than the weapon's range
    if(player_ang <= 5 &&
        dists < enm->player->guns[enm->player->gun_index].gun.range-0.5 && follow_player){
        enm->player->direction = new_dir;
        enm->player->movm.movement.x = 0;
        // 50-50 change he shoots
        enm->player->movm.lb_held_down = (rand()%2 == 0);
    }
    else{
        if(abs_double(ang_diff) <= 10) enm->player->direction = new_dir;
        else if(ang_diff > 0)
            rotate_vec(&enm->player->direction, -5);
        else 
            rotate_vec(&enm->player->direction, 5);
        normalize(&enm->player->direction);
        enm->player->movm.movement.x = (rand() % 25 > 0) ? 1.0:0.0;
        enm->player->movm.movement.y = 0;
        enm->player->movm.lb_held_down = false;
    }
    return OK;
}

void handle_enemies(){
    for (int i = 0; i < num_controllers; i++)
    {
        handle_enemy(&controllers[i]);
    }
}

# name of the program (Minix service)
PROG=proj

# source code files to be compiled
SRCS = main.c lib/kbc.c lib/timer.c file/map.c lib/video_graphics.c lib/sprites.c utils/utils.c graphics/render.c graphics/hud.c controls/controls.c player/player.c guns/gun.c inventory/menu.c ai/enemy_controller.c pickups/pickups.c  game/level_manager.c game/main_menu.c game/game_manager.c lib/queue.c lib/uart.c networking/networking.c lib/rtc_controller.c

# additional compilation flags
# "-Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter" are already set
CFLAGS = -Wall -Wextra -Werror -I . -std=c11 -Wno-unused-parameter -O3 -funroll-loops -D __LCOM_OPTIMIZED__
# list of library dependencies (for Lab 2, only LCF library)
DPADD += ${LIBLCF}
LDADD += -llcf
lib/video_graphics.o: lib/video_graphics.c lib/video_graphics.h
		cc $(CFLAGS) -o lib/video_graphics.o -c lib/video_graphics.c 
lib/kbc.o: lib/kbc.c lib/kbc.h
		cc $(CFLAGS) -o lib/kbc.o -c lib/kbc.c 
lib/timer.o: lib/timer.c lib/timer.h
		cc $(CFLAGS) -o lib/timer.o -c lib/timer.c
lib/sprites.o: lib/sprites.c lib/sprites.h
		cc $(CFLAGS) -o lib/sprites.o -c lib/sprites.c
lib/queue.o: lib/queue.c lib/queue.h
		cc $(CFLAGS) -o lib/queue.o -c lib/queue.c
lib/uart.o: lib/uart.c lib/uart.h
		cc $(CFLAGS) -o lib/uart.o -c lib/uart.c
lib/rtc_controller.o: lib/rtc_controller.c lib/rtc_controller.h
		cc $(CFLAGS) -o lib/rtc_controller.o -c lib/rtc_controller.c
networking/networking.o: networking/networking.c networking/networking.h
		cc $(CFLAGS) -o networking/networking.o -c networking/networking.c
file/map.o: file/map.c file/map.h #$(DPADD)
		cc $(CFLAGS) -o file/map.o -c file/map.c
graphics/render.o: graphics/render.c graphics/render.h
		cc $(CFLAGS) -o graphics/render.o -c graphics/render.c
graphics/hud.o: graphics/hud.c graphics/hud.h
		cc $(CFLAGS) -o graphics/hud.o -c graphics/hud.c
utils/utils.o: utils/utils.c utils/utils.h
		cc $(CFLAGS) -o utils/utils.o -c utils/utils.c 
controls/controls.o: controls/controls.c controls/controls.h
		cc $(CFLAGS) -o controls/controls.o -c controls/controls.c 
player/player.o: player/player.c player/player.h
		cc $(CFLAGS) -o player/player.o -c player/player.c 
guns/gun.o: guns/gun.c guns/gun.h
		cc $(CFLAGS) -o guns/gun.o -c guns/gun.c 
inventory/menu.o: inventory/menu.c inventory/menu.h
		cc $(CFLAGS) -o inventory/menu.o -c inventory/menu.c 
pickups/pickups.o: pickups/pickups.c pickups/pickups.h
		cc $(CFLAGS) -o pickups/pickups.o -c pickups/pickups.c
ai/enemy_controller.o: ai/enemy_controller.c ai/enemy_controller.h
		cc $(CFLAGS) -o ai/enemy_controller.o -c ai/enemy_controller.c
game/level_manager.o: game/level_manager.c game/level_manager.h
		cc $(CFLAGS) -o game/level_manager.o -c game/level_manager.c 
game/main_menu.o: game/main_menu.c game/main_menu.h
		cc $(CFLAGS) -o game/main_menu.o -c game/main_menu.c
game/game_manager.o: game/game_manager.c game/game_manager.h
		cc $(CFLAGS) -o game/game_manager.o -c game/game_manager.c 
# include LCOM's makefile that does all the "heavy lifting" 
# include LCOM's makefile that does all the "heavy lifting"
.include <minix.lcom.mk>

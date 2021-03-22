#include "map.h"
#include "file.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int(load_map)(uint16_t mapNumber, map_t* map) {
    char filePath[55], auxString[55];
    int32_t num = 0;
    uint16_t curLine = 0, curCol = 0;

    strcpy(filePath, "/home/lcom/labs/minixop/data/maps/map");
    sprintf(auxString, "%03d", mapNumber);

    strcat(filePath, auxString);

    strcpy(auxString, ".mxmap\0");

    strcat(filePath, auxString);

    FILE* file = fopen(filePath, "r");
    if (!file) {
        printf("error: %s\n", strerror(errno));
        return BAD_FILE;
    }

    fscanf(file, "%hd", &map->lines);
    fscanf(file, "%hd", &map->columns);

    map->matrix = (int32_t**)malloc(map->lines * sizeof(int32_t*));
    map->matrix[0] = (int32_t*)malloc(map->columns * sizeof(int32_t));

    while (curLine < map->lines) {
        fscanf(file, "%d", &num);
        map->matrix[curLine][curCol] = num;

        curCol++;
        if (curCol >= map->columns) {
            curLine++;
            curCol = 0;
            map->matrix[curLine] = (int32_t*)malloc(map->columns * sizeof(int32_t));
        }
    }

    fclose(file);
    return OK;
}

void(close_map)(map_t* map) {
    for (uint16_t li = 0; li < map->lines; li++) {
        free(map->matrix[li]);
    }

    free(map->matrix);
}

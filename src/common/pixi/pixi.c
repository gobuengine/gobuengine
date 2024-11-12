#include "pixi.h"

/**
 * Dibuja una cuadrícula en la pantalla con coordenadas.
 *
 * Esta función dibuja una cuadrícula en la pantalla utilizando la biblioteca Raylib.
 * La cuadrícula consiste en líneas verticales y horizontales, y muestra las coordenadas
 * de cada celda en la esquina superior izquierda de la misma.
 *
 * @param width Ancho total de la cuadrícula en píxeles.
 * @param height Alto total de la cuadrícula en píxeles.
 * @param size Tamaño de cada celda de la cuadrícula en píxeles.
 */
void pixi_draw_grid(int width, int height, int size)
{
    const Color GRID_COLOR1 = (Color){66, 66, 66, 255};
    const Color GRID_COLOR2 = (Color){57, 57, 57, 255};
    const Color GRID_COLOR3 = (Color){50, 50, 50, 255};

    for (int y = 0; y < height / size + 1; y++)
    {
        for (int x = 0; x < width / size + 1; x++)
        {
            DrawRectangle(x * size, y * size, size, size, (x + y) % 2 == 0 ? GRID_COLOR2 : GRID_COLOR1);
        }
    }

    // Dibujar líneas verticales
    for (int x = 0; x <= width; x += size)
    {
        DrawLineV((Vector2){(float)x, 0}, (Vector2){(float)x, (float)height}, GRID_COLOR3);
    }

    // Dibujar líneas horizontales
    for (int y = 0; y <= height; y += size)
    {
        DrawLineV((Vector2){0, (float)y}, (Vector2){(float)width, (float)y}, GRID_COLOR3);
    }

}

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
    const Color GRID_COLOR = LIGHTGRAY;
    const int TEXT_SIZE = 10;
    const int TEXT_PADDING = 2;

    // Dibujar líneas verticales
    for (int x = 0; x <= width; x += size)
    {
        DrawLineV((Vector2){(float)x, 0}, (Vector2){(float)x, (float)height}, GRID_COLOR);
    }

    // Dibujar líneas horizontales
    for (int y = 0; y <= height; y += size)
    {
        DrawLineV((Vector2){0, (float)y}, (Vector2){(float)width, (float)y}, GRID_COLOR);
    }

    // Dibujar coordenadas en cada celda
    for (int x = 0; x < width; x += size)
    {
        for (int y = 0; y < height; y += size)
        {
            const char *text = TextFormat("[%i,%i]", x / size, y / size);
            DrawText(text, x + TEXT_PADDING, y + TEXT_PADDING, TEXT_SIZE, GRID_COLOR);
        }
    }
}

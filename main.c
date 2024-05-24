#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define FONT_SIZE 30 // Tamaño de la fuente
#define MAX_PRODUCTS 100 //Maximo de productos posibles

// Estructura para un botón
typedef struct
{
    SDL_Rect rect; //Rectangulo en SDL
    char text[50]; //Variable para guardar el texto del boton
        bool clicked; //Variable de estado para el boton
    } Button;

// Estructura para un producto
typedef struct
{
    int numProd; //Variable para almacenar el numero de producto
    char name[50]; //Nombre del producto
    int quantity; //Cantidad del producto
} Product;

//Prototipado de Funciones
void renderText(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, const char *text, int x, int y);
void getTextInput(SDL_Renderer *renderer, TTF_Font *font, char *inputText, int maxLength, int x, int y, int opc);
int getIntInput(SDL_Renderer *renderer, TTF_Font *font, int x, int y, int opc);
void renderButton(SDL_Renderer *renderer, TTF_Font *font, Button *button);
void renderMenu(SDL_Renderer *renderer, TTF_Font *font, Button *buttons, int buttonCount);
void renderTableHeader(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, int x, int y);
void renderTableRow(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, const char *productName, int quantity, int x, int y);
int handleMainMenuEvents(SDL_Event *event, Button *buttons, int buttonCount, SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int *inventoryCount);
void handleInventoryUpdate(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int *inventoryCount);
void renderInventory(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int inventoryCount);
void handleQuantityModification(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int inventoryCount);
void editInventory(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int *inventoryCount);
void viewInventory(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int inventoryCount);
void clearEventQueue();

int main(int argc, char *argv[])
{
    SDL_Window *window; // Ventana SDL
    SDL_Renderer *renderer; // Lienzo SDL
    TTF_Font *font; // Fuente SDL
    SDL_Event event; // Evento de SDL
    Button mainMenuButtons[4]; // Botones del menú principal
    Product inventory[MAX_PRODUCTS]; // Inventario de productos
    int inventoryCount = 0; // Contador de productos en el inventario

    // Inicialización de SDL
    SDL_Init(SDL_INIT_VIDEO); // Inicializa el subsistema de video de SDL
    TTF_Init(); // Inicializa el subsistema de fuentes de SDL_ttf

    // Código para obtener los valores del tamaño de la pantalla
    SDL_DisplayMode displayMode; // Estructura para guardar la información del modo de pantalla
    SDL_GetDesktopDisplayMode(0, &displayMode); // Obtiene el modo de pantalla del escritorio
    int screenWidth = displayMode.w; // Valor del ancho de la pantalla
    int screenHeight = displayMode.h; // Valor del alto de la pantalla

    // Crear ventana maximizada
    window = SDL_CreateWindow("Gestion de Inventarios", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_MAXIMIZED); // Crea una ventana SDL
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // Crea un renderer SDL para la ventana
    font = TTF_OpenFont("arial.ttf", FONT_SIZE); // Abre una fuente TTF

    // Crear botones del menú principal
    mainMenuButtons[0].rect = (SDL_Rect){screenWidth / 4, screenHeight / 4, screenWidth / 2, screenHeight / 10}; // Configura el primer botón
    strcpy(mainMenuButtons[0].text, "Actualizar Inventario"); // Asigna el texto del primer botón
    mainMenuButtons[0].clicked = false; // Establece que el primer botón no está clicado

    mainMenuButtons[1].rect = (SDL_Rect){screenWidth / 4, screenHeight / 4 + screenHeight / 10 * 1, screenWidth / 2, screenHeight / 10}; // Configura el segundo botón
    strcpy(mainMenuButtons[1].text, "Editar Inventario"); // Asigna el texto del segundo botón
    mainMenuButtons[1].clicked = false; // Establece que el segundo botón no está clicado

    mainMenuButtons[2].rect = (SDL_Rect){screenWidth / 4, screenHeight / 4 + screenHeight / 10 * 2, screenWidth / 2, screenHeight / 10}; // Configura el tercer botón
    strcpy(mainMenuButtons[2].text, "Consultar Inventario"); // Asigna el texto del tercer botón
    mainMenuButtons[2].clicked = false; // Establece que el tercer botón no está clicado

    mainMenuButtons[3].rect = (SDL_Rect){screenWidth / 4, screenHeight / 4 + screenHeight / 10 * 3, screenWidth / 2, screenHeight / 10}; // Configura el cuarto botón
    strcpy(mainMenuButtons[3].text, "Salir"); // Asigna el texto del cuarto botón
    mainMenuButtons[3].clicked = false; // Establece que el cuarto botón no está clicado

    int buttonCount = 4; // Número de botones

    bool quit = false; // Estado para cerrar el programa
    while (!quit)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establece el color de fondo del render a blanco
        SDL_RenderClear(renderer); // Limpia el renderer con el color de fondo

        // Renderizar menú principal
        renderMenu(renderer, font, mainMenuButtons, buttonCount); // Llama a la función para renderizar el menú

        SDL_RenderPresent(renderer); // Muestra lo que hay en el render

        // Manejo de eventos
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT: // Si se cierra la ventana
                quit = true; // Establece que se debe salir del bucle principal
                break;
            case SDL_MOUSEBUTTONDOWN: // Si se da click
                // Comprobar clic en botones del menú principal
                for (int i = 0; i < buttonCount; i++)
                {
                    if (event.button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &mainMenuButtons[i].rect)) // Si se da clic izquierdo en algún botón
                    {
                        mainMenuButtons[i].clicked = true; // Establece que el botón fue clicado
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP: 
                // Manejo de eventos del menú principal
                if (handleMainMenuEvents(&event, mainMenuButtons, buttonCount, renderer, font, inventory, &inventoryCount)) // Llama a la función para manejar eventos del menú
                    quit = true; // Salir del bucle principal si la función lo indica
                break;
            }
        }
    }

    TTF_CloseFont(font); // Cierra la fuente TTF
    SDL_DestroyRenderer(renderer); // Destruye el renderer SDL
    SDL_DestroyWindow(window); // Destruye la ventana SDL
    TTF_Quit(); // Cierra el subsistema de fuentes de SDL_ttf
    SDL_Quit(); // Cierra todos los subsistemas de SDL

    return 0; // Retorna 0, indicando que el programa terminó correctamente
}

void renderText(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, const char *text, int x, int y)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color); // Crea una superficie con el texto renderizado en color sólido
    if (surface == NULL) // Verifica si la creación de la superficie falló
    {
        return; // Sale de la función si la superficie no se pudo crear
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Crea una textura a partir de la superficie
    if (texture == NULL) // Verifica si la creación de la textura falló
    {
        SDL_FreeSurface(surface); // Libera la superficie si no se pudo crear la textura
        return; // Sale de la función si la textura no se pudo crear
    }

    // Creación del rectángulo donde irá el texto
    SDL_Rect rect; 
    rect.x = x; // Posición x del rectángulo
    rect.y = y; // Posición y del rectángulo
    rect.w = surface->w; // Ancho del rectángulo, igual al ancho de la superficie
    rect.h = surface->h; // Alto del rectángulo, igual al alto de la superficie

    // Libera la superficie ya que la textura ha sido creada
    SDL_FreeSurface(surface);

    // Copia la textura al renderer y luego destruye la textura ya que no se necesita más
    SDL_RenderCopy(renderer, texture, NULL, &rect); // Copia la textura al renderer en la posición y tamaño del rectángulo
    SDL_DestroyTexture(texture); // Destruye la textura para liberar recursos
}

void getTextInput(SDL_Renderer *renderer, TTF_Font *font, char *inputText, int maxLength, int x, int y, int opc)
{
    SDL_StartTextInput(); // Inicialización del modo de entrada de texto
    SDL_Event event; // Estructura para eventos de SDL
    SDL_bool done = SDL_FALSE; // Variable de estado para el bucle de entrada de texto
    int textLen = 0; // Longitud actual del texto de entrada
    const char *prompt; // Mensaje que se mostrará al usuario

    // Establecer el mensaje apropiado según el valor de opc
    if (opc == 0)
        prompt = "Ingrese la cantidad del producto:";
    else if (opc == 1)
        prompt = "Ingrese el indice del producto a eliminar:";
    else
        prompt = "Ingrese el indice del producto a modificar:";

    while (!done) // Bucle principal de entrada de texto
    {
        while (SDL_PollEvent(&event)) // Procesar eventos
        {
            if (event.type == SDL_QUIT) // Si se cierra la ventana
            {
                done = SDL_TRUE; // Salir del bucle
            }
            else if (event.type == SDL_KEYDOWN) // Si se presiona una tecla
            {
                if (event.key.keysym.sym == SDLK_RETURN) // Si se presiona Enter
                {
                    done = SDL_TRUE; // Salir del bucle
                }
                else if (event.key.keysym.sym == SDLK_BACKSPACE && textLen > 0) // Si se presiona Backspace y hay texto
                {
                    inputText[--textLen] = '\0'; // Eliminar el último carácter del texto
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE) // Si se presiona Escape
                {
                    done = SDL_TRUE; // Salir del bucle
                }
            }
            else if (event.type == SDL_TEXTINPUT) // Si se introduce texto
            {
                if (textLen < maxLength - 1) // Si el texto no ha alcanzado la longitud máxima
                {
                    strcat(inputText, event.text.text); // Añadir el texto al final de inputText
                    textLen += strlen(event.text.text); // Aumentar la longitud del texto
                }
            }
        }

        // Renderizar la interfaz
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
        SDL_RenderClear(renderer); // Limpiar el renderer

        SDL_Color textColor = {0, 0, 0, 255}; // Color del texto: negro
        renderText(renderer, font, textColor, prompt, x, y); // Renderizar el mensaje de prompt
        renderText(renderer, font, textColor, inputText, x, y + 50); // Renderizar el texto introducido

        SDL_RenderPresent(renderer); // Actualizar la pantalla
    }

    SDL_StopTextInput(); // Finalizar el modo de entrada de texto
}

int getIntInput(SDL_Renderer *renderer, TTF_Font *font, int x, int y, int opc)
{
    char inputText[10] = ""; // Inicializar variable local para almacenar la entrada del usuario
    SDL_bool done = SDL_FALSE; // Variable de estado para el bucle de entrada
    int intValue = 0; // Variable para almacenar el valor entero convertido

    while (!done) // Bucle principal de entrada de texto
    {
        // Renderizar el mensaje antes de pedir la entrada
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
        SDL_RenderClear(renderer); // Limpiar el renderer
        SDL_Color textColor = {0, 0, 0, 255}; // Color del texto: negro

        // Obtener la entrada del usuario
        getTextInput(renderer, font, inputText, 10, x, y + 50, opc); // Llama a la función para obtener la entrada de texto, ajustando la posición en Y para que aparezca debajo del mensaje

        // Convertir la entrada a entero
        intValue = atoi(inputText); // Convertir el texto de entrada a un valor entero

        // Verificar si la entrada es un número válido
        done = SDL_TRUE; // Asumir que la entrada es válida
        for (int i = 0; i < strlen(inputText); i++) // Verificar cada carácter en la entrada de texto
        {
            if (!isdigit(inputText[i])) // Si algún carácter no es un dígito
            {
                done = SDL_FALSE; // Marcar la entrada como inválida
                break; // Salir del bucle
            }
        }

        if (!done) // Si la entrada no es válida
        {
            SDL_DisplayMode displayMode; // Estructura para obtener el modo de pantalla
            SDL_GetDesktopDisplayMode(0, &displayMode); // Obtener el modo de pantalla del escritorio
            int screenWidth = displayMode.w; // Ancho de la pantalla
            int screenHeight = displayMode.h; // Alto de la pantalla

            // Mostrar el mensaje de error
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
            SDL_RenderClear(renderer); // Limpiar el renderer
            renderText(renderer, font, textColor, "Entrada no valida. Por favor ingrese un numero", (screenWidth - 500) / 2, (screenHeight / 2) - 30); // Renderizar el mensaje de error
            SDL_RenderPresent(renderer); // Actualizar la pantalla
            memset(inputText, 0, 10); // Limpiar la entrada de texto
            SDL_Delay(2000); // Esperar 2 segundos antes de pedir la entrada de nuevo
        }
    }

    return intValue; // Retornar el valor entero válido
}

void renderButton(SDL_Renderer *renderer, TTF_Font *font, Button *button)
{
    SDL_Color textColor = {0, 0, 0, 255}; // Color del texto: negro
    SDL_Color buttonColor = {200, 200, 200, 255}; // Color del botón: gris claro

    // Renderizar botón
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a); // Establecer color de relleno del botón
    SDL_RenderFillRect(renderer, &button->rect); // Dibujar el relleno del botón
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Establecer color para el borde del botón: negro
    SDL_RenderDrawRect(renderer, &button->rect); // Dibujar el borde del botón

    // Renderizar texto centrado en el botón
    int textWidth, textHeight; // Variables para almacenar el tamaño del texto
    TTF_SizeText(font, button->text, &textWidth, &textHeight); // Obtener el tamaño del texto
    int textX = button->rect.x + (button->rect.w - textWidth) / 2; // Calcular la posición X para centrar el texto
    int textY = button->rect.y + (button->rect.h - textHeight) / 2; // Calcular la posición Y para centrar el texto
    renderText(renderer, font, textColor, button->text, textX, textY); // Renderizar el texto en la posición calculada
}

void renderMenu(SDL_Renderer *renderer, TTF_Font *font, Button *buttons, int buttonCount)
{
    SDL_Color textColor = {0, 0, 0, 255}; // Color del texto: negro
    SDL_DisplayMode displayMode; // Estructura para obtener el modo de pantalla
    SDL_GetDesktopDisplayMode(0, &displayMode); // Obtener el modo de pantalla del escritorio
    int screenWidth = displayMode.w; // Ancho de la pantalla

    // Renderizar el título del menú
    renderText(renderer, font, textColor, "Gestion de Inventario", (screenWidth - 290) / 2, 100); // Renderizar el texto centrado horizontalmente en la pantalla

    // Renderizar botones del menú
    for (int i = 0; i < buttonCount; i++) // Bucle para renderizar cada botón
    {
        renderButton(renderer, font, &buttons[i]); // Renderizar el botón
    }
}

void renderTableHeader(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, int x, int y)
{
    // Renderizar la cabecera de la tabla
    renderText(renderer, font, color, "Producto", x, y); // Renderizar el encabezado "Producto" en la posición (x, y)
    renderText(renderer, font, color, "Cantidad", x + 200, y); // Renderizar el encabezado "Cantidad" en la posición (x + 200, y)
}

void renderTableRow(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, const char *productName, int quantity, int x, int y)
{
    // Renderizar una fila de la tabla
    renderText(renderer, font, color, productName, x, y); // Renderizar el nombre del producto en la posición (x, y)
    char quantityStr[10]; // Variable para almacenar la cantidad como cadena
    snprintf(quantityStr, sizeof(quantityStr), "%d", quantity); // Convertir la cantidad a cadena
    renderText(renderer, font, color, quantityStr, x + 200, y); // Renderizar la cantidad en la posición (x + 200, y)
}

int handleMainMenuEvents(SDL_Event *event, Button *buttons, int buttonCount, SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int *inventoryCount)
{
    // Manejar clics en botones del menú
    if (event->type == SDL_MOUSEBUTTONUP) // Verificar si se soltó el botón del ratón
    {
        for (int i = 0; i < buttonCount; i++) // Iterar sobre todos los botones
        {
            // Verificar si se hizo clic con el botón izquierdo del ratón dentro de un botón
            if (event->button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&(SDL_Point){event->button.x, event->button.y}, &buttons[i].rect))
            {
                // Comparar el texto del botón para determinar la acción a realizar
                if (strcmp(buttons[i].text, "Actualizar Inventario") == 0) // Si el texto del botón es "Actualizar Inventario"
                {
                    handleInventoryUpdate(renderer, font, inventory, inventoryCount); // Llamar a la función para actualizar el inventario
                    return 0; // Retornar 0 para indicar que no se debe salir del programa
                }
                else if (strcmp(buttons[i].text, "Editar Inventario") == 0) // Si el texto del botón es "Editar Inventario"
                {
                    editInventory(renderer, font, inventory, inventoryCount); // Llamar a la función para editar el inventario
                    return 0; // Retornar 0 para indicar que no se debe salir del programa
                }
                else if (strcmp(buttons[i].text, "Consultar Inventario") == 0) // Si el texto del botón es "Consultar Inventario"
                {
                    viewInventory(renderer, font, inventory, *inventoryCount); // Llamar a la función para consultar el inventario
                    return 0; // Retornar 0 para indicar que no se debe salir del programa
                }
                else if (strcmp(buttons[i].text, "Salir") == 0) // Si el texto del botón es "Salir"
                {
                    return 1; // Retornar 1 para indicar que se debe salir del programa
                }
            }
        }
    }
    return 0; // Retornar 0 si no se realizó ninguna acción que requiera salir del programa
}

void handleInventoryUpdate(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int *inventoryCount)
{
    SDL_DisplayMode displayMode; // Estructura para obtener el modo de pantalla
    SDL_GetDesktopDisplayMode(0, &displayMode); // Obtener el modo de pantalla del escritorio
    int screenWidth = displayMode.w; // Ancho de la pantalla
    int screenHeight = displayMode.h; // Alto de la pantalla

    // Renderizar mensaje de actualización de inventario
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
    SDL_RenderClear(renderer); // Limpiar el renderer
    SDL_Color textColor = {0, 0, 0, 255}; // Color del texto: negro
    renderText(renderer, font, textColor, "Actualizando Inventario...", (screenWidth - 250) / 2, (screenHeight / 2) - 30); // Renderizar el mensaje de actualización centrado en la pantalla
    SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el mensaje

    // Simulación de una actualización de inventario
    SDL_Delay(2000); // Esperar 2 segundos para simular el tiempo de actualización

    // Después de la actualización, limpiar la pantalla nuevamente
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
    SDL_RenderClear(renderer); // Limpiar el renderer
}

void renderInventory(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int inventoryCount)
{
    // Renderizar inventario en la pantalla
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
    SDL_RenderClear(renderer); // Limpiar el renderer

    // Mostrar cada producto en el inventario
    SDL_Color textColor = {0, 0, 0, 255}; // Color del texto: negro
    int y = 100; // Posición inicial en Y para renderizar cada producto en el inventario
    for (int i = 0; i < inventoryCount; i++) // Iterar sobre cada producto en el inventario
    {
        char itemText[100]; // Variable para almacenar el texto de cada producto
        snprintf(itemText, sizeof(itemText), "%s - Cantidad: %d", inventory[i].name, inventory[i].quantity); // Crear el texto con el nombre y la cantidad del producto
        renderText(renderer, font, textColor, itemText, 200, y); // Renderizar el texto en la posición (200, y)
        y += 50; // Aumentar la posición en Y para el siguiente producto
    }

    SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el inventario
}

void editInventory(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int *inventoryCount)
{
    bool editDone = false; // Variable de estado para controlar la edición del inventario
    SDL_Event event; // Evento SDL para manejar la interacción del usuario
    SDL_Color textColor = {0, 0, 0, 255}; // Color del texto: negro

    // Limpiar la pantalla
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
    SDL_RenderClear(renderer); // Limpiar el renderer

    SDL_DisplayMode displayMode; // Estructura para obtener el modo de pantalla
    SDL_GetDesktopDisplayMode(0, &displayMode); // Obtener el modo de pantalla del escritorio
    int screenWidth = displayMode.w; // Ancho de la pantalla
    int screenHeight = displayMode.h; // Alto de la pantalla

    // Crear botones para agregar, eliminar, modificar la cantidad y volver al menú principal
    Button addButton = {{(screenWidth - 400) / 2, 150, 400, 50}, "Agregar Nuevo Producto", false}; // Botón para agregar un nuevo producto
    Button deleteButton = {{(screenWidth - 400) / 2, 250, 400, 50}, "Eliminar Producto", false}; // Botón para eliminar un producto
    Button modifyQuantityButton = {{(screenWidth - 400) / 2, 350, 400, 50}, "Modificar Cantidad", false}; // Botón para modificar la cantidad de un producto (nuevo botón)
    Button backButton = {{(screenWidth - 400) / 2, 450, 400, 50}, "Volver al Menu Principal", false}; // Botón para volver al menú principal

    while (!editDone)
    {
        // Limpiar la pantalla
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
        SDL_RenderClear(renderer); // Limpiar el renderer

        // Renderizar título y botones del menú de edición del inventario
        renderText(renderer, font, textColor, "Editar Inventario", (screenWidth - 210) / 2, 80); // Renderizar el título del menú
        renderButton(renderer, font, &addButton); // Renderizar el botón para agregar un nuevo producto
        renderButton(renderer, font, &deleteButton); // Renderizar el botón para eliminar un producto
        renderButton(renderer, font, &modifyQuantityButton); // Renderizar el botón para modificar la cantidad de un producto
        renderButton(renderer, font, &backButton); // Renderizar el botón para volver al menú principal

        SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el menú de edición del inventario

        // Manejo de eventos
        while (SDL_PollEvent(&event)) // Recorrer todos los eventos en la cola de eventos
        {
            switch (event.type) // Verificar el tipo de evento
            {
            case SDL_QUIT: // Si se cierra la ventana
                editDone = true; // Cambiar el estado de edición a "hecho"
                break;
            case SDL_MOUSEBUTTONDOWN: // Si se presiona un botón del ratón
                // Comprobar el clic en los botones
                if (event.button.button == SDL_BUTTON_LEFT) // Si se hace clic con el botón izquierdo del ratón
                {
                    if (SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &addButton.rect)) // Si se hace clic en el botón para agregar un nuevo producto
                    {
                        if (*inventoryCount < MAX_PRODUCTS) // Verificar si el inventario aún no está lleno
                        {
                            clearEventQueue(); // Limpiar la cola de eventos para evitar clics adicionales
                            int itemQuantity = getIntInput(renderer, font, (screenWidth - 450) / 2, 200, 0); // Obtener la cantidad del nuevo producto

                            // Limpiar la pantalla antes de mostrar el mensaje de éxito o error
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
                            SDL_RenderClear(renderer); // Limpiar el renderer

                            // Validar la entrada del usuario
                            if (itemQuantity > 0) // Si la cantidad es válida
                            {
                                char itemName[50]; // Variable para almacenar el nombre del nuevo producto
                                sprintf(itemName, "Producto %i", *inventoryCount + 1); // Generar un nombre para el nuevo producto
                                inventory[*inventoryCount].numProd = *inventoryCount + 1; // Asignar un número de producto único
                                strcpy(inventory[*inventoryCount].name, itemName); // Asignar el nombre al nuevo producto
                                                                inventory[*inventoryCount].quantity = itemQuantity; // Asignar la cantidad al nuevo producto
                                (*inventoryCount)++; // Incrementar el contador de inventario
                                printf("Nuevo producto agregado al archivo de inventario.\n"); // Mensaje de éxito en la consola
                                renderText(renderer, font, textColor, "Producto agregado exitosamente.", (screenWidth - 350) / 2, (screenHeight / 2) - 60); // Mostrar mensaje de éxito en la pantalla
                            }
                            else // Si la cantidad no es válida
                            {
                                renderText(renderer, font, textColor, "Caracter no valido. Volviendo al menu...", (screenWidth - 450) / 2, (screenHeight / 2) - 60); // Mostrar mensaje de error en la pantalla
                            }
                            SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el mensaje
                            SDL_Delay(2000); // Esperar 2 segundos
                        }
                        else // Si el inventario está lleno
                        {
                            renderText(renderer, font, textColor, "No se puede agregar mas productos. Limite alcanzado.", (screenWidth - 600) / 2, (screenHeight / 2) - 60); // Mostrar mensaje de error en la pantalla
                            SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el mensaje
                            SDL_Delay(2000); // Esperar 2 segundos
                        }
                    }
                    else if (SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &deleteButton.rect)) // Si se hace clic en el botón para eliminar un producto
                    {
                        // Solicitar al usuario que ingrese el número del producto a eliminar
                        clearEventQueue(); // Limpiar la cola de eventos para evitar clics adicionales
                        int productIndex = getIntInput(renderer, font, (screenWidth - 450) / 2, 300, 1); // Obtener el índice del producto a eliminar

                        // Verificar si el índice es válido
                        if (productIndex >= 1 && productIndex <= *inventoryCount) // Si el índice está dentro del rango válido
                        {
                            for (int j = productIndex - 1; j < *inventoryCount - 1; j++) // Eliminar el producto del inventario
                            {
                                inventory[j] = inventory[j + 1];
                            }
                            (*inventoryCount)--; // Decrementar el contador de inventario
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
                            SDL_RenderClear(renderer); // Limpiar el renderer
                            renderText(renderer, font, textColor, "Producto eliminado con exito.", (screenWidth - 350) / 2, (screenHeight / 2) - 60); // Mostrar mensaje de éxito en la pantalla
                            SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el mensaje
                            SDL_Delay(2000); // Esperar 2 segundos
                        }
                        else // Si el índice no es válido
                        {
                            // Renderizar un mensaje de error en la pantalla
                            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
                            SDL_RenderClear(renderer); // Limpiar el renderer
                            renderText(renderer, font, textColor, "Indice de producto no valido. Intente de nuevo.", (screenWidth - 550) / 2, (screenHeight / 2) - 60); // Mostrar mensaje de error en la pantalla
                            SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el mensaje
                            SDL_Delay(2000); // Esperar 2 segundos
                        }
                    }
                    else if (SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &modifyQuantityButton.rect)) // Si se hace clic en el botón para modificar la cantidad de un producto
                    {
                        // Lógica para modificar la cantidad de un producto
                        handleQuantityModification(renderer, font, inventory, *inventoryCount);
                    }
                    else if (SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &backButton.rect)) // Si se hace clic en el botón para volver al menú principal
                    {
                        // Volver al menú principal
                        event.button.x = 0; // Reiniciar la posición X del evento del botón
                        event.button.y = 0; // Reiniciar la posición Y del evento del botón
                        SDL_ShowCursor(SDL_DISABLE); // Ocultar el cursor
                        SDL_ShowCursor(SDL_ENABLE); // Mostrar el cursor
                        SDL_Delay(200); // Pausa breve para evitar que se registre el clic en el menú principal
                        clearEventQueue(); // Limpiar la cola de eventos para evitar clics adicionales
                        editDone = true; // Cambiar el estado de edición a "hecho"
                    }
                }
                break;
            }
        }
    }
}

void handleQuantityModification(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int inventoryCount)
{
    clearEventQueue(); // Limpiar la cola de eventos para evitar clics adicionales
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
    SDL_RenderClear(renderer); // Limpiar el renderer

    SDL_Color textColor = {0, 0, 0, 255}; // Color de texto negro

    SDL_DisplayMode displayMode; // Modo de visualización de la pantalla
    SDL_GetDesktopDisplayMode(0, &displayMode); // Obtener el modo de visualización de la pantalla
    int screenWidth = displayMode.w; // Ancho de la pantalla
    int screenHeight = displayMode.h; // Alto de la pantalla

    // Obtener el índice del producto del usuario
    int productIndex = getIntInput(renderer, font, (screenWidth - 450) / 2, 300, 2); // Obtener el índice del producto a modificar

    // Verificar si el índice es válido
    if (productIndex >= 1 && productIndex <= inventoryCount) // Si el índice está dentro del rango válido
    {
        int flag = 0; // Bandera para verificar si se encuentra el producto con el índice dado
        for (int i = 0; i < inventoryCount; i++) // Iterar sobre el inventario para buscar el producto
        {
            if (productIndex == inventory[i].numProd) // Si se encuentra el producto con el índice dado
            {
                flag = 1; // Establecer la bandera en 1
            }
        }

        if (flag == 1) // Si se encuentra el producto con el índice dado
        {
            // Obtener la nueva cantidad del usuario
            int newQuantity = getIntInput(renderer, font, (screenWidth - 375) / 2, 200, 0); // Obtener la nueva cantidad del producto

            // Actualizar la cantidad del producto en el inventario
            inventory[productIndex - 1].quantity = newQuantity; // Actualizar la cantidad del producto en el inventario
        }
        else // Si no se encuentra el producto con el índice dado
        {
            // Renderizar un mensaje de error en la pantalla
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
            SDL_RenderClear(renderer); // Limpiar el renderer
            renderText(renderer, font, textColor, "Indice de producto no encontrado. Volviendo al menu...", (screenWidth - 550) / 2, (screenHeight / 2) - 60); // Mostrar mensaje de error en la pantalla
            SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el mensaje
            SDL_Delay(2000); // Esperar 2 segundos
        }
    }
    else // Si el índice no es válido
    {
        // Renderizar un mensaje de error en la pantalla
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
        SDL_RenderClear(renderer); // Limpiar el renderer
        renderText(renderer, font, textColor, "Indice de producto no valido. Volviendo al menu...", (screenWidth - 550) / 2, (screenHeight / 2) - 60); // Mostrar mensaje de error en la pantalla
        SDL_RenderPresent(renderer); // Actualizar la pantalla para mostrar el mensaje
        SDL_Delay(2000); // Esperar 2 segundos
    }
}

void viewInventory(SDL_Renderer *renderer, TTF_Font *font, Product *inventory, int inventoryCount)
{
    bool viewing = true; // Variable de control para visualizar el inventario
    while (viewing)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Establecer color de fondo blanco
        SDL_RenderClear(renderer); // Limpiar el renderer

        SDL_Color textColor = {0, 0, 0, 255}; // Color de texto negro

        SDL_DisplayMode displayMode; // Modo de visualización de la pantalla
        SDL_GetDesktopDisplayMode(0, &displayMode); // Obtener el modo de visualización de la pantalla
        int screenWidth = displayMode.w; // Ancho de la pantalla

        // Texto del título
        int titleWidth, titleHeight; // Ancho y alto del título
        TTF_SizeText(font, "Inventario", &titleWidth, &titleHeight); // Obtener el tamaño del texto "Inventario"
        int titleX = (screenWidth - titleWidth) / 2; // Posición X centrada para el título
        renderText(renderer, font, textColor, "Inventario", titleX, 50); // Renderizar el título "Inventario"

        // Renderizar cabecera de la tabla
        int tableX = (screenWidth - 400) / 2; // Posición X de la tabla centrada
        int startY = 100; // Posición Y inicial de la tabla
        int rowHeight = 50; // Altura de cada fila
        renderTableHeader(renderer, font, textColor, tableX, startY); // Renderizar la cabecera de la tabla

        // Renderizar filas de productos
        int y = startY + rowHeight; // Posición Y inicial de la primera fila de productos
        for (int i = 0; i < inventoryCount; i++) // Iterar sobre los productos del inventario
        {
            renderTableRow(renderer, font, textColor, inventory[i].name, inventory[i].quantity, tableX, y); // Renderizar la fila del producto
            y += rowHeight; // Actualizar la posición Y para la siguiente fila
        }

        // Mostrar opción para volver al Menú Principal
        int promptWidth, promptHeight; // Ancho y alto del mensaje de retorno
        TTF_SizeText(font, "Presiona 'ESC' para volver al Menu Principal", &promptWidth, &promptHeight); // Obtener el tamaño del mensaje
        int promptX = (screenWidth - promptWidth) / 2; // Posición X centrada para el mensaje
        renderText(renderer, font, textColor, "Presiona 'ESC' para volver al Menu Principal", promptX, y + 50); // Renderizar el mensaje

        SDL_RenderPresent(renderer); // Actualizar el renderer para mostrar todos los elementos en pantalla

        // Manejo de eventos
        SDL_Event event; // Variable para almacenar eventos
        while (SDL_PollEvent(&event)) // Iterar sobre todos los eventos en la cola
        {
            switch (event.type) // Determinar el tipo de evento
            {
            case SDL_QUIT: // Si se cierra la ventana
                viewing = false; // Salir del bucle de visualización
                break;
            case SDL_KEYDOWN: // Si se presiona una tecla
                if (event.key.keysym.sym == SDLK_ESCAPE) // Si la tecla presionada es ESC
                    viewing = false; // Salir del bucle de visualización
                break;
            }
        }
    }
}

void clearEventQueue()
{
    SDL_Event event; // Variable para almacenar eventos
    while (SDL_PollEvent(&event)) // Iterar sobre todos los eventos en la cola
    {
        // Vaciar la cola de eventos
    }
}

/**
 * @file     defines.h
 *
 * @brief    Definiciones de los tipos enumerados y estructuras para el proyecto.
 * 
 * @version  0.6   (2024/06/06) Traducción de los comentarios al español
 * @version  0.5   (2024/05/07) Solucionado bug en la clase box_t "bucle infinito"
 * @version  0.4.1 (2024/05/06) Añadido cálculo de TCP pose de todos los items
 * @version  0.4   (2024/04/28) Añadida función (generate_mqtt_order)
 * @version  0.3.2 (2024/04/27) Añadido cálculo de TCP pose de PULSERA y RELOJ
 * @version  0.3.1 (2024/04/26) Añadido cálculo de TCP pose de TELEFONO y TABLET
 * @version  0.2   (2024/03/20) Añadido funciones de log
 * @version  0.1   (2024/03/15) Prototipo inicial funcional
 *
 * @author   Grupo PR2-A04' <mbelmar@etsinf.upv.es>
 * 
 * @date     Junio, 2024
 * @section  PR2-GIIROB
 */

#ifndef DEFINES_T
#define DEFINES_T

typedef enum 
{
    PULSERA,
    RELOJ,
    FUNDA_TELEFONO,
    TELEFONO,
    FUNDA_EREADER,
    EREADER,
    FUNDA_TABLET,
    TABLET

} itemType_t;

typedef enum 
{
    BOX_S,
    BOX_M,
    BOX_L

} boxType_t;

typedef struct
{
    uint16_t x, y, z;

} point_t;

#endif /* DEFINES_T */

/*** end of file ***/
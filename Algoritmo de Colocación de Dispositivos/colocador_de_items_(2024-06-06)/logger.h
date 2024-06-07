/**
 * @file     logger.h
 *
 * @brief    Implementación y definición de las funciones de logger.
 * 
 * En este fichero se definen hasta seis niveles diferentes para realizar
 * la depuración del código por la terminal. Para cada nivel hay disponibles
 * dos versiones, imprimir un mensaje pasando a la línea siguiente o
 * manteniéndose en la misma línea.
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

#include <iostream>

#define TRACE 6
#define DEBUG 5
#define INFO  4
#define WARN  3
#define ERROR 2
#define FATAL 1
#define NONE  0

#define LOG_LEVEL NONE // levels on logger: TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NONE

#ifdef LOG_LEVEL

#define traceln(tag, message) if ( LOG_LEVEL >= TRACE ) { printf("[TRACE] "); printf("%s: %s\n", tag, message); }
#define debugln(tag, message) if ( LOG_LEVEL >= DEBUG ) { printf("[DEBUG] "); printf("%s: %s\n", tag, message); }
#define infoln(tag, message)  if ( LOG_LEVEL >= INFO  ) { printf("[INFO] " ); printf("%s: %s\n", tag, message); }
#define warnln(tag, message)  if ( LOG_LEVEL >= WARN  ) { printf("[WARN] " ); printf("%s: %s\n", tag, message); }
#define errorln(tag, message) if ( LOG_LEVEL >= ERROR ) { printf("[ERROR] "); printf("%s: %s\n", tag, message); }
#define fatalln(tag, message) if ( LOG_LEVEL >= FATAL ) { printf("[FATAL] "); printf("%s: %s\n", tag, message); }

#else
#define traceln(message)
#define debugln(message)
#define infoln(message)
#define warnln(message)
#define errorln(message)
#define fatalln(message)

#endif

/*** end of file ***/
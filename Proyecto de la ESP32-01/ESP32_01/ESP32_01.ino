/**
 * @file     ESP32-01.ino
 *
 * @brief    Solución de la ESP32-01 basada en la 'Plantilla Dispositivo IoT'.
 * 
 * En este fichero se definen las funciones principales de Arduino, el setup(),
 * donde se realizan las inicializaciones, y el loop(), función que se ejecuta
 * de forma indefinida.
 * 
 * @version  0.8   (2024/06/05) Versión para la entrega del día 6 de junio
 * @version  0.7   (2024/04/21) Aplicando estándar de programación BARR-C
 * @version  0.6   (2024/02/21) JSON Management
 * @version  0.5.1 (2024/02/20) SSL fork
 * @version  0.5   (2024/02/19) Mejora en comunicaciones y reconexiones
 * @version  0.4   (2024/02/15) Reestructuración y simplificación
 * @version  0.3   (2024/02/14) Añadido funciones de log
 * @version  0.2   (2023/12/28) Dividido en sub-ficheros
 * @version  0.1   (2023/11/29) Prototipo Inicial Funcional
 *
 * @authors  Joan Fons' <jjfons@dsic.upv.es>, Grupo PR2-A04' <mbelmar@etsinf.upv.es>
 * @date     Junio, 2024
 * @section  PR2-GIIROB
 */

//-----[ LIBRERIAS NECESARIAS ]-----------------------------//

#include "config.h"

#include <WiFi.h>
#ifdef SSL_ROOT_CA
#include <WiFiClientSecure.h>
#endif
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ID del Dispositivo: se proporciona el ID asignado en la planta automatizada.
String deviceID = String("PR2-A04-device-") + String(DEVICE_ID);        

/******************************************************************************/
/*!
 * @brief  Este setup configura conceptos 'core', inicializa la wifi y la 
 *         conexión con el bróker MQTT, y ejecuta los siguientes métodos:
 *         - suscribirseATopics()  ->  topics MQTT a suscribir para recibir
 *                                     mensajes (h_comunicaciones.ino)
 *         - on_setup()            ->  configuración de los pines, inicializa-
 *                                     ción de variables, etc. (s_setup.ino)
 * @param  void
 * @return void
 */
void setup(void)
{
    #ifdef LOGGER_ENABLED
    // Inicializamos comunicaciones serial.
    Serial.begin(BAUDS);
    delay(1000);
    Serial.println();
    #endif

    // Nos conectamos a la wifi.
    wifi_connect();

    // Nos conectamos al broker MQTT, indicando un 'client-id'.
    mqtt_connect(deviceID);

    // Función de (h_comunicaciones.ino)
    suscribirseATopics();

    // Función de (s_setup.ino)
    on_setup();

}   /* setup() */

/******************************************************************************/
/*!
 * @brief  Este loop ejecuta los siguientes métodos en bucle:
 *         - wifi_loop()  ->  función bucle WiFi (d_wifi_lib_no_tocar.ino)
 *         - mqtt_loop()  ->  función bucle MQTT (e_mqtt_lib_no_tocar.ino)
 *         - on_loop()    ->  tareas a realizar dentro del 'loop' (w_loop.ino)
 * @param  void
 * @return void
 */
void loop(void)
{
    // NO QUITAR (jjfons)
    wifi_loop();
    mqtt_loop();

    // Función de (w_loop.ino)
    on_loop();

}   /* loop() */

/*** end of file ***/

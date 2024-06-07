/**
 * @file     c_config.h
 *
 * @brief    Donde se crean las definiciones, tipos
 *           enumerados y estructuras necesarias.
 *
 * @version  0.2 (versión ESP32-02)
 * @authors  Joan Fons' <jjfons@dsic.upv.es>, Grupo PR2-A04' <mbelmar@etsinf.upv.es>
 * @date     Junio, 2024
 * @section  PR2-GIIROB
 */

//-----[ LIBRERIAS NECESARIAS ]-----------------------------//

#include "buffer_circular_t.h"

//-----[ COMM BAUDS ]---------------------------------------//

#define BAUDS 115200
#define LOGGER_ENABLED             // Comentar para deshabilitar el logger por consola serie.
#define LOG_LEVEL TRACE            // Niveles en c_logger: TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NONE.


//-----[ IDENTIFICADOR DEL DISPOSITIVO ]--------------------//

#define DEVICE_ID                  "02"


//-----[ WIFI ]---------------------------------------------//

#define NET_SSID                   "****"
#define NET_PASSWD                 "****"


//-----[ MQTT ]---------------------------------------------//

#if 0
    #define MQTT_SERVER_IP         "mqtt.dsic.upv.es"
    #define MQTT_SERVER_PORT        1883
    #define MQTT_USERNAME          "giirob"
    #define MQTT_PASSWORD          "UPV2024"
#else
    #define MQTT_SERVER_IP         "broker.emqx.io"
    #define MQTT_SERVER_PORT        1883
    #define MQTT_USERNAME          "emqx"
    #define MQTT_PASSWORD          "public"
#endif

// Topics para publicar:
#define ACCION_RETENEDOR_TOPIC     "giirob/PR2/A04/escenario/cinta/01/retenedor/accion"
#define ACCION_CINTA_01_TOPIC      "giirob/PR2/A04/escenario/cinta/01/cinta/accion"
#define ACCION_CINTA_02_TOPIC      "giirob/PR2/A04/escenario/cinta/02/cinta/accion"
#define ORDEN_INDUSTRIAL_TOPIC     "giirob/PR2/A04/escenario/estanterias/robot/orden"
#define ORDEN_PERSONAL_TOPIC       "giirob/PR2/A04/escenario/personal/orden"
#define PROMEDIO_TIEMPOS_TOPIC     "giirob/PR2/A04/escenario/promedio_tiempos_llenado"

// Topics para suscripciones:
#define PULSADOR_EMERGENCIA_TOPIC  "giirob/PR2/A04/escenario/boton_emergencia/estado"
#define SENSOR_INICIO_CINTA_TOPIC  "giirob/PR2/A04/escenario/cinta/01/sensores/inicio_cinta"
#define SENSOR_FINAL_CINTA_TOPIC   "giirob/PR2/A04/escenario/cinta/01/sensores/final_cinta"
#define LECTOR_QR_TOPIC            "giirob/PR2/A04/escenario/cinta/01/sensores/lectorQR"
#define INDUSTRIAL_AVISO_TOPIC     "giirob/PR2/A04/escenario/estanterias/robot/caja_llena"


//-----[ VARIABLES PARA LA GESTIÓN DE LAS CINTAS ]----------//

typedef enum 
{
    NO_HAY_CAJA,
    HAY_CAJA,
    DESCONOCIDO
} estado_sensor_t;

typedef enum 
{
    CAJA_S,
    CAJA_M,
    CAJA_L,
    NINGUNA
} tipo_caja_t;

typedef struct
{
    estado_sensor_t sensor_inicio_cinta = NO_HAY_CAJA;
    estado_sensor_t sensor_final_cinta  = NO_HAY_CAJA;

    // Variables útiles para el cálculo de tiempos de llenado de las cajas.
    tipo_caja_t  caja_actual_en_llenado = NINGUNA;
    unsigned long tiempo_inicio_llenado = 0.0;

    // Buffers Circulares para almacenar los tiempos 
    // llenado de cada una de las cajas por separado.
    //
    buffer_circular_t<unsigned long> buffer_tiempos_de_llenado_caja_s;
    buffer_circular_t<unsigned long> buffer_tiempos_de_llenado_caja_m;
    buffer_circular_t<unsigned long> buffer_tiempos_de_llenado_caja_l;

    uint8_t caja_en_la_cinta_1 = 0;
    bool nueva_caja_llena      = false;
    portMUX_TYPE taskMux       = portMUX_INITIALIZER_UNLOCKED;

    // Variable para almacenar la lectura QR realizada por la ESP32-01.
    char * lectura_QR = "esperando_lectura";
} gestion_cintas_t;

gestion_cintas_t gestion_cintas;


/*** end of file ***/

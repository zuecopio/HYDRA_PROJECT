/**
 * @file     config.h
 *
 * @brief    Donde se crean las definiciones, tipos
 *           enumerados y estructuras necesarias.
 *
 * @version  0.1 (versión ESP32-01)
 * @authors  Joan Fons' <jjfons@dsic.upv.es>, Grupo PR2-A04' <mbelmar@etsinf.upv.es>
 * @date     Junio, 2024
 * @section  PR2-GIIROB
 */

//-----[ LIBRERIAS NECESARIAS ]-----------------------------//

#include "src/quirc/quirc.h"
#include "esp_camera.h"

//-----[ COMM BAUDS ]---------------------------------------//

#define BAUDS 115200
#define LOGGER_ENABLED             // Comentar para deshabilitar el logger por consola serie.
#define LOG_LEVEL TRACE            // Niveles en c_logger: TRACE, DEBUG, INFO, WARN, ERROR, FATAL, NONE.

//-----[ IDENTIFICADOR DEL DISPOSITIVO ]--------------------//

#define DEVICE_ID                  "01"


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
#define PULSADOR_EMERGENCIA_TOPIC  "giirob/PR2/A04/escenario/boton_emergencia/estado"
#define SENSOR_INICIO_CINTA_TOPIC  "giirob/PR2/A04/escenario/cinta/01/sensores/inicio_cinta"
#define SENSOR_FINAL_CINTA_TOPIC   "giirob/PR2/A04/escenario/cinta/01/sensores/final_cinta"
#define LECTOR_QR_TOPIC            "giirob/PR2/A04/escenario/cinta/01/sensores/lectorQR"

// Topics para suscripciones:
#define ACCION_RETENEDOR_TOPIC     "giirob/PR2/A04/escenario/cinta/01/retenedor/accion"
#define ACCION_CINTA_01_TOPIC      "giirob/PR2/A04/escenario/cinta/01/cinta/accion"
#define ACCION_CINTA_02_TOPIC      "giirob/PR2/A04/escenario/cinta/02/cinta/accion"


//-----[ SENSORES DE PRESENCIA ]----------------------------//

// Este tipo enumerado se usa para seleccionar
// con que sensor llamar a getUsDistance().
//
typedef enum 
{
    inicio_cinta,
    final_cinta
} tipo_sensor_t;

typedef enum 
{
    NO_HAY_CAJA,
    HAY_CAJA,
    DESCONOCIDO
} estado_sensor_t;

// Variable global que será modificada por las
// tareas por medio del parámetro de creación.
//
typedef struct
{
    estado_sensor_t sensor_inicio_cinta;
    estado_sensor_t sensor_final_cinta;
} estado_cinta_t;

estado_cinta_t estado_cinta = {NO_HAY_CAJA, NO_HAY_CAJA};

#define trigPin_inicio_cinta  39    // Definición del TrigPin para el sensor_inicio_cinta.
#define echoPin_inicio_cinta  40    // Definición del EchoPin para el sensor_inicio_cinta.
#define trigPin_final_cinta   41    // Definición del TrigPin para el sensor_final_cinta.
#define echoPin_final_cinta   42    // Definición del EchoPin para el sensor_final_cinta.
#define MAX_DISTANCE         700    // La distancia máxima del sensor es de 400-500 cm.

int   soundVelocity = 340;          // Definir la velocidad del sonido = 340 m/s.
float timeOut = MAX_DISTANCE * 60;

//-----[ CÁMARA PARA LECTURA DE QR ]------------------------//

// GPIO de la CAMERA_MODEL_ESP32S3_EYE.
//
#define PWDN_GPIO_NUM   -1
#define RESET_GPIO_NUM  -1
#define XCLK_GPIO_NUM   15
#define SIOD_GPIO_NUM    4
#define SIOC_GPIO_NUM    5

#define Y9_GPIO_NUM     16
#define Y8_GPIO_NUM     17
#define Y7_GPIO_NUM     18
#define Y6_GPIO_NUM     12
#define Y5_GPIO_NUM     10
#define Y4_GPIO_NUM      8
#define Y3_GPIO_NUM      9
#define Y2_GPIO_NUM     11
#define VSYNC_GPIO_NUM   6
#define HREF_GPIO_NUM    7
#define PCLK_GPIO_NUM   13
 
// Declaración de variables.
//
struct QRCodeData
{
    bool valid;
    int dataType;
    uint8_t payload[1024];
    int payloadLen;
};

struct quirc * q = NULL;
uint8_t * image  = NULL;  
camera_fb_t * fb = NULL;
struct quirc_code code;
struct quirc_data data;
quirc_decode_error_t err;
struct QRCodeData qrCodeData;  
String QRCodeResult = "";


//-----[ PULSADOR DE EMERGENCIA ]---------------------------//

#define EMERGENCY_BUTTON  1

typedef struct
{
    const uint8_t PIN;
    bool pressed;
} button_t;

// Variable global que será modificada por el gestor
// de interrupción para el pulsador de emergencia isr().
//
button_t emergency_button = {EMERGENCY_BUTTON, false};


//-----[ RETENEDOR (SERVOMOTOR) ]---------------------------//

typedef enum 
{
    RETENER_S,
    RETENER_M_L,
    SOLTAR
} accion_retenedor_t;

#define RETENEDOR_PIN  35   // Definir PIN para la salida PWM.
#define SERVO_CHN       0   // Definir el canal PWM.
#define SERVO_FRQ      50   // Frecuencia de PWM.
#define SERVO_BIT      12   // Bits usados para codificar el PWM.


//-----[ MOTORES DE LAS (MOTORES CC) ]----------------------//

#define EN_MARCHA     0x1  // HIGH
#define DETENER       0x0  // LOW

// Definir PIN para la simular funcionamiento de las cintas.
//
#define motor_cinta_1 37
#define motor_cinta_2 38

/*** end of file ***/

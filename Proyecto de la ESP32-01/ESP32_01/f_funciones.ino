/**
 * @file   f_funciones.ino
 * @brief  Definición e implementación de las funciones
 *         de inicialización y gestión del hardware.
 */

/******************************************************************************/
/*!
 * @brief  Gestor de interrupción para el pulsador de emergencia. Esta función
 *         se aloja en la memoria RAM (hay que usar la directiva de IRAM_ATTR).
 * @param  void
 * @return void
 */
void IRAM_ATTR isr(void)
{
    emergency_button.pressed = true;

}   /* isr() */

/******************************************************************************/
/*!
 * @brief  Función para conocer la distancia detectada por el ultrasonido.
 * @param  sensor  Indica qué sensor de la cinta se desea
 *                 utilizar para realizar la medición.
 * @return Devuelve la distancia en centímetros en formato float.
 */
float getUsDistance(tipo_sensor_t sensor)
{
    uint8_t trigPin_sensor;
    uint8_t echoPin_sensor;
    unsigned long pingTime;
    float distance;

    if (sensor == inicio_cinta) 
    {
        trigPin_sensor = trigPin_inicio_cinta;
        echoPin_sensor = echoPin_inicio_cinta;
    }
    else // sensor == final_cinta
    {
        trigPin_sensor = trigPin_final_cinta;
        echoPin_sensor = echoPin_final_cinta;
    }
    
    // Hace que la salida de trigPin tenga un nivel alto que dure 10 μs para activar el HC-SR04.
    digitalWrite(trigPin_sensor, HIGH); 
    delayMicroseconds(10);
    digitalWrite(trigPin_sensor, LOW);
    
    // Espera que el HC-SR04 vuelva al nivel alto y mide el tiempo de espera.
    pingTime = pulseIn(echoPin_sensor, HIGH, timeOut); 
    
    // Calcula la distancia según el tiempo.
    distance = (float)pingTime * soundVelocity / 2.0 / 10000.0;

    #if 1
        trace("Distancia medida por ");
        if (sensor == inicio_cinta)
        {
            trace("sensor_inicio_cinta: ");
        }
        else // (sensor == final_cinta)
        {
            trace("sensor_final_cinta: ")
        }
        
        trace(distance); traceln(" cm")
    #endif
    
    // Devuelve el valor de la distancia en centímetros.
    return distance;
    
}   /* getUsDistance() */

/******************************************************************************/
/*!
 * @brief  Función para inicializar la cámara de la ESP32.
 * @param  void
 * @return void
 */
void camera_init(void)
{
    // Configuración de la cámara.
    //
    traceln("camera_init() - Se empieza a inicializar y configurar la cámara ...");
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = Y2_GPIO_NUM;
    config.pin_d1       = Y3_GPIO_NUM;
    config.pin_d2       = Y4_GPIO_NUM;
    config.pin_d3       = Y5_GPIO_NUM;
    config.pin_d4       = Y6_GPIO_NUM;
    config.pin_d5       = Y7_GPIO_NUM;
    config.pin_d6       = Y8_GPIO_NUM;
    config.pin_d7       = Y9_GPIO_NUM;
    config.pin_xclk     = XCLK_GPIO_NUM;
    config.pin_pclk     = PCLK_GPIO_NUM;
    config.pin_vsync    = VSYNC_GPIO_NUM;
    config.pin_href     = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn     = PWDN_GPIO_NUM;
    config.pin_reset    = RESET_GPIO_NUM;
    config.xclk_freq_hz = 10000000;
    config.pixel_format = PIXFORMAT_GRAYSCALE;
    config.frame_size   = FRAMESIZE_QVGA;
    config.jpeg_quality = 15;
    config.fb_count     = 1;

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        trace("camera_init() - Error al inicializar la cámara."); traceln(err);
        ESP.restart();
    }

    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_QVGA);

    traceln("camera_init() - Configuración e inicialización de la cámara exitosas.");

}   /* camera_init() */

/******************************************************************************/
/*!
 * @brief  Función para activar la cámara de la ESP32 para leer un código QR.
 * @param  void
 * @return Devuelve el contenido del código QR en formato String.
 */
String camera_get_QR(void)
{
    String id_caja;

    q = quirc_new();
    if (q == NULL)
    {
        infoln("camera_get_QR() - No se puede crear el objeto quirc.");  
        // continue;
    }

    fb = esp_camera_fb_get();
    if (!fb)
    {
        infoln("camera_get_QR() - Falló la captura de la cámara.");
        // continue;
    }   

    quirc_resize(q, fb->width, fb->height);
    image = quirc_begin(q, NULL, NULL);
    memcpy(image, fb->buf, fb->len);
    quirc_end(q);

    int count = quirc_count(q);
    if (count > 0)
    {
        quirc_extract(q, 0, &code);
        err = quirc_decode(&code, &data);

        if (err)
        {
            infoln("camera_get_QR() - Decodificación FALLIDA.");
            id_caja = "Decodificación FALLIDA";
        }
        else
        {
            infoln("camera_get_QR() - Decodificación CORRECTA:");
            id_caja = (const char *)data.payload;
            info("camera_get_QR() - Payload: "); infoln((const char *)data.payload);
        } 
        
    } 

    esp_camera_fb_return(fb);
    fb = NULL;
    image = NULL;  
    quirc_destroy(q);

    return id_caja;

}   /* camera_get_QR() */ 

/******************************************************************************/
/*!
 * @brief  Función para inicializar el retenedor (servomotor en la demo).
 * @param  int  Es el pin al que se quiere conectar en la placa.
 * @return void
 */
void retenedor_init(int pin)
{
    ledcSetup(SERVO_CHN, SERVO_FRQ, SERVO_BIT);
    ledcAttachPin(pin, SERVO_CHN);
    
    long pwm_value = map(0, 0, 180, 103, 512);
    ledcWrite(SERVO_CHN, pwm_value);

}   /* servo_set_pin() */

/******************************************************************************/
/*!
 * @brief  Función para accionar el retenedor (servomotor en la demo).
 * @param  accion_retenedor  Posición en la que se quiere colocar el retenedor.
 * @return void
 */
void retenedor_set_accion(accion_retenedor_t accion_retenedor)
{
    long pwm_value;

    if (accion_retenedor == RETENER_S)
    {
        long pwm_value = map(90, 0, 180, 103, 512);
    }
    else if (accion_retenedor == RETENER_M_L)
    {
        long pwm_value = map(180, 0, 180, 103, 512);
    }
    else // (accion_retenedor == SOLTAR)
    {
        long pwm_value = map(0, 0, 180, 103, 512);
    }

    ledcWrite(SERVO_CHN, pwm_value);

}   /* servo_set_angle() */

/*** end of file ***/

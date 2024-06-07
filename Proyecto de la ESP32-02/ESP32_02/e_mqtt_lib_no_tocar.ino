/**
 * @file   e_mqtt_lib_no_tocar.ino
 * @brief  Definición e implementación de las funciones de MQTT.
 */

#define MQTT_CONNECTION_RETRIES 3

PubSubClient mqttClient(espWifiClient);

// MQTT CONFIG
const char * mqttServerIP   = MQTT_SERVER_IP;
unsigned int mqttServerPort = MQTT_SERVER_PORT;
String mqttClientID;

/******************************************************************************/
/*!
 * @brief  Función bucle MQTT.
 * @param  void
 * @return void
 */
void mqtt_loop(void)
{
    if (!mqttClient.connected())
    {
        mqtt_reconnect(MQTT_CONNECTION_RETRIES);
        suscribirseATopics();
    }

    mqttClient.loop();

}   /* mqtt_loop() */

/******************************************************************************/
/*!
 * @brief  Función de conexión MQTT. Se configura el cliente MQTT y se
 *         configura 'mqttCallback' como la función que se invocará al
 *         recibir datos por las suscripciones realizadas.
 * @param  clientID  (String)
 * @return void
 */
void mqtt_connect(String clientID)
{
    // Configuramos cliente MQTT.
    mqttClientID = String(clientID);
    mqttClient.setServer(mqttServerIP, mqttServerPort);

    // Configuramos 'mqttCallback' como la función que se
    // invocará al recibir datos por las suscripciones realizadas.
    mqttClient.setCallback(mqttCallback);

    // Conectamos.
    mqtt_reconnect(MQTT_CONNECTION_RETRIES);

}   /* mqtt_connect() */

/******************************************************************************/
/*!
 * @brief  Función de reconexión MQTT.
 * @param  retries  (int)
 * @return void
 */
void mqtt_reconnect(int retries)
{
    if (!WiFi.isConnected())
    {
        return;
    }

    if (!mqttClient.connected())
    {
        warnln("Desconectado del broker MQTT");
    }

    // Bucle hasta que volvamos a conectarnos, o varios reintentos ...
    int r = 0;
    while ((!mqttClient.connected()) && (r < retries))
    {
        r++;

        trace("Intentando una conexión MQTT a: 'mqtt://");
        trace(mqttServerIP);
        trace(":");
        trace(mqttServerPort);
        trace("' con id-cliente: '");
        trace(mqttClientID);
        traceln("' ... ");

        #ifdef MQTT_USERNAME
        if (mqttClient.connect(mqttClientID.c_str(), MQTT_USERNAME, MQTT_PASSWORD))
        {
        #else
        if (mqttClient.connect(mqttClientID.c_str()))
        {
        #endif
            debugln("-=- Conectado al broker MQTT");
            // Damos tiempo a que la conexión se establezca por completo.
            delay(1000);
        }
        else
        {
            debug("-X- fallo, rc=");
            debugln(mqttClient.state());
            debugln("-R-   reintentado en 5 segundos");
            // Espera 5 segundos antes de volver a intentarlo.
            delay(5000);
        }
    }

}   /* mqtt_reconnect() */

/******************************************************************************/
/*!
 * @brief  Función callback MQTT. Función que se invocará automáticamente al
 *         recibir datos por algún topic sobre el que nos hayamos suscrito.
 * @param  topic    (char *)
 * @param  message  (byte *)
 * @param  length   (unsigned int)
 * @return void
 */
void mqttCallback(char * topic, byte * message, unsigned int length)
{
    // Función que se invocará automáticamente al recibir datos
    // por algún topic sobre el que nos hayamos suscrito.

    // Cargamos los datos recibidos en una variable.
    String incomingMessage;
    for (int i = 0; i < length; i++)
    {
        incomingMessage += (char)message[i];
    }

    traceln("<<~~ RECIBIENDO un mensaje MQTT:");
    traceln(topic);
    traceln(incomingMessage);

    alRecibirMensajePorTopic(topic, incomingMessage);

}   /* mqttCallback() */

/******************************************************************************/
/*!
 * @brief  Función de publicación de mensajes.
 * @param  topic            (const char *)
 * @param  outgoingMessage  (String)
 * @return void
 */
void mqtt_publish(const char * topic, String outgoingMessage)
{
    if (!mqttClient.connected())
    {
        error("No se puede enviar un mensaje a través del topic ... ");
        errorln("¡¡el Cliente MQTT está desconectado!!");
        return;
    }

    traceln("~~>> PUBLICANDO un mensaje MQTT:");
    traceln(topic);
    traceln(outgoingMessage);
    mqttClient.publish(topic, outgoingMessage.c_str());

}   /* mqtt_publish() */

/******************************************************************************/
/*!
 * @brief  Función de suscripción a los topics.
 * @param  topic  (const char *)
 * @return void
 */
void mqtt_subscribe(const char * topic)
{
    if (!mqttClient.connected())
    {
        error("No se puede suscribir al topic... ");
        errorln("¡¡el cliente MQTT está desconectado!!");
        return;
    }

    trace("Suscrito al topic: ");
    traceln(topic);
    mqttClient.subscribe(topic);

}   /* mqtt_subscribe() */

/*** end of file ***/

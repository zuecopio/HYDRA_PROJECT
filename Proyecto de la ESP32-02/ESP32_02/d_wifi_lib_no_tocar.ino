/**
 * @file   d_wifi_lib_no_tocar.ino
 * @brief  Definición e implementación de funciones de biblioteca WiFi.
 */

#define WIFI_CONNECTION_TIMEOUT_SECONDS 15

// Usamos comunicaciones TLS/SSL si se define el certificado raíz CA.
#ifdef SSL_ROOT_CA
WiFiClientSecure espWifiClient;
#else
WiFiClient espWifiClient;
#endif

const char * wifiSSID   = NET_SSID;
const char * wifiPasswd = NET_PASSWD;

/******************************************************************************/
/*!
 * @brief  Función bucle WiFi.
 * @param  void
 * @return void
 */
void wifi_loop(void)
{
    if (!WiFi.isConnected())
    {
        wifi_reconnect(WIFI_CONNECTION_TIMEOUT_SECONDS);
    }

}   /* wifi_loop() */

/******************************************************************************/
/*!
 * @brief  Función de conexión WiFi.
 * @param  void
 * @return void
 */
void wifi_connect(void)
{
    delay(10);

    // Opcional.
    WiFi.mode(WIFI_STA);
    trace("Dirección MAC: ");
    traceln(WiFi.macAddress());

    #ifdef SSL_ROOT_CA
    // Establecer certificado de CA raíz.
    espWifiClient.setCACert(SSL_ROOT_CA);
    traceln("Estableciendo comunicaciones TLS/SSL ...");
    #endif

    #ifdef SSL_CLIENT_CERTIFICATE
    espWifiClient.setCertificate(SSL_CLIENT_CERTIFICATE);
    espWifiClient.setPrivateKey(SSL_CLIENT_PRIVATE_KEY);
    traceln("Permitir la validación SSL con Certificado de Cliente");
    #endif

    wifi_reconnect(WIFI_CONNECTION_TIMEOUT_SECONDS);

}   /* wifi_connect() */

/******************************************************************************/
/*!
 * @brief  Función de reconexión WiFi.
 * @param  retries (uint)
 * @return void
 */
void wifi_reconnect(uint retries)
{
    trace("Conectando a ");
    traceln(wifiSSID);
    WiFi.begin(wifiSSID, wifiPasswd);

    uint8_t r = 0;
    while (WiFi.status() != WL_CONNECTED && r < retries)
    {
        r++;
        delay(1000);
        trace(".");
    }

    traceln("");

    if (WiFi.isConnected())
    {
        debugln("-=- Conectado a la red WiFi");
        debug("IP local de la ESP32: ");
        debugln(WiFi.localIP().toString());
    }
    else
    {
        errorln("-X- No se puede conectar a la red WiFi");
    }

}   /* wifi_reconnect() */

/*** end of file ***/

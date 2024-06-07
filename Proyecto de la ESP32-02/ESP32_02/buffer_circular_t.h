/**
 * @file     buffer_circular_t.h
 *
 * @brief    Definición e implementación de la clase templatizada Buffer Circular.
 *
 * @authors  Grupo PR2-A04' <mbelmar@etsinf.upv.es>
 * @date     Junio, 2024
 * @section  PR2-GIIROB
 */

#ifndef BUFFER_CIRCULAR_T
#define BUFFER_CIRCULAR_T

#define TRUE     1
#define FALSE    0
#define BUFSIZE 10

typedef enum
{
    buffer_lleno,
    buffer_vacio,
    ningun_error
} err_buffer_t;

template <typename T>
class buffer_circular_t
{
    private:

        //-----[ ATRIBUTOS PRIVADOS DE LA CLASE ]-------------------//
        T datos[BUFSIZE];
        int bufIN;
        int bufOUT;
        int contador;

    public:

        //-----[ ATRIBUTOS PÚBLICOS DE LA CLASE ]-------------------//
        portMUX_TYPE taskMux;

        //-----[ MÉTODOS PÚBLICOS DE LA CLASE ]---------------------//

        /******************************************************************************/
        /*!
        * @brief  Método constructor del Buffer Circular.
        * @param  void
        */
        buffer_circular_t(void)
        {
            bufIN    = 0;
            bufOUT   = 0;
            contador = 0;
            taskMux  = portMUX_INITIALIZER_UNLOCKED;
        
        }   /* buffer_circular_t() */

        /******************************************************************************/
        /*!
        * @brief  Método destructor del Buffer Circular.
        * @param  void
        */
        ~buffer_circular_t(void)
        {
        }   /* ~buffer_circular_t */

        /******************************************************************************/
        /*!
        * @brief  Método  para saber si el Buffer Circular está vacío.
        * @param  void
        * @return True or False.
        */
        bool isEmpty(void)
        {
            if (this->contador == 0)
            {
                return true;
            }
            else
            {
                return false;
            }

        }   /* isEmpty() */

        /******************************************************************************/
        /*!
        * @brief  Método para saber si el Buffer Circular está lleno.
        * @param  void
        * @return True or False.
        */
        bool isFull(void)
        {
            if (this->contador == BUFSIZE)
            {
                return true;
            }
            else
            {
                return false;
            }

        }   /* isFull() */

        /******************************************************************************/
        /*!
        * @brief  Método para obtener un elemento del Buffer Circular.
        * @param  T  Puntero a la variable donde se quiere
        *            guardar el valor del elemento del Buffer.
        * @return Devuelve 'buffer_vacio' si no cabe ningún elemento
        *         más, si no devuelve el elemento obtenido.
        */
        err_buffer_t get_item(T * X)
        {
            if (isEmpty())
            {
                return buffer_vacio;
            }
            else
            {
                portENTER_CRITICAL(&(this->taskMux));

                *X = this->datos[this->bufOUT];
                this->contador--;
                // Aritmética en módulo del índice del vector.
                this->bufOUT = (this->bufOUT + 1) % BUFSIZE;

                portEXIT_CRITICAL(&(this->taskMux));
                return ningun_error;	
            }

        }   /* get_item() */

        /******************************************************************************/
        /*!
        * @brief  Método para introducir un elemento en el Buffer Circular.
        * @param  T  Elemento que se quiere guardar en el Buffer.
        * @return Devuelve 'buffer_lleno' si no cabe ningún elemento más.
        */
        err_buffer_t put_item(T X)
        {
            if (isFull())
            {
                return buffer_lleno;
            }
            else
            {
                portENTER_CRITICAL(&(this->taskMux));

                this->datos[this->bufIN] = X;
                this->contador++;
                // Aritmética en módulo del índice del vector.
                this->bufIN = (this->bufIN + 1) % BUFSIZE;

                portEXIT_CRITICAL(&(this->taskMux));
                return ningun_error;	
            }

        }   /* put_item() */

        /******************************************************************************/
        /*!
        * @brief  Método para saber cuántos elementos tiene el Buffer Circular.
        * @param  void
        * @return Devuelve el número de elementos.
        */
        int number(void)
        {
            return this->contador;

        }   /* number() */

        /******************************************************************************/
        /*!
        * @brief  Método  para saber cuántos elementos puede almacenar el Buffer Circular.
        * @param  void
        * @return Devuelve el tamaño del Buffer Circular.
        */
        int size(void)
        {
            return BUFSIZE;

        }   /* size() */

};

#endif /* BUFFER_CIRCULAR_T */

/*** end of file ***/

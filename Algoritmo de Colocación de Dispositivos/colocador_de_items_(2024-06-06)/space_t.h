/**
 * @file     space_t.h
 *
 * @brief    Dados dos puntos, se define un volumen en el espacio.
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

#ifndef SPACE_T_H
#define SPACE_T_H

#include <ostream>
#include <string>
#include "defines.h"
#include "logger.h"

// static const char * SPACE_TAG = __FILE__;
static const char * SPACE_TAG = "space_t.h";

using namespace std;

class space_t
{
    private:
    
        // ATRIBUTOS.
        point_t min;
        point_t max;

    public:

        /******************************************************************************/
        /*!
         * @brief  Primer constructor de la clase space_t.
         * @param  void
         */
        space_t(void)
        {
            traceln(SPACE_TAG, "First space_t()");

            this->min.x = 0;
            this->min.y = 0;
            this->min.z = 0;

            this->max.x = 0;
            this->max.y = 0;
            this->max.z = 0;

            traceln(SPACE_TAG, "First space_t() - END");

        }   /* space_t() */

        /******************************************************************************/
        /*!
         * @brief  Segundo constructor de la clase space_t.
         * @param  x0  Coordenada x del punto mínimo.
         * @param  y0  Coordenada y del punto mínimo.
         * @param  z0  Coordenada z del punto mínimo.
         * @param  x1  Coordenada x del punto máximo.
         * @param  y1  Coordenada y del punto máximo.
         * @param  z1  Coordenada z del punto máximo.
         */
        space_t(uint16_t x0, uint16_t y0, uint16_t z0,
                uint16_t x1, uint16_t y1, uint16_t z1)
        {
            traceln(SPACE_TAG, "Second space_t()");

            this->min.x = x0;
            this->min.y = y0;
            this->min.z = z0;

            this->max.x = x1;
            this->max.y = y1;
            this->max.z = z1;

            traceln(SPACE_TAG, "Second space_t() - END");
    
        }   /* space_t() */

        /******************************************************************************/
        /*!
         * @brief  El destructor de clase space_t.
         * @param  void
         */
        ~space_t(void)
        {
            traceln(SPACE_TAG, "~space_t()");
            traceln(SPACE_TAG, "~space_t() - END");

        }   /* ~space_t() */

        /******************************************************************************/
        /*!
         * @brief  Establece las coordenadas del objeto space_t.
         * @param  x0  Coordenada x del punto mínimo.
         * @param  y0  Coordenada y del punto mínimo.
         * @param  z0  Coordenada z del punto mínimo.
         * @param  x1  Coordenada x del punto máximo.
         * @param  y1  Coordenada y del punto máximo.
         * @param  z1  Coordenada z del punto máximo.
         * @return void
         */
        void
        set_space(uint16_t x0, uint16_t y0, uint16_t z0,
                  uint16_t x1, uint16_t y1, uint16_t z1)
        {
            traceln(SPACE_TAG, "set_space()");

            this->min.x = x0;
            this->min.y = y0;
            this->min.z = z0;

            this->max.x = x1;
            this->max.y = y1;
            this->max.z = z1;

            traceln(SPACE_TAG, "set_space() - END");
        
        }   /* set_space() */

        /******************************************************************************/
        /*!
         * @brief  Indica si el objeto que llama a la función es un
         *         subconjunto del parámetro de entrada (otherSpace).
         * @param  otherSpace  El espacio a comparar.
         * @return Devuelve verdadero o falso.
         */
        bool
        is_subset_of(space_t otherSpace)
        {  
            traceln(SPACE_TAG, "is_subset_of()");
            bool answer = false;

            if (this->min.x >= otherSpace.min.x &&
                this->min.y >= otherSpace.min.y &&
                this->min.z >= otherSpace.min.z &&
                this->max.x <= otherSpace.max.x &&
                this->max.y <= otherSpace.max.y &&
                this->max.z <= otherSpace.max.z)
            {
                answer = true;
            }

            traceln(SPACE_TAG, "is_subset_of() - END");
            return (answer);

        }   /* is_subset_of() */

        /******************************************************************************/
        /*!
         * @brief  Indica si el objeto que llama a la función está
         *         formado por el parámetro de entrada (otherSpace).
         * @param  otherSpace  El espacio a comparar.
         * @return Devuelve verdadero o falso.
         */
        bool
        is_made_up_of(space_t otherSpace)
        {  
            traceln(SPACE_TAG, "is_made_up_of()");
            bool answer = false;

            if (this->min.x <= otherSpace.min.x &&
                this->min.y <= otherSpace.min.y &&
                this->min.z <= otherSpace.min.z &&
                this->max.x >= otherSpace.max.x &&
                this->max.y >= otherSpace.max.y &&
                this->max.z >= otherSpace.max.z)
            {
                answer = true;
            }

            traceln(SPACE_TAG, "is_made_up_of() - END");
            return (answer);

        }   /* is_made_up_of() */

        /******************************************************************************/
        /*!
         * @brief  Agrega un desplazamiento a un espacio.
         * @param  space         El espacio a desplazar.
         * @param  displacement  La cantidad que se quiere desplazar.
         * @return El nuevo espacio desplazado.
        */
        friend space_t
        operator+(const space_t & space, const point_t & displacement)
        {
            traceln(SPACE_TAG, "operator+()");
            space_t finalPose;

            finalPose.min.x = space.min.x + displacement.x;
            finalPose.min.y = space.min.y + displacement.y;
            finalPose.min.z = space.min.z + displacement.z;
            
            finalPose.max.x = space.max.x + displacement.x;
            finalPose.max.y = space.max.y + displacement.y;
            finalPose.max.z = space.max.z + displacement.z;
            
            traceln(SPACE_TAG, "operator+() - END");
            return finalPose;

        }   /* operator+() */

        /******************************************************************************/
        /*!
         * @brief  La función para saber si space1 es lo mismo que space2.
         * @param  space1  El primer espacio a comparar.
         * @param  space2  El segundo espacio a comparar.
         * @return Devuelve verdadero o falso.
         */
        friend bool
        operator!=(const space_t & space1, const space_t & space2)
        {
            traceln(SPACE_TAG, "operator!=()");
            bool answer = true;

            if (space1.min.x == space2.min.x &&
                space1.min.y == space2.min.y &&
                space1.min.z == space2.min.z &&
                space1.max.x == space2.max.x &&
                space1.max.y == space2.max.y &&
                space1.max.z == space2.max.z)
            {
                answer = false;
            }
            
            traceln(SPACE_TAG, "operator!=() - END");
            return (answer);

        }   /* operator!=() */

        /******************************************************************************/
        /*!
         * @brief  Convierte las coordenadas space_t en una string.
         * @param  void
         * @return String con los valores de las coordenadas.
         */
        string
        space_to_str(void)
        {
            traceln(SPACE_TAG, "space_to_str()");
            
            string coordinates;
            
            coordinates = "[(";
            coordinates += to_string(min.x) + ", ";
            coordinates += to_string(min.y) + ", ";
            coordinates += to_string(min.z) + ")(";
            coordinates += to_string(max.x) + ", ";
            coordinates += to_string(max.y) + ", ";
            coordinates += to_string(max.z) + ")]";

            traceln(SPACE_TAG, "space_to_str() - END");
            return (coordinates);

        }   /* space_to_str() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el valor del atributo min.x.
         * @param  void
         * @return Coordenada x del punto mínimo.
         */
        uint16_t
        min_x(void)
        {
            return min.x;
        
        }   /* min_x() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el valor del atributo min.y.
         * @param  void
         * @return Coordenada y del punto mínimo.
         */
        uint16_t
        min_y(void)
        {
            return min.y;
        
        }   /* min_y() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el valor del atributo min.z.
         * @param  void
         * @return Coordenada z del punto mínimo.
         */
        uint16_t
        min_z(void)
        {
            return min.z; 
        
        }   /* min_z() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el valor del atributo max.x.
         * @param  void
         * @return Coordenada x del punto máximo.
        */
        uint16_t
        max_x(void)
        {
            return max.x;
        
        }   /* max_x() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el valor del atributo max.y.
         * @param  void
         * @return Coordenada y del punto máximo.
         */
        uint16_t
        max_y(void)
        {
            return max.y;
        
        }   /* max_y() */

        /******************************************************************************/
        /*!
         * @brief  Devuelve el valor del atributo max.z.
         * @param  void
         * @return Coordenada z del punto máximo.
         */
        uint16_t
        max_z(void)
        {
            return max.z;

        }   /* max_z() */
};

#endif /* SPACE_T_H */

/*** end of file ***/
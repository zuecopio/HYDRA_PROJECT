/**
 * @file     main.cpp
 *
 * @brief    Tres ejemplos de uso, uno por cada tipo de caja. Dada una lista con
 *           dispositivos, se calculan sus posiciones de place dentro de caja y
 *           se genera la orden (formato JSON) para el robot industrial.
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
#include <list>
#include "defines.h"
#include "logger.h"
#include "item_t.h"
#include "box_t.h"

#define EJEMPLO_PEDIDO_S 1
#define EJEMPLO_PEDIDO_M 0
#define EJEMPLO_PEDIDO_L 0

using namespace std;

// static const char * TAG = __FILE__;
static const char * TAG = "main.cpp";

int main(void)
{
	#if EJEMPLO_PEDIDO_S
	item_t item_01("tablet_A_01");
	item_t item_02("tablet_A_01");
	item_t item_03("tablet_A_01");
	item_t item_04("ereader_B_02");
	item_t item_05("ereader_A_02");
	item_t item_06("ereader_B_funda");
	item_t item_07("ereader_B_funda");
	item_t item_08("ereader_A_funda");
	item_t item_09("ereader_A_funda");
	item_t item_10("telefono_C_03");
	item_t item_11("telefono_A_02");
	item_t item_12("telefono_F_02");
	item_t item_13("telefono_D_01");
	item_t item_14("telefono_D_01");
	item_t item_15("telefono_B_01");
	item_t item_16("reloj_B_01");
	item_t item_17("reloj_B_01");
	item_t item_18("reloj_B_01");
	item_t item_19("reloj_B_02");
	item_t item_20("reloj_B_02");
	item_t item_21("reloj_B_01");
	item_t item_22("telefono_B_01");
	item_t item_23("telefono_B_01");
	item_t item_24("telefono_D_funda");
	item_t item_25("telefono_D_funda");
	item_t item_26("telefono_B_funda");

	boxType_t caja_ejemplo = BOX_S;
	
	#elif EJEMPLO_PEDIDO_M
	item_t item_01("tablet_A_02");
	item_t item_02("tablet_A_02");
	item_t item_03("tablet_A_funda");
	item_t item_04("tablet_A_funda");
	item_t item_05("tablet_A_01");
	item_t item_06("tablet_A_01");
	item_t item_07("tablet_B_funda");
	item_t item_08("tablet_B_funda");
	item_t item_09("tablet_B_02");
	item_t item_10("tablet_B_02");
	item_t item_11("tablet_B_funda");
	item_t item_12("tablet_B_funda");
	item_t item_13("tablet_D_funda");
	item_t item_14("tablet_D_funda");
	item_t item_15("ereader_A_02");
	item_t item_16("ereader_A_01");
	item_t item_17("ereader_B_01");
	item_t item_18("ereader_B_01");
	item_t item_19("telefono_F_02");
	item_t item_20("telefono_D_01");
	item_t item_21("telefono_D_01");
	item_t item_22("telefono_B_01");
	item_t item_23("telefono_B_01");
	item_t item_24("telefono_B_01");
	item_t item_25("telefono_B_01");
	item_t item_26("telefono_D_01");

	boxType_t caja_ejemplo = BOX_M;

	#elif EJEMPLO_PEDIDO_L
	item_t item_01("tablet_A_02");
	item_t item_02("tablet_A_02");
	item_t item_03("tablet_A_02");
	item_t item_04("tablet_C_02");
	item_t item_05("tablet_C_02");
	item_t item_06("tablet_C_02");
	item_t item_07("tablet_B_01");
	item_t item_08("tablet_B_01");
	item_t item_09("tablet_B_01");
	item_t item_10("tablet_A_01");
	item_t item_11("tablet_A_01");
	item_t item_12("tablet_D_01");
	item_t item_13("tablet_A_funda");
	item_t item_14("tablet_A_funda");
	item_t item_15("tablet_B_funda");
	item_t item_16("tablet_B_funda");
	item_t item_17("tablet_A_funda");
	item_t item_18("tablet_A_funda");
	item_t item_19("tablet_A_funda");
	item_t item_20("tablet_A_funda");
	item_t item_21("ereader_B_01");
	item_t item_22("ereader_B_01");
	item_t item_23("ereader_B_01");
	item_t item_24("ereader_B_01");
	item_t item_25("telefono_B_01");
	item_t item_26("telefono_B_01");
	item_t item_27("telefono_B_01");
	item_t item_28("telefono_B_01");
	item_t item_29("telefono_B_01");
	item_t item_30("telefono_B_01");
	item_t item_31("pulsera_B_01");
	item_t item_32("pulsera_B_01");
	item_t item_33("pulsera_B_01");
	item_t item_34("pulsera_B_01");
	item_t item_35("pulsera_B_01");
	item_t item_36("pulsera_B_01");
	item_t item_37("pulsera_B_01");
	item_t item_38("pulsera_B_01");
	item_t item_39("pulsera_B_01");
	item_t item_40("pulsera_B_01");
	item_t item_41("pulsera_B_01");
	item_t item_42("pulsera_B_01");
	item_t item_43("telefono_B_funda");
	item_t item_44("telefono_B_funda");
	item_t item_45("telefono_B_funda");
	item_t item_46("telefono_B_funda");
	item_t item_47("telefono_B_funda");
	item_t item_48("telefono_B_funda");

	boxType_t caja_ejemplo = BOX_L;

	#endif

	list<item_t> itemsToPlaceInOrder;

	#if EJEMPLO_PEDIDO_S
	itemsToPlaceInOrder.push_back(item_01);
	itemsToPlaceInOrder.push_back(item_02);
	itemsToPlaceInOrder.push_back(item_03);
	itemsToPlaceInOrder.push_back(item_04);
	itemsToPlaceInOrder.push_back(item_05);
	itemsToPlaceInOrder.push_back(item_06);
	itemsToPlaceInOrder.push_back(item_07);
	itemsToPlaceInOrder.push_back(item_08);
	itemsToPlaceInOrder.push_back(item_09);
	itemsToPlaceInOrder.push_back(item_10);
	itemsToPlaceInOrder.push_back(item_11);
	itemsToPlaceInOrder.push_back(item_12);
	itemsToPlaceInOrder.push_back(item_13);
	itemsToPlaceInOrder.push_back(item_14);
	itemsToPlaceInOrder.push_back(item_15);
	itemsToPlaceInOrder.push_back(item_16);
	itemsToPlaceInOrder.push_back(item_17);
	itemsToPlaceInOrder.push_back(item_18);
	itemsToPlaceInOrder.push_back(item_19);
	itemsToPlaceInOrder.push_back(item_20);
	itemsToPlaceInOrder.push_back(item_21);
	itemsToPlaceInOrder.push_back(item_22);
	itemsToPlaceInOrder.push_back(item_23);
	itemsToPlaceInOrder.push_back(item_24);
	itemsToPlaceInOrder.push_back(item_25);
	itemsToPlaceInOrder.push_back(item_26);

	#elif EJEMPLO_PEDIDO_M
	itemsToPlaceInOrder.push_back(item_01);
	itemsToPlaceInOrder.push_back(item_02);
	itemsToPlaceInOrder.push_back(item_03);
	itemsToPlaceInOrder.push_back(item_04);
	itemsToPlaceInOrder.push_back(item_05);
	itemsToPlaceInOrder.push_back(item_06);
	itemsToPlaceInOrder.push_back(item_07);
	itemsToPlaceInOrder.push_back(item_08);
	itemsToPlaceInOrder.push_back(item_09);
	itemsToPlaceInOrder.push_back(item_10);
	itemsToPlaceInOrder.push_back(item_11);
	itemsToPlaceInOrder.push_back(item_12);
	itemsToPlaceInOrder.push_back(item_13);
	itemsToPlaceInOrder.push_back(item_14);
	itemsToPlaceInOrder.push_back(item_15);
	itemsToPlaceInOrder.push_back(item_16);
	itemsToPlaceInOrder.push_back(item_17);
	itemsToPlaceInOrder.push_back(item_18);
	itemsToPlaceInOrder.push_back(item_19);
	itemsToPlaceInOrder.push_back(item_20);
	itemsToPlaceInOrder.push_back(item_21);
	itemsToPlaceInOrder.push_back(item_22);
	itemsToPlaceInOrder.push_back(item_23);
	itemsToPlaceInOrder.push_back(item_24);
	itemsToPlaceInOrder.push_back(item_25);
	itemsToPlaceInOrder.push_back(item_26);

	#elif EJEMPLO_PEDIDO_L
	itemsToPlaceInOrder.push_back(item_01);
	itemsToPlaceInOrder.push_back(item_02);
	itemsToPlaceInOrder.push_back(item_03);
	itemsToPlaceInOrder.push_back(item_04);
	itemsToPlaceInOrder.push_back(item_05);
	itemsToPlaceInOrder.push_back(item_06);
	itemsToPlaceInOrder.push_back(item_07);
	itemsToPlaceInOrder.push_back(item_08);
	itemsToPlaceInOrder.push_back(item_09);
	itemsToPlaceInOrder.push_back(item_10);
	itemsToPlaceInOrder.push_back(item_11);
	itemsToPlaceInOrder.push_back(item_12);
	itemsToPlaceInOrder.push_back(item_13);
	itemsToPlaceInOrder.push_back(item_14);
	itemsToPlaceInOrder.push_back(item_15);
	itemsToPlaceInOrder.push_back(item_16);
	itemsToPlaceInOrder.push_back(item_17);
	itemsToPlaceInOrder.push_back(item_18);
	itemsToPlaceInOrder.push_back(item_19);
	itemsToPlaceInOrder.push_back(item_20);
	itemsToPlaceInOrder.push_back(item_21);
	itemsToPlaceInOrder.push_back(item_22);
	itemsToPlaceInOrder.push_back(item_23);
	itemsToPlaceInOrder.push_back(item_24);
	itemsToPlaceInOrder.push_back(item_25);
	itemsToPlaceInOrder.push_back(item_26);
	itemsToPlaceInOrder.push_back(item_27);
	itemsToPlaceInOrder.push_back(item_28);
	itemsToPlaceInOrder.push_back(item_29);
	itemsToPlaceInOrder.push_back(item_30);
	itemsToPlaceInOrder.push_back(item_31);
	itemsToPlaceInOrder.push_back(item_32);
	itemsToPlaceInOrder.push_back(item_33);
	itemsToPlaceInOrder.push_back(item_34);
	itemsToPlaceInOrder.push_back(item_35);
	itemsToPlaceInOrder.push_back(item_36);
	itemsToPlaceInOrder.push_back(item_37);
	itemsToPlaceInOrder.push_back(item_38);
	itemsToPlaceInOrder.push_back(item_39);
	itemsToPlaceInOrder.push_back(item_40);
	itemsToPlaceInOrder.push_back(item_41);
	itemsToPlaceInOrder.push_back(item_42);
	itemsToPlaceInOrder.push_back(item_43);
	itemsToPlaceInOrder.push_back(item_44);
	itemsToPlaceInOrder.push_back(item_45);
	itemsToPlaceInOrder.push_back(item_46);
	itemsToPlaceInOrder.push_back(item_47);
	itemsToPlaceInOrder.push_back(item_48);

	#endif

	box_t box_01(caja_ejemplo, &itemsToPlaceInOrder);

	box_01.place_items_in_box();

	box_01.generate_mqtt_order();

	cout << (box_01.get_mqtt_order());

	cout << endl;

	return 0;

}	/* main() */

/*** end of file ***/
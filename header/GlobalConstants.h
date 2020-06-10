#ifndef GLOBAL_CONSTANTS
#define GLOBAL_CONSTANTS

static struct GlobalConstants {

	const float DEFAULT_ROTATION = 3;		//Der Winkel, der ein Objekt bei einem Tastendruck weiter rotiert
	const float BEZIER_ACCURACY = 0.01f;	//Die Genauigkeit der Kurve, je kleiner der Wert desto genauer

	const float SELECTION_OFFSET = 2;		//Der Abstand bei dem ein Punkt noch immer ausgewählt wird

} globalConstants;
#endif // !GLOBAL_CONSTANTS

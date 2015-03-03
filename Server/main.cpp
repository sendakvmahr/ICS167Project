#include "NTP.h"


void main(){
	NTPServer n;
	n.CalculateOffset();
	cout << "offset: " << n.getOffset() << endl;
	system("pause");
}
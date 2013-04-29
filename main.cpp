#include "datetime.h"

#include <iostream>
#include <time.h>
using namespace std;


int main() {
	cout <<	DateTime::get_current() << endl;
//while (1) {
	DateTime a(DateTime::get_current()), b(2013, 1, 1, 0, 0, 0);
	b = a - a.Diff(b);
	cout << b << endl;

	cout << a.get_week() << a.get_time(" неделя: |w или |W, дата(date): |d.|m|M.|y, год: |Y, время: |h:|i:|s\n");

	cout << "a = " << a << "\nb = " << b << "\n(a == b) = " << (a == b) << "\n(a < b) = " << (a < b) << 
		"\n(a <= b) = " << (a <= b) << "\n(a > b) = " << (a > b) << "\n(a >= b) = " << (a >= b) << endl;

	DateTime c("2050, 01, 02* &Ymd&m&i 03.04.05", "|Y, |m, |d* &Ymd&m&i |h.|i.|s");
	cout << c << endl;
//}

	return 0;
}

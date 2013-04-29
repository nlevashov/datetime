//(done)сделать переменную
//(done)обработать исключения
//(done)экранировать формат

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef time_t TimeSpan;

class DateTime {
	time_t _ut;
	tm * _data;

	public:
		DateTime ();
		DateTime (time_t);
		DateTime (int, int, int, int, int, int);
		DateTime (int, int, int);
		DateTime (std::string);
		DateTime (std::string, std::string);
		DateTime (const DateTime &);
		~DateTime ();

		const DateTime & operator = (const DateTime &);

		void scan(std::istream &);
		void print() const;

		int get_week();				//Определение дня недели (1..7 = mon..sun)
		static const DateTime get_current();	//Статический метод, возвращающий объект с текущим временем.

		const bool operator == (const DateTime &);
		const bool operator <  (const DateTime &);
		const bool operator <= (const DateTime &);
		const bool operator >  (const DateTime &);
		const bool operator >= (const DateTime &);

		std::string get_time(std::string);	//Представление времени в виде строки. Формат представления времени в виде строки задается отдельным параметром

		const TimeSpan Diff(DateTime &) const;	//Разница в датах
		const DateTime operator + (TimeSpan);
		const DateTime operator - (TimeSpan);
};


//-----constructors--------------------------------------------------------------------

DateTime::DateTime () : _ut(0), _data(NULL) {}


DateTime::DateTime (time_t unixTime) : _ut(unixTime)
{
	try {
		_data = new tm;
		*_data = *localtime(&_ut);
	} catch (std::bad_alloc const & err) {
		_ut = 0;
		_data = NULL;
		std::cout << err.what() << std::endl;
	}
}


DateTime::DateTime (int year, int month, int day, int h, int m, int s)
{
	if (! ((year >= 0) && (year <= 9999) && (month >= 1) && (month <= 12) && (day >= 1) && (day <= 31)
		&& (h >= 0) && (h < 24) && (m >= 0) && (m < 60) && (s >= 0) && (s < 60))) {
		_ut = 0;
		_data = NULL;
		std::cout << "Incorrect data" << std::endl;
	} else {
		try {
			_data = new tm;

			_data->tm_sec = s;
			_data->tm_min = m;
			_data->tm_hour = h;
			_data->tm_mday = day;
			_data->tm_mon = month - 1;
			_data->tm_year = year - 1900;

			_ut = mktime(_data);

		} catch (std::bad_alloc const & err) {
			_ut = 0;
			_data = NULL;
			std::cout << err.what() << std::endl;
		}
	}
}


DateTime::DateTime (int year, int month, int day)
{
	if (! ((year >= 0) && (year <= 9999) && (month >= 1) && (month <= 12) && (day >= 1) && (day <= 31))) {
		_ut = 0;
		_data = NULL;
		std::cout << "Incorrect data" << std::endl;
	} else {
		try {
			_data = new tm;

			_data->tm_mday = day;
			_data->tm_mon = month - 1;
			_data->tm_year = year - 1900;
			_data->tm_hour = 0;
			_data->tm_min = 0;
			_data->tm_sec = 0;

			_ut = mktime(_data);

		} catch (std::bad_alloc const & err) {
			_ut = 0;
			_data = NULL;
			std::cout << err.what() << std::endl;
		}
	}
}


DateTime::DateTime (std::string input) //format: "d*m*Y*h*i*s", '*' == any symbol
{
	if (input.length() < 19) {
		_ut = 0;
		_data = NULL;
		std::cout << "Incorrect data" << std::endl;
	} else {
		try {
			_data = new tm;

			_data->tm_mday = (input[0] - '0') * 10 + (input[1] - '0');
			_data->tm_mon = (input[3] - '0') * 10 + (input[4] - '0') - 1;
			_data->tm_year = (input[6] - '0') * 1000 + (input[7] - '0') * 100 + (input[8] - '0') * 10 + (input[9] - '0') - 1900;
			_data->tm_hour = (input[11] - '0') * 10 + (input[12] - '0');
			_data->tm_min = (input[14] - '0') * 10 + (input[15] - '0');
			_data->tm_sec = (input[17] - '0') * 10 + (input[18] - '0');

			if (! ((_data->tm_year >= -1900) && (_data->tm_year <= 8099) && (_data->tm_mon >= 1) && (_data->tm_mon <= 12) &&
				(_data->tm_mday >= 1) && (_data->tm_mday <= 31) && (_data->tm_hour >= 0) && (_data->tm_hour < 24) &&
				(_data->tm_min >= 0) && (_data->tm_min < 60) && (_data->tm_sec >= 0) && (_data->tm_sec < 60))) {
				_ut = 0;
				delete _data;
				_data = NULL;
				std::cout << "Incorrect data" << std::endl;
			} else _ut = mktime(_data);

		} catch (std::bad_alloc const & err) {
			_ut = 0;
			_data = NULL;
			std::cout << err.what() << std::endl;
		}
	}
}


DateTime::DateTime (std::string input, std::string format)
{
	try {
		_data = new tm;

		int j = 0;
		bool flag = false;
		for (int i = 0; i < format.length(); i++) {
			if (format[i] == '|') {
				i++;
				switch (format[i]) {
					case 'Y':
						_data->tm_year = (input[j] - '0') * 1000 + (input[j+1] - '0') * 100 + (input[j+2] - '0') * 10 + (input[j+3] - '0') - 1900;
						if (! ((_data->tm_year >= -1900) && (_data->tm_year <= 8099))) {
							_ut = 0;
							delete _data;
							_data = NULL;
							std::cout << "Incorrect data (year)" << std::endl;
							flag = true;
						}
						j += 4;
						break;
					case 'm':
						_data->tm_mon = (input[j] - '0') * 10 + (input[j+1] - '0') - 1;
						if (! ((_data->tm_mon >= 0) && (_data->tm_mon <= 11))) {
							_ut = 0;
							delete _data;
							_data = NULL;
							std::cout << "Incorrect data (month)" << std::endl;
							flag = true;
						}
						j += 2;
						break;
					case 'd':
						_data->tm_mday = (input[j] - '0') * 10 + (input[j+1] - '0');
						if (! ((_data->tm_mday >= 1) && (_data->tm_mday <= 31))) {
							_ut = 0;
							delete _data;
							_data = NULL;
							std::cout << "Incorrect data (day)" << std::endl;
							flag = true;
						}
						j += 2;
						break;
					case 'h':
						_data->tm_hour = (input[j] - '0') * 10 + (input[j+1] - '0');
						if (! ((_data->tm_hour >= 0) && (_data->tm_hour < 24))) {
							_ut = 0;
							delete _data;
							_data = NULL;
							std::cout << "Incorrect data (hour)" << std::endl;
							flag = true;
						}
						j += 2;
						break;
					case 'i':
						_data->tm_min = (input[j] - '0') * 10 + (input[j+1] - '0');
						if (! ((_data->tm_min >= 0) && (_data->tm_min < 60))) {
							_ut = 0;
							delete _data;
							_data = NULL;
							std::cout << "Incorrect data (minute)" << std::endl;
							flag = true;
						}
						j += 2;
						break;
					case 's':
						_data->tm_sec = (input[j] - '0') * 10 + (input[j+1] - '0');
						if (! ((_data->tm_sec >= 0) && (_data->tm_sec < 60))) {
							_ut = 0;
							delete _data;
							_data = NULL;
							std::cout << "Incorrect data (second)" << std::endl;
							flag = true;
						}
						j += 2;
						break;
				}
			} else j++;

			if (flag) break;
		}

		if (! flag) _ut = mktime(_data);

	} catch (std::bad_alloc const & err) {
		_ut = 0;
		_data = NULL;
		std::cout << err.what() << std::endl;
	}

}


DateTime::DateTime (const DateTime & c) : _ut(c._ut)
{
	if (c._data != NULL) {
		try {
			_data = new tm;
			memcpy(_data, c._data, sizeof(tm));
		} catch (std::bad_alloc const & err) {
			_ut = 0;
			_data = NULL;
			std::cout << err.what() << std::endl;
		}
	} else _data = NULL;
}


DateTime::~DateTime ()
{
	if (_data != NULL) delete _data;
}


const DateTime & DateTime::operator = (const DateTime & c)
{
	_ut = c._ut;

	if (c._data != NULL) {
		try {
			if (_data == NULL) _data = new tm;
			memcpy(_data, c._data, sizeof(tm));
		} catch (std::bad_alloc const & err) {
			_ut = 0;
			_data = NULL;
			std::cout << err.what() << std::endl;
		}
	} else {
		if (_data != NULL) delete _data;
		_data = NULL;
	}

	return *this;
}



//-----input_output--------------------------------------------------------------------

std::istream & operator >> (std::istream & is, DateTime & c)
{
	c.scan(is);
	return is;
}


std::ostream & operator << (std::ostream & os, const DateTime & c)
{
	c.print();
	return os;
}


void DateTime::scan (std::istream & is)
{
	std::string s;
	getline(is, s);

	try {
		DateTime * temp = new DateTime(s);
		*this = *temp;
	} catch (std::bad_alloc const & err) {
		std::cout << err.what() << std::endl;
	}
}


void DateTime::print () const
{
	if (_data == NULL) printf("DateTime wasn't initializating\n");
	else {
		switch (_data->tm_wday) {
			case 0: printf("SUN "); break;
			case 1: printf("MON "); break;
			case 2: printf("TUE "); break;
			case 3: printf("WED "); break;
			case 4: printf("THU "); break;
			case 5: printf("FRI "); break;
			case 6: printf("SAT "); break;
			case 7: printf("SUN "); break;
			default: printf("XXX ");
		}

		printf("%02d.%02d.%04d %02d:%02d:%02d", _data->tm_mday, _data->tm_mon + 1, _data->tm_year + 1900, _data->tm_hour, _data->tm_min, _data->tm_sec);
	}
}


int DateTime::get_week()	//Определение дня недели (1..7 = mon..sun)
{
	return _data->tm_wday;
}


const DateTime DateTime::get_current()	//Статический метод, возвращающий объект с текущим временем.
{
	return DateTime(time(NULL));
}



//-----comparison----------------------------------------------------------------------

const bool DateTime::operator == (const DateTime & c)
{
	return (_ut == c._ut);
}

const bool DateTime::operator < (const DateTime & c)
{
	return (_ut < c._ut);
}

const bool DateTime::operator <= (const DateTime & c)
{
	return (_ut <= c._ut);
}

const bool DateTime::operator > (const DateTime & c)
{
	return (_ut > c._ut);
}

const bool DateTime::operator >= (const DateTime & c)
{
	return (_ut >= c._ut);
}



std::string DateTime::get_time(std::string format)
{
	if (_data == NULL) {
		printf("DateTime wasn't initializating\n");
		return std::string("");
	} else {
		std::string ans;

		int j = 0;
		for (int i = 0; i < format.length(); i++) {
			if (format[i] == '|') {
				i++;
				switch (format[i]) {
					case 'Y':
						ans += (_data->tm_year + 1900) / 1000 + '0';
						ans += ((_data->tm_year + 1900) / 100) % 10 + '0';
						ans += ((_data->tm_year + 1900) / 10) % 10 + '0';
						ans += (_data->tm_year + 1900) % 10 + '0';
						break;
					case 'y':
						ans += ((_data->tm_year + 1900) / 10) % 10 + '0';
						ans += (_data->tm_year + 1900) % 10 + '0';
						break;
					case 'M':
						switch (_data->tm_mon) {
							case 0: ans += 'J'; ans += 'A'; ans += 'N'; break;
							case 1: ans += 'F'; ans += 'E'; ans += 'B'; break;
							case 2: ans += 'M'; ans += 'A'; ans += 'R'; break;
							case 3: ans += 'A'; ans += 'P'; ans += 'R'; break;
							case 4: ans += 'M'; ans += 'A'; ans += 'Y'; break;
							case 5: ans += 'J'; ans += 'U'; ans += 'N'; break;
							case 6: ans += 'J'; ans += 'U'; ans += 'L'; break;
							case 7: ans += 'A'; ans += 'U'; ans += 'G'; break;
							case 8: ans += 'S'; ans += 'E'; ans += 'P'; break;
							case 9: ans += 'O'; ans += 'C'; ans += 'T'; break;
							case 10: ans += 'N'; ans += 'O'; ans += 'V'; break;
							case 11: ans += 'D'; ans += 'E'; ans += 'C'; break;
						}
						break;
					case 'm':
						ans += (_data->tm_mon + 1) / 10 + '0';
						ans += (_data->tm_mon + 1) % 10 + '0';
						break;
					case 'd':
						ans += _data->tm_mday / 10 + '0';
						ans += _data->tm_mday % 10 + '0';
						break;
					case 'W':
						switch (_data->tm_wday) {
							case 1: ans += 'M'; ans += 'O'; ans += 'N'; break;
							case 2: ans += 'T'; ans += 'U'; ans += 'E'; break;
							case 3: ans += 'W'; ans += 'E'; ans += 'D'; break;
							case 4: ans += 'T'; ans += 'H'; ans += 'U'; break;
							case 5: ans += 'F'; ans += 'R'; ans += 'I'; break;
							case 6: ans += 'S'; ans += 'A'; ans += 'T'; break;
							case 7: ans += 'S'; ans += 'U'; ans += 'N'; break;
						}
						break;
					case 'w':
						ans += _data->tm_wday + '0';
						break;
					case 'h':
						ans += _data->tm_hour / 10 + '0';
						ans += _data->tm_hour % 10 + '0';
						break;
					case 'i':
						ans += _data->tm_min / 10 + '0';
						ans += _data->tm_min % 10 + '0';
						break;
					case 's':
						ans += _data->tm_sec / 10 + '0';
						ans += _data->tm_sec % 10 + '0';
						break;
					case '|':
						ans += '|';
						break;
				}

			} else ans += format[i];
		}

		return ans;
	}
}


const TimeSpan DateTime::Diff(DateTime & c) const	//Разница в датах
{
	return (_ut < c._ut ? c._ut - _ut : _ut - c._ut);
}


const DateTime DateTime::operator + (TimeSpan d)
{
	return DateTime(_ut + d);
}


const DateTime DateTime::operator - (TimeSpan d)
{
	return DateTime(_ut - d);
}

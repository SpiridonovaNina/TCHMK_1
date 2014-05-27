#include "bigNumber.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bigNumber::bigNumber()
{
	_size = 1;
	_sign = 0;
	_digits = new unsigned int[1];
	_digits[0] = 0;
}

bigNumber::bigNumber(const char* string)
{// конструктор из строки
	//
	if (!string)
		return;

	int strSize = strlen(string);
	int strSign = 0;
	if (string[0] == '-')
	{
		strSize--;
		strSign = 1;
	}
	char* strCpy = new char[strlen(string) + 1]; 
	strcpy(strCpy, string);

	// проверка входной строки
	char* pStr = strCpy + strSign;
	while (*pStr)
	{
		if (*pStr < '0' || *pStr > '9')
		{
			_size = 0;
			delete[] strCpy;
			_SetSize(1);
			return;
		}
		pStr++;
	}

	bigNumber res;
	// получаем количество разрядов 
	res._SetSize((strSize + strSign + 8) / 9); 

	// разбиваем строку на части по 9 символов
	for (int i = 0; i < (strSize + strSign) / 9; i++)
	{
		pStr -= 9;
		char splStr[10];
		memcpy(splStr, pStr, 9);
		splStr[9] = '\0';
		unsigned int digitI = atol(splStr);
		res[i] = digitI;
	}

	// обработываем оставшиеся символы
	*pStr = 0;
	if (*(strCpy + strSign) != '\0')
	{
		unsigned int lastDigit = atol(strCpy + strSign);
		res[-1] = lastDigit;
	}
	
	delete[] strCpy;
	res._sign = strSign;
	res._DelNeedlessZeros();
	this->_size = 0;
	*this = res;
}

bigNumber::bigNumber(const bigNumber &rhv)
{
	_Copy(rhv);
}

bigNumber::bigNumber(long long int value)
{
	_digits = new unsigned int[3]();
	_size = 0;
	_sign = 0;
	long long int carry = value;
	if (carry < 0)
	{
		_sign = 1;
		carry = -carry;
	}
	do
	{
		_size++;
		_digits[_size - 1] = carry % BASE;
		carry = carry / BASE;
	} while (carry);
	_DelNeedlessZeros();
}

bigNumber::~bigNumber()
{
	if (_size) delete[] _digits;
}


char* bigNumber::GetString()
{// возвращает строку, в которой записано число в 10-ричной системе счисления
	char* strBuffer = new char[_size * 9 + 1 + _sign]();
	char* pString = strBuffer + _size * 9 + _sign; // указатель на текущую позицию для записи числа
	
	for (int i = 0; i < _size; i++)
	{
		// получаем строковое представление очередного разряда
		char splStr[10];
		sprintf(splStr, "%09u", _digits[i]);

		pString -= 9;
		memcpy(pString, splStr, 9);
	}

	// удаление ведущих нулей
	while (*pString == '0' && *(pString + 1))
		pString++;
	
	if (this->_sign)
	{
		pString--;
		*pString = '-';
	}

	char* string = new char[strlen(pString) + 1]();
	strcpy(string, pString);
	delete[] strBuffer;

	return string;
}

bool bigNumber::GetNumberFromFile(const char* filename)
{
	FILE* Text_file = fopen(filename, "r");
	if (!Text_file)
		return false;

	fseek(Text_file, 0, SEEK_END); //fseek() устанавливает указатель текущей позиции файла. SEEK_END     -  конец файла
	int SizeOfFile = ftell(Text_file);// ftell() возвращает текущее значение указателя позиции файла для заданного потока
	fseek(Text_file, 0, SEEK_SET);// SEEK_SET     -  начало файла

	char* string = new char[SizeOfFile + 1]();
	fscanf(Text_file, "%s", string);
	fclose(Text_file);

	bigNumber res(string);
	*this = res;
	delete[] string;
	return true;
}

bool bigNumber::SaveNumberToFile(const char* filename)
{
	FILE* Result_file = fopen(filename, "w");
	if (!Result_file)
		return false;

	char* string = this->GetString();
	int len = strlen(string);
	int res = fwrite(string, sizeof(char), len, Result_file);
	fclose(Result_file);
	delete[] string;
	if (res < len)
		return false;
	return true;
}

bool bigNumber::SaveNumberInBinFile(const char* filename)
{// в бинарный файл первым делом записывается знак.
	FILE* Result_file = fopen(filename, "w+b");
	if (!Result_file)
		return false;

	fwrite(&_sign, sizeof(_sign), 1, Result_file);
	int res = fwrite(_digits, sizeof(unsigned int), _size, Result_file);
	fclose(Result_file);
	if (res < _size)
		return false;
	return true;
}

bool bigNumber::GetNumberFromBinFile(const char* filename)
{// первым делом из бинарного файла считывается знак
	FILE* Bin_file = fopen(filename, "r+b");
	if (!Bin_file)
		return false;

	fseek(Bin_file, 0, SEEK_END);
	int fileSize = ftell(Bin_file);
	fseek(Bin_file, 0, SEEK_SET);

	if (fileSize < sizeof(_sign))
		return false;
	int len = fread(&_sign, sizeof(_sign), 1, Bin_file);
	fileSize -= sizeof(_sign);
	if (_size) delete[] _digits;
	_size = fileSize / sizeof(unsigned int);
	if (!_size)
	{
		_SetSize(1);
		return false;
	}
	_digits = new unsigned int[_size]();
	len = fread(_digits, sizeof(unsigned int), _size, Bin_file);
	fclose(Bin_file);

	return true;
}

bigNumber& bigNumber::operator=(const bigNumber& rhv)
{
	if (this->_digits == rhv._digits)
		return *this;
	if (_size)
		delete[] _digits;
	_Copy(rhv);
	return *this;
}


bigNumber bigNumber::operator+(const bigNumber& right) const
{
	return _Sum_and_Sub(*this, right);
}

bigNumber bigNumber::operator-() const
{// унарный минус
	bigNumber res(*this);
	res._sign = !res._sign;
	return res;
}

bigNumber bigNumber::operator-(const bigNumber& right) const
{
	return bigNumber(*this + (-right));
}

bigNumber bigNumber::operator*(const bigNumber& right) const
{
	return _Multiplication(*this, right);
}

bigNumber bigNumber::operator/(const bigNumber& right) const
{
	bigNumber rem;
	return _Division(*this, right, rem);
}

bigNumber bigNumber::operator%(const bigNumber& right) const
{
	bigNumber rem;
	_Division(*this, right, rem);
	return rem;
}

bigNumber bigNumber::operator^(const bigNumber& right) const
{// возведение *this в степень right
	bigNumber res = 1;
	bigNumber base = *this;
	for (bigNumber i = right; i > (long long int) 0; i = i - 1)
		res = res * base;
	return res;
}



bool bigNumber::operator>(const bigNumber& B)
{
	return this->_Compare(B) > 0;
}

bool bigNumber::operator>=(const bigNumber& B)
{
	return this->_Compare(B) >= 0;
}

bool bigNumber::operator<(const bigNumber& B)
{
	return this->_Compare(B) < 0;
}

bool bigNumber::operator<=(const bigNumber& B)
{
	return this->_Compare(B) <= 0;
}

bool bigNumber::operator==(const bigNumber& B)
{
	return this->_Compare(B) == 0;
}

bool bigNumber::operator!=(const bigNumber& B)
{
	return this->_Compare(B) != 0;
}



std::ostream& operator<<(std::ostream &out, bigNumber A)
{
	char* str = A.GetString();
	out << str;
	delete[] str;
	return out;
}

std::istream& operator>>(std::istream &is, bigNumber &A)
{
	char string[10000];
	is >> string;
	bigNumber res(string);
	A = res;
	return is;
}

void bigNumber::_SetSize(int size)
{	// изменяет размер числа, при этом обнуляя его. 
	if (_size)
		delete[] _digits;
	_size = size;
	_sign = 0;
	_digits = new unsigned int[_size]();
}

unsigned int & bigNumber::operator[](int i)
{
	if (i < 0)
		return _digits[_size + i];
	return this->_digits[i];
}

unsigned int bigNumber::operator[](int i) const
{
	if (i < 0)
		return _digits[_size + i];
	return this->_digits[i];
}

void bigNumber::_Copy(const bigNumber &rhv)
{
	_size = rhv._size;
	if (!_size)
	{
		_digits = new unsigned int[1];
		_digits[0] = 0;
		_sign = 0;
		return;
	}
	_digits = new unsigned int[_size];
	_sign = rhv._sign;
	memcpy(_digits, rhv._digits, _size*sizeof(unsigned int));
	return;
}

void bigNumber::_DelNeedlessZeros()
{
	while ((_size - 1) && _digits && _digits[_size - 1] == 0)
		_size--;
	if (_size == 1 && _digits[0] == 0)
		_sign = 0;
}

long long int bigNumber::_Compare(const bigNumber& B)
{
	// функция возвращает
	// 0 - если числа равны,
	// >0 - если this больше
	// <0 - если this меньше
	int thisSign = 1;
	if(this->_sign == 1)
		thisSign = -1;

	if(this->_sign != B._sign)
		return thisSign;

	if (this->_size > B._size)
		return thisSign;

	if (this->_size < B._size)
		return -thisSign;

	int i = this->_size - 1;

	while( this->_digits[i] == B[i] && i > 0)
	{
		i--;
	}
	return ((long long int) this->_digits[i] - (long long int)B[i])*thisSign;
}

void bigNumber::_ShiftLeft(int s)
{// сдвигает число на s разрядов вправо
	// то есть, по сути, это умножение на BASE^s
	// сдвиг на отрицательное s - деление на BASE^(-s)
	unsigned int* newDig = new unsigned int[_size + s]();
	for (int i = 0; i < _size; i++)
	{
		if (i + s >= 0)
		{
			newDig[i + s] = _digits[i];
		}
	}
	delete[] _digits;
	_digits = newDig;
	_size += s;
	_DelNeedlessZeros();
}

const bigNumber _Sum_and_Sub(const bigNumber& left, const bigNumber& right)
{
	bigNumber A = left, B = right; // в А будет большее по модулю число, в B - меньшее.
	A._sign = 0;
	B._sign = 0;
	if (A > B)
	{
		A._sign = left._sign;
		B._sign = right._sign;
	}
	else
	{
		A = right;
		B = left;
	}

	if (A._sign == B._sign)
	{// если числа одного знака, то просто складываем их и выставляем нужный знак
	
		bigNumber res;
		res._SetSize(A._size + 1);
		
		unsigned int carry = 0;
		// прибавляем число меньшей размерности к числу большей размерности
		for (int i = 0; i < B._size; i++)
		{
			unsigned int tmp = A[i] + B[i] + carry;
			res[i] = tmp % BASE;
			carry = tmp / BASE;
		}
		// добавляем перенос к оставшейся части более длинного числа
		for (int i = B._size; i < A._size; i++)
		{
			unsigned int tmp = A[i] + carry;
			res[i] = tmp % BASE;
			carry = tmp / BASE;
		}
		res[A._size] = carry;
		res._sign = A._sign;
		res._DelNeedlessZeros();
		return res;
	}
	else
	{// отнимаем одно от другого и выставляем нужный знак
		bigNumber res;
		res._SetSize(A._size);

		unsigned int carry = 0;
		for (int i = 0; i < B._size; i++)
		{
			int tmp = A[i] - B[i] - carry;
			carry = 0;
			if (tmp < 0)
			{
				carry = 1;
				tmp += BASE;
			}
			res[i] = tmp;
		}

		for (int i = B._size; i < A._size; i++)
		{
			int tmp = A[i] - carry;
			carry = 0;
			if (tmp < 0)
			{
				carry = 1;
				tmp += BASE;
			}
			res[i] = tmp;
		}
		res._sign = A._sign;
		res._DelNeedlessZeros();
		return res;
	}
}

const bigNumber _Multiplication(const bigNumber& A, const bigNumber& B)
{// простое умножение "столбиком"
	bigNumber res;
	res._SetSize(A._size + B._size);
	unsigned int carry = 0;
	for (int i = 0; i < B._size; i++)
	{
		carry = 0;
		for (int j = 0; j < A._size; j++)
		{
			unsigned long long int tmp = ( unsigned long long int) B[i] * ( unsigned long long int) A[j] + carry + res[i + j];
			carry = tmp / BASE;
			res[i + j] = tmp % BASE;
		}
		res[i + A._size] = carry;
	}

	res._sign = (A._sign != B._sign);
	res._DelNeedlessZeros();
	return res;
}

const bigNumber _Division(const bigNumber& A, const bigNumber& B, bigNumber &remainder)
{// возвращает целую часть от деления, в remainder - остаток
	remainder = A;
	remainder._sign = 0;

	bigNumber divider = B;
	divider._sign = 0;

	if (divider == bigNumber((long long int) 0))
	{
		throw DIV_ON_ZERO;
		return bigNumber(-1);
	}

	if (remainder < divider)
	{
		remainder = A;
		return bigNumber((long long int) 0);
	}

	bigNumber res;
	res._SetSize(A._size - B._size + 1);

	for (int i = A._size - B._size + 1; i; i--)
	{
		long long int qGuessMax = BASE; // для того, чтобы qGuessMin могло быть равно BASE - 1
		long long int qGuessMin = 0;
		long long int qGuess = qGuessMax;

		// цикл - подбор бинарным поиском числа qGuess
		while (qGuessMax - qGuessMin > 1)
		{
			qGuess = (qGuessMax + qGuessMin) / 2;

			// получаем tmp = qGuess * divider * BASE^i;
			bigNumber tmp = divider * qGuess;
			tmp._ShiftLeft(i - 1);	// сдвигает число на (i - 1) разрядов вправо
									// то есть, по сути, это умножение на BASE^(i - 1)
			if (tmp > remainder)
				qGuessMax = qGuess;
			else
				qGuessMin = qGuess;
		}
		bigNumber tmp = divider * qGuessMin;
		tmp._ShiftLeft(i - 1); // умножение на BASE ^ (i - 1)
		remainder = remainder - tmp;
		res[i - 1] = qGuessMin;
	}

	res._sign = (A._sign != B._sign);
	remainder._sign = (A._sign != B._sign);
	remainder._DelNeedlessZeros();
	res._DelNeedlessZeros();

	return res;
}

const bigNumber Pow(const bigNumber& A, const bigNumber& B, bigNumber& modulus)
{// возведение A в степень B по модулю modulus
	if (modulus <= (long long int) 0)
		return A ^ B;

	bigNumber base = A % modulus;
	bigNumber res = 1;
	
	for (bigNumber i = B; i > (long long int) 0; i = i - 1)
		res = (res * base) % modulus;
	
	return res;
}
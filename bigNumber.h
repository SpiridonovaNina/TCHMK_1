#pragma once
#include <iostream>

#define BASE 1000000000
#define DIV_ON_ZERO 1

class bigNumber
{
public:
	bigNumber();
	bigNumber(const char* String);
	bigNumber(const bigNumber& RightHandValue);
	bigNumber(long long int RightHandValue);
	~bigNumber();

	char* GetString();
	char* __str__();
	char* __repr__();
	bool GetNumberFromFile(const char* FileName);
	bool SaveNumberToFile(const char* FileName);
	bool GetNumberFromBinFile(const char* FileName);
	bool SaveNumberInBinFile(const char* FileName);


	bigNumber& operator=(const bigNumber& RightHandValue);

	//R:  для арифметических оперторв нужно использовать bigNumber& для возвращаемого значения
	//    это позволи избежать ненужного копирования
	// исправлено
	bigNumber& operator+(const bigNumber& right) const;
	bigNumber& operator-() const;
	bigNumber& operator-(const bigNumber& right) const;
	bigNumber& operator*(const bigNumber& right) const;
	bigNumber& operator/(const bigNumber& right) const;
	bigNumber& operator%(const bigNumber& right) const;
	bigNumber& operator^(const bigNumber& right) const;


	bool operator>(const bigNumber& B);
	bool operator>=(const bigNumber& B);
	bool operator<(const bigNumber& B);
	bool operator<=(const bigNumber& B);
	bool operator==(const bigNumber& B);
	bool operator!=(const bigNumber& B);


	friend std::ostream& operator<<(std::ostream &out, bigNumber A);
	friend std::istream& operator>>(std::istream &is, bigNumber &A);


private:
	//Т.к оператор new имеет ограничение на максимальный размер массива 0x7fffffff байт поэтому тип у _size - int 
	int _size;
	unsigned int* _digits;
	int _sign;


	void _SetSize(int size);
	unsigned int & operator[](int i);
	unsigned int operator[](int i) const;
	void _Copy(const bigNumber &rhv);
	void _DelNeedlessZeros();
	long long int _Compare(const bigNumber& B);
	void _ShiftLeft(int s);

	// R: Зачем эти функции объявлены как дружественные для класса ?
	// исправлено
	bigNumber& _Sum_and_Sub(const bigNumber& left, const bigNumber& right) const;
	bigNumber& _Multiplication(const bigNumber A, const bigNumber B) const;
	bigNumber& _Division(const bigNumber& A, const bigNumber& B, bigNumber &remainder) const;

};

bigNumber Pow(const bigNumber& A, const bigNumber& B, bigNumber& modulus);
/*
 * Copyright (C) 2010 kilabit.org
 * Author:
 *	- m.shulhan (ms@kilabit.org)
 */

#ifndef _LIBVOS_RECORD_HPP
#define	_LIBVOS_RECORD_HPP	1

#include "Buffer.hpp"

namespace vos {

/**
 * @class		: Record
 * @attr		:
 *	- _next_col	: pointer to the next column.
 *	- _last_col	: pointer to the last column.
 *	- _next_row	: pointer to the next row.
 *	- _last_row	: pointer to the last row.
 * @desc		:
 *	module to handle field data when parsing DSV file.
 */
class Record : public Buffer {
public:
	Record();
	~Record();

	void dump();

	Record* get_column(int n);
	int set_column(int n, Buffer* bfr);
	int set_column_number(int n, const int number);
	int set_column_ulong(int n, const unsigned long number);
	void columns_reset();

	static void ADD_COL(Record** row, Record* col);
	static void ADD_ROW(Record** rows, Record* row);
	static int INIT_ROW(Record** row, int n_col);

	Record* _next_col;
	Record* _last_col;
	Record* _next_row;
	Record* _last_row;
private:
	Record(const Record&);
	void operator=(const Record&);
};

} /* namespace::vos */
#endif

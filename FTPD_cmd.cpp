/*
 * Copyright (C) 2010 kilabit.org
 * Author:
 *	- m.shulhan (ms@kilabit.org)
 */

#include "FTPD_cmd.hpp"

namespace vos {

FTPD_cmd::FTPD_cmd() :
	_code(0)
,	_name()
,	_parm()
,	_callback(NULL)
,	_next(NULL)
,	_last(this)

{}

FTPD_cmd::~FTPD_cmd()
{
	if (_next) {
		delete _next;
	}
	reset();
}

/**
 * @method	: FTPD_cmd::reset
 * @desc	: reset all attribute.
 */
void FTPD_cmd::reset()
{
	_code = 0;
	_name.reset();
	_parm.reset();
	_callback = 0;
}

/**
 * @method	: FTPD_cmd::set
 * @param	:
 *	> cmd	: pointer to FTPD_cmd object.
 * @desc	: set content of this object using data from 'cmd' object.
 */
void FTPD_cmd::set(FTPD_cmd *cmd)
{
	_code = cmd->_code;
	_name.copy(&cmd->_name);
	_parm.copy(&cmd->_parm);
	_callback = cmd->_callback;
}

/**
 * @method	: FTPD_cmd::dump
 * @desc	: Dump content of FTPD_cmd object.
 */
void FTPD_cmd::dump()
{
	printf(	"[vos::FTPD_cmd__] dump:\n"
		"  command   : %s\n"
		"  parameter : %s\n", _name.v(), _parm.v());
}

/**
 * @method		: FTPD_cmd::INIT
 * @param		:
 *	> name		: name of new command.
 * @return		: pointer to function.
 *	< !NULL		: success, pointer to new FTPD_cmd object.
 *	< NULL		: fail.
 * @desc		: Create and initialize a new FTPD_cmd object.
 */
FTPD_cmd* FTPD_cmd::INIT(const int code, const char* name
			, void (*callback)(const void*, const void*))
{
	FTPD_cmd* cmd = new FTPD_cmd();
	if (cmd) {
		cmd->_code	= code;
		cmd->_callback	= callback;
		cmd->_name.copy_raw(name);
	}
	return cmd;
}

/**
 * @method		: FTPD_cmd::ADD
 * @param		:
 *	> cmds		: pointer to list of FTPD_cmd object.
 *	> cmd_new	: pointer to FTPD_cmd object.
 * @desc		: Add new command 'cmd_new' to list of command 'cmds'.
 */
void FTPD_cmd::ADD(FTPD_cmd** cmds, FTPD_cmd* cmd_new)
{
	if (!cmd_new) {
		return;
	}
	if (!(*cmds)) {
		(*cmds) = cmd_new;
	} else {
		(*cmds)->_last->_next = cmd_new;
	}
	(*cmds)->_last = cmd_new;
}

} /* namespace::vos */

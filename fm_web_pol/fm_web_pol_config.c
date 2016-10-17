/*
* Copyright (c) 2001, 2014, Oracle and/or its affiliates. All rights reserved.
 *
 * This material is the confidential property of Oracle Corporation
 * or its licensors and may be used, reproduced, stored or transmitted
 * only in accordance with a valid Oracle license or sublicense agreement.
 *
 */

#ifndef lint
static const char Sccs_id[] = "@(#)$Id: fm_web_pol_config.c /cgbubrm_7.5.0.portalbase/1 2015/11/27 03:59:02 nishahan Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include <pcm.h>
#include <pinlog.h>
#include "cm_fm.h"

#include "/opt/app/brm/7.5/include/pcm_ops.h"

#define FILE_LOGNAME "fm_web_pol_config.c(1)"

#ifdef MSDOS
__declspec(dllexport) void * fm_web_pol_config_func();
#endif


/*******************************************************************
 *******************************************************************/
struct cm_fm_config fm_web_pol_config[] = {
	/* opcode as a u_int, function name (as a string) */
	{ WEB_OP_ACC_CREATE,		"fm_web_acc_creation" , CM_FM_OP_OVERRIDABLE},
	{ WEB_OP_GET_ACC_DETAILS,		"op_accdetails_search_op" ,CM_FM_OP_OVERRIDABLE},
	{ WEB_OP_GET_BILL_DETAILS,		"fm_web_bill_acc", CM_FM_OP_OVERRIDABLE },
	{ CTP_WEB_OP_GET_PLAN_DETAILS,		"op_plandetails_search_op" , CM_FM_OP_OVERRIDABLE},
	{ WEB_OP_BILL_HTML_DETAILS ,		"op_inv_search_op" , CM_FM_OP_OVERRIDABLE},
	{ WEB_OP_PLAN_TRANSITION ,		"op_plantransition_search_op" , CM_FM_OP_OVERRIDABLE},
	{ WEB_OP_PYMT ,				"op_web_cash_payment" , CM_FM_OP_OVERRIDABLE},
	{ WEB_OP_ADJ ,				"op_adjustment_op" , CM_FM_OP_OVERRIDABLE},
	{ WEB_OP_ADJ_HIS ,				"op_adjustment_his_op" , CM_FM_OP_OVERRIDABLE},
	{ WEB_OP_PAYMENT_HIS ,				"op_pymnt_his_op" , CM_FM_OP_OVERRIDABLE},
	{ 0,	(char *)0 }
};

#ifdef MSDOS
void *
fm_web_pol_config_func()
{
  return ((void *) (fm_web_pol_config));
}
#endif


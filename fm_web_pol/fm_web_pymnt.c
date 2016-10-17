#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_pymnt.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 *  *  *  * Contains the service_search_op operation.
 *   *   *   *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "/opt/app/brm/7.5/include/pcm.h"
#include "cm_fm.h"
#include "/opt/app/brm/7.5/include/pcm_ops.h"
#include "/opt/app/brm/7.5/include/pin_pymt.h"
#include "/opt/app/brm/7.5/include/ops/pymt.h"

/*******************************************************************
 *  * Routines contained within.
 *   *******************************************************************/

EXPORT_OP void
op_web_cash_payment(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);

//extern void fm_get_accdetails();

void
op_web_cash_payment(
        cm_nap_connection_t     *connp,
        int32                   opcode,
        int32                   flags,
        pin_flist_t             *i_flistp,
        pin_flist_t             **o_flistpp,
        pin_errbuf_t            *ebufp)
{
        pcm_context_t           *ctxp = connp->dm_ctx;
        pin_flist_t             *r_flistp = NULL;
        pin_errbuf_t            ebuf;
        int64                   database;
        pin_flist_t             *final_flistp=NULL;
        poid_t                  *db = NULL;
	poid_t                  *db1 = NULL;
	poid_t                  *s1_pdp=NULL;
	poid_t                  *acc_pdp = NULL;
	int32                   s_flags=SRCH_DISTINCT;
	char                    s1_template[2*BUFSIZ] = {""};
	pin_flist_t		*pymt_flistp = NULL;
	pin_flist_t		*chrg_flistp = NULL;
	pin_flist_t		*ret_flistp = NULL;
	pin_flist_t		*pmt_fnl_flistp = NULL; 
	pin_flist_t             *pymt_tmp_flistp = NULL;
	pin_flist_t             *search_flistp = NULL;
	pin_flist_t             *res11_flistp = NULL;
	pin_flist_t             *arg11_flistp = NULL;
	pin_flist_t             *b1_flistp = NULL;
	 pin_flist_t             *tmpacc_flistp = NULL;
	u_int                   pay_type = 10011;
	pin_decimal_t           *amountp = NULL;
	



        if (PIN_ERR_IS_ERR(ebufp))
                return;
        PIN_ERR_CLEAR_ERR(ebufp);

        /***********************************************************
	* Insanity check.
       **********************************************************/
        if (opcode != WEB_OP_PYMT   ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "WEB_OP_PYMT   opcode error", ebufp);
                return;
        }

        /***********************************************************
 	* Debug: What we got.
        ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_PYMT  input flist", i_flistp);

	//fm_get_accdetails(ctxp,i_flistp,&final_flistp,ebufp);

	/*if (PIN_ERR_IS_ERR(ebufp))

	{       
		***************************************************
 		*Log something and return nothing.
 		****************************************************

        	PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                       "No info Available error ", ebufp);
            	PIN_FLIST_DESTROY(final_flistp, NULL);
            	*o_flistpp = NULL;

        } */
	//else 
	//{
                /***************************************************
		*Point the real return flist to the right thing.
		****************************************************/
		/**o_flistpp = final_flistp;

        	PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);

		ret_flistp  = PIN_FLIST_COPY(final_flistp, ebufp);

		PIN_FLIST_DESTROY(final_flistp, NULL);
	
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                	"WEB_OP_PYMT  return flist", ret_flistp);*/

		/********************************
 		**  Search For Account Poid *****
		********************************/
		search_flistp = PIN_FLIST_CREATE(ebufp);

        	db1= (poid_t *)PIN_FLIST_FLD_GET(i_flistp, PIN_FLD_POID,
                                        0, ebufp);
        	s1_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db1), "/search", -1, ebufp);
        	PIN_FLIST_FLD_PUT(search_flistp, PIN_FLD_POID, (void *)s1_pdp, ebufp);
        	PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s1_template, "select X from /account where F1 = V1");
        	PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_TEMPLATE, (void *)s1_template,ebufp);

        	res11_flistp = PIN_FLIST_ELEM_ADD (search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
        	arg11_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,1, ebufp);
        	PIN_FLIST_FLD_SET(arg11_flistp, PIN_FLD_ACCOUNT_NO, PIN_FLIST_FLD_GET(i_flistp,
                                        PIN_FLD_ACCOUNT_NO, 0, ebufp)  , ebufp);

        	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                	"accounts input search flist ",
                		search_flistp);
                PCM_OP(ctxp, PCM_OP_SEARCH, 0,search_flistp , &b1_flistp, ebufp);
        	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                	"accounts  output flist",
                		b1_flistp);

         	tmpacc_flistp = PIN_FLIST_ELEM_GET(b1_flistp, PIN_FLD_RESULTS,
                                     0, 1, ebufp);
		acc_pdp =  PIN_FLIST_FLD_GET(tmpacc_flistp,
                                        PIN_FLD_POID, 0, ebufp);
		/***********************************
               **  Prepare Payment input flist *****
               ************************************/

		pymt_flistp = PIN_FLIST_CREATE(ebufp);

		amountp = pbo_decimal_from_str(PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_NAME, 1, ebufp),ebufp);

		PIN_FLIST_FLD_PUT(pymt_flistp,PIN_FLD_POID,PIN_FLIST_FLD_GET(i_flistp,
                	PIN_FLD_POID, 0,ebufp),ebufp);

		PIN_FLIST_FLD_SET(pymt_flistp ,PIN_FLD_PROGRAM_NAME,"Payment Tool",ebufp);

		chrg_flistp =  PIN_FLIST_ELEM_ADD (pymt_flistp,PIN_FLD_CHARGES,0, ebufp);

		PIN_FLIST_FLD_SET(chrg_flistp,PIN_FLD_ACCOUNT_OBJ,(void *)acc_pdp,ebufp);

		PIN_FLIST_FLD_SET(chrg_flistp,PIN_FLD_AMOUNT,amountp,ebufp);

		PIN_FLIST_FLD_SET(chrg_flistp,PIN_FLD_COMMAND,PIN_CHARGE_CMD_NONE,ebufp);

		PIN_FLIST_FLD_SET(chrg_flistp,PIN_FLD_PAY_TYPE,(void *)&pay_type,ebufp);
	
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                	"WEB_OP_PYMT payment input flist", pymt_flistp);

		pmt_fnl_flistp = PIN_FLIST_CREATE(ebufp);

		//amountp = (pin_decimal_t *)PIN_FLIST_FLD_GET(i_flistp,
                  //              PIN_FLD_AMOUNT, 1, ebufp);
		if (!pin_decimal_is_NULL(amountp, ebufp)) {
                        if (pin_decimal_signum( amountp, ebufp) <= 0) {
                               	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Insufficient balance for payment. Amount is less than 0");
				PIN_FLIST_FLD_SET(pmt_fnl_flistp,PIN_FLD_NAME,"Please Enter the amount greater than 0",ebufp);
				*o_flistpp = pmt_fnl_flistp;
				PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);
                        }
			else
			{
				PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Amount is greater than 0");
				PCM_OP(ctxp, PCM_OP_PYMT_COLLECT, 0, pymt_flistp, &r_flistp, ebufp);
                                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                                        "WEB_OP_PYMT payment :output flist",
                                                r_flistp);
				if( PIN_ERR_IS_ERR( ebufp )) {
                                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_DEBUG,
                                        "WEB_OP_PYMT: PCM_OP_PYMT_COLLECT error", ebufp);
                		}
				pymt_tmp_flistp =PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS,0, 1, ebufp);
				if(!pymt_tmp_flistp)
				{
					PIN_FLIST_FLD_SET(pmt_fnl_flistp,PIN_FLD_NAME,"Failed", ebufp);
					*o_flistpp = pmt_fnl_flistp;
					PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);
				}
				else
				{
					PIN_FLIST_FLD_SET(pmt_fnl_flistp,PIN_FLD_NAME,"Success", ebufp);
					*o_flistpp = pmt_fnl_flistp;
					PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);
				}
			}
                }

		//*o_flistpp = pymt_flistp;

		/***************************************************
 		** Debug: What we're sending back.
       		***************************************************/
       		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               		"WEB_OP_PYMT output", *o_flistpp);
	//}

}

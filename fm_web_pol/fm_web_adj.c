#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_acc_details.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 *  *  * Contains the service_search_op operation.
 *   *   *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "/opt/app/brm/7.5/include/pcm.h"
#include "/opt/app/brm/7.5/include/pcm_ops.h"
#include "/opt/app/brm/7.5/include/ops/bill.h"
#include "cm_fm.h"



/*******************************************************************
 *  *  * Routines contained within.
 *   *   *******************************************************************/
EXPORT_OP void
op_adjustment_op(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);
void
fm_acc_poid(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
        //int64                   database ;
        pin_errbuf_t            *ebufp );
void
fm_make_adjustment(
        pcm_context_t           *ctxp,
        pin_flist_t             *flist_flistp,
	pin_flist_t             *i_flistp,
        pin_flist_t             **final_flistp,
        //int64                   database ;
      pin_errbuf_t            *ebufp );

EXPORT_OP void
op_adjustment_op(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp)


{
 pcm_context_t           *ctxp = connp->dm_ctx;
	pin_flist_t             *flist_flistp=NULL;
	pin_flist_t             *final_flistp=NULL;


        if (PIN_ERR_IS_ERR(ebufp))
                return;
        PIN_ERR_CLEAR_ERR(ebufp);

        /***********************************************************
 *  *      * Insanity check.
 *   *      ***********************************************************/
        if (opcode != WEB_OP_ADJ  ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "WEB_OP_ADJ  opcode error", ebufp);
                return;
        }

        /***********************************************************
 *  *          * Debug: What we got.
 *           ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_ADJ input flist", i_flistp);

	fm_acc_poid(ctxp,i_flistp,&flist_flistp,ebufp);
	fm_make_adjustment (ctxp,flist_flistp,i_flistp,&final_flistp,ebufp);
 /***********************************************************
 *  *          * Results.
 *   ************************************************************/
       if (PIN_ERR_IS_ERR(ebufp))

{                /***************************************************
 *                  * Log something and return nothing.
 *                   *                ***************************************************/

        PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                       "No info Available error ", ebufp);
            PIN_FLIST_DESTROY(final_flistp, NULL);
            *o_flistpp = NULL;

        } else {

                /***************************************************
 *  *               * Point the real return flist to the right thing.
 *   *               ***************************************************/


                        *o_flistpp = final_flistp;
        		PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);

                 /***************************************************
 *  *                * Debug: What we're sending back.
 *   *                ***************************************************/

        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "WEB_OP_ADJ  output", *o_flistpp);

}

}
void
fm_acc_poid(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
        //int64                   database ;
      pin_errbuf_t            *ebufp 	)

{        pin_flist_t             *user2_flistp=NULL;
        poid_t                  *db1=NULL;
        poid_t                  *s1_pdp=NULL;
        char                    s1_template[2*BUFSIZ] = {""};
        pin_flist_t             *res11_flistp=NULL;
        pin_flist_t             *arg11_flistp=NULL;
        pin_flist_t             *b1_flistp=NULL;
        pin_flist_t             *tmpacc_flistp=NULL;
	pin_flist_t             *acc_poid=NULL;
	int32                   s_flags = SRCH_DISTINCT;


        user2_flistp = PIN_FLIST_CREATE(ebufp);
        acc_poid = PIN_FLIST_CREATE(ebufp);

        db1= (poid_t *)PIN_FLIST_FLD_GET(input_flistp, PIN_FLD_POID,
                                        0, ebufp);
        s1_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db1), "/search", -1, ebufp);
        PIN_FLIST_FLD_PUT(user2_flistp, PIN_FLD_POID, (void *)s1_pdp, ebufp);
        PIN_FLIST_FLD_SET(user2_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s1_template, "select X from /account where F1 = V1");
        PIN_FLIST_FLD_SET(user2_flistp, PIN_FLD_TEMPLATE, (void *)s1_template,ebufp);

        res11_flistp = PIN_FLIST_ELEM_ADD (user2_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
        arg11_flistp = PIN_FLIST_ELEM_ADD (user2_flistp,PIN_FLD_ARGS,1, ebufp);
        PIN_FLIST_FLD_SET(arg11_flistp, PIN_FLD_ACCOUNT_NO, PIN_FLIST_FLD_GET(input_flistp,
                                        PIN_FLD_ACCOUNT_NO, 0, ebufp)  , ebufp);

        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "accounts input search flist ",
                user2_flistp);
                PCM_OP(ctxp, PCM_OP_SEARCH, 0,user2_flistp , &b1_flistp, ebufp);
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "accounts  output flist",
                b1_flistp);

         tmpacc_flistp = PIN_FLIST_ELEM_GET(b1_flistp, PIN_FLD_RESULTS,
                                     0, 0, ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "accounts input search flist ",
               tmpacc_flistp);

	 PIN_FLIST_FLD_SET(acc_poid, PIN_FLD_POID, PIN_FLIST_FLD_GET(tmpacc_flistp,
                                        PIN_FLD_POID, 0, ebufp),ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "accounts input flisdt ",
               acc_poid);
	*flist_flistp=acc_poid;

}

void
fm_make_adjustment(
        pcm_context_t           *ctxp,
        pin_flist_t             *flist_flistp,
	pin_flist_t             *i_flistp,
        pin_flist_t             **final_flistp,
        //int64                   database ;
      pin_errbuf_t            *ebufp    )
{

	poid_t                  *acc_pdp = NULL;
	pin_flist_t             *adj_o_flistp = NULL;
	pin_flist_t             *tmp1_flistp = NULL;
	pin_flist_t             *b_flistp = NULL;
	pin_flist_t             *adj_final_flistp = NULL;
	pin_flist_t             *adj_res_flistp = NULL;
	pin_flist_t             *adj_i_flistp = NULL;
	pin_decimal_t           *amountp = NULL;
	

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_make_adjustment input flist:",
                	flist_flistp);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_make_adjustment input flist2:",
                        i_flistp);
	adj_i_flistp = PIN_FLIST_CREATE(ebufp);
	amountp = pbo_decimal_from_str(PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_NAME, 1, ebufp),ebufp);
	acc_pdp = PIN_FLIST_FLD_GET(flist_flistp,PIN_FLD_POID,0, ebufp);
	PIN_FLIST_FLD_SET(adj_i_flistp, PIN_FLD_POID,(void *)acc_pdp, ebufp);
	PIN_FLIST_FLD_SET(adj_i_flistp,PIN_FLD_AMOUNT,amountp,ebufp);
	PIN_FLIST_FLD_SET(adj_i_flistp, PIN_FLD_PROGRAM_NAME,"Customer Center",ebufp);

	PIN_FLIST_FLD_SET(adj_i_flistp, PIN_FLD_DESCR, PIN_FLIST_FLD_GET(i_flistp,
                                        PIN_FLD_DESCR, 0, ebufp)  , ebufp);
	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "adj_i_flistp:",
                        adj_i_flistp);

		
	adj_final_flistp = PIN_FLIST_CREATE(ebufp);
//	amountp = (pin_decimal_t *)PIN_FLIST_FLD_GET(adj_i_flistp,
//                                PIN_FLD_AMOUNT, 1, ebufp);
                if (!pin_decimal_is_NULL(amountp, ebufp)) {
                        if (pin_decimal_signum( amountp, ebufp) == 0) {
                                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Insufficient balance for payment. Amount is less than 0");
                                PIN_FLIST_FLD_SET(adj_final_flistp,PIN_FLD_NAME,"Please Enter the amount greater than 0",ebufp);
                                *final_flistp = adj_final_flistp;
				//PIN_FLIST_DESTROY_EX(&adj_final_flistp, NULL);
				//PIN_FLIST_DESTROY_EX(&adj_i_flistp, NULL);
                        }
		
		else
		{
			PCM_OP(ctxp, PCM_OP_AR_ACCOUNT_ADJUSTMENT, 0, adj_i_flistp, &adj_o_flistp, ebufp);
			PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,"inside else");
			adj_res_flistp = PIN_FLIST_ELEM_GET(adj_o_flistp, PIN_FLD_RESULTS,
                                     0, 0, ebufp);
			
			if(adj_res_flistp)
                	{
                        	PIN_FLIST_FLD_SET(adj_final_flistp, PIN_FLD_NAME, "Success",ebufp);
                	}
			
                	*final_flistp = adj_final_flistp;
			//PIN_FLIST_DESTROY_EX(&adj_final_flistp, NULL);
                        //PIN_FLIST_DESTROY_EX(&adj_i_flistp, NULL);
}		
}
}

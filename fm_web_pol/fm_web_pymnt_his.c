#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_acc_details.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 *  *  *  * Contains the service_search_op operation.
 *   *   *   *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "/opt/app/brm/7.5/include/pcm.h"
#include "/opt/app/brm/7.5/include/pcm_ops.h"
#include "/opt/app/brm/7.5/include/ops/bill.h"
#include "cm_fm.h"



/*******************************************************************
 *  *  *  * Routines contained within.
 *   *   *   *******************************************************************/
EXPORT_OP void
op_pymnt_his_op(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);

void
fm_his_pymnt(
        pcm_context_t           *ctxp,
        pin_flist_t             *i_flistp,
        pin_flist_t             **final_flistp,
        //int64                   database ;
      pin_errbuf_t            *ebufp );

extern void fm_acc_poid();
EXPORT_OP void
op_pymnt_his_op(
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
 *  *  *      * Insanity check.
 *   *   *      ***********************************************************/
        if (opcode != WEB_OP_PAYMENT_HIS  ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "WEB_OP_PAYMENT_HIS  opcode error", ebufp);
                return;
        }

 PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_PAYMENT_HIS input flist", i_flistp);

	fm_his_pymnt (ctxp,i_flistp,&final_flistp,ebufp);	
 /***********************************************************
 *  *  *          * Results.
 *   *   ************************************************************/
       if (PIN_ERR_IS_ERR(ebufp))

{                /***************************************************
 *                  * Log something and return nothing.
 *                   *                   *                ***************************************************/

        PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                       "No info Available error ", ebufp);
            PIN_FLIST_DESTROY(final_flistp, NULL);
            *o_flistpp = NULL;

        } else {

                /***************************************************
 *  *  *               * Point the real return flist to the right thing.
 *   *   *               ***************************************************/


                        *o_flistpp = final_flistp;
                        PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);

                 /***************************************************
 *  *  *                * Debug: What we're sending back.
 *   *   *                ***************************************************/

        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "WEB_OP_PAYMENT_HIS  output", *o_flistpp);

}

}
void
fm_his_pymnt(
        pcm_context_t           *ctxp,
        pin_flist_t             *i_flistp,
        pin_flist_t             **final_flistp,
        //int64                   database ;
        pin_errbuf_t            *ebufp    )
        

{
	poid_t                  *db1=NULL;
        poid_t                  *s1_pdp=NULL;
        char                    s1_template[2*BUFSIZ] = {""};
        pin_flist_t             *res11_flistp=NULL;
        pin_flist_t             *arg11_flistp=NULL;
        pin_flist_t             *array_flistp=NULL;
        pin_flist_t             *search_flistp=NULL;
        pin_flist_t             *flist_flistp=NULL;
        pin_flist_t             *acc_flistp=NULL;
        pin_flist_t             *b1_flistp=NULL;
        int32                   s_flags = SRCH_DISTINCT;
	pin_flist_t             *res_out_flistp=NULL;
int32                   elemid_s=0;
pin_cookie_t            cookie_s=NULL;
pin_flist_t             *array1_flistp=NULL;
pin_flist_t             *arg12_flistp=NULL;
pin_flist_t             *f_flistp=NULL;
 void                    *end_t=NULL;
struct  tm *temp_date = NULL;
pin_flist_t             *tmp_flistp=NULL;
pin_flist_t             *res_flistp=NULL;
char        buff1[100];
 char        buff[100];
char        buff3[100];
char buff2[100];
	 pin_decimal_t           *totalp = NULL;

        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_his_pymnt input flist:",
                        i_flistp); 

	fm_acc_poid(ctxp,i_flistp,&flist_flistp,ebufp);
	
	search_flistp = PIN_FLIST_CREATE(ebufp);

	/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 * 0 PIN_FLD_FLAGS                      INT [0] 256
 * 0 PIN_FLD_OP_CORRELATION_ID          STR [0] "1:DIN31005014:UnknownProgramName:0:AWT-EventQueue-0:7:1474457534:0"
 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /event where F1 = V1 and F2 > V2 event_t.poid_type like \'%%/event/billing/adjustment/account%%\' order by F2 "
 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 2, used 2
 * 1     PIN_FLD_END_T               TSTAMP [0] NULL
 * 1     PIN_FLD_BAL_IMPACTS          ARRAY [*] allocated 1, used 1
 * 2         PIN_FLD_AMOUNT         DECIMAL [0] NULL
 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
 * 1     PIN_FLD_ACCOUNT_OBJ           POID [0] 0.0.0.1 /account 187363 9
 * 0 PIN_FLD_ARGS                     ARRAY [2] allocated 1, used 1
 * 1     PIN_FLD_END_T               TSTAMP [0] NULL*/


	 db1= (poid_t *)PIN_FLIST_FLD_GET(i_flistp, PIN_FLD_POID,
                                        0, ebufp);
        s1_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db1), "/search", -1, ebufp);
        PIN_FLIST_FLD_PUT(search_flistp, PIN_FLD_POID, (void *)s1_pdp, ebufp);
        PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s1_template, "select X from /event where F1 = V1 and F2 >V2 and event_t.poid_type like \'%%/event/billing/payment/cash%%\' order by F2 ");
        PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_TEMPLATE, (void *)s1_template,ebufp);
	res11_flistp = PIN_FLIST_ELEM_ADD (search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
	PIN_FLIST_FLD_SET(res11_flistp,PIN_FLD_END_T,NULL,ebufp);
	array_flistp = PIN_FLIST_ELEM_ADD (res11_flistp,PIN_FLD_BAL_IMPACTS,PIN_ELEMID_ANY, ebufp);
	

	PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_AMOUNT,totalp,ebufp);
	arg11_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,1, ebufp);
	PIN_FLIST_FLD_SET(arg11_flistp, PIN_FLD_ACCOUNT_OBJ, PIN_FLIST_FLD_GET(flist_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);

	arg12_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,2, ebufp);
        PIN_FLIST_FLD_SET(arg12_flistp,PIN_FLD_END_T,NULL,ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_his_pymnt: input search flist ",
                search_flistp);
                PCM_OP(ctxp, PCM_OP_SEARCH, 0,search_flistp , &b1_flistp, ebufp);
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_his_pymnt output flist",
                b1_flistp);
	res_flistp = PIN_FLIST_ELEM_GET(b1_flistp, PIN_FLD_RESULTS,0,1,ebufp);

	f_flistp=PIN_FLIST_CREATE(ebufp);
	if(!res_flistp)
	{
			
		array1_flistp = PIN_FLIST_ELEM_ADD (f_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
		PIN_FLIST_FLD_SET(array1_flistp,PIN_FLD_NAME,"NO PAYMNENT HISTORY FOUND",ebufp);
		PIN_FLIST_FLD_SET(array1_flistp,PIN_FLD_AMOUNT,totalp,ebufp);
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "fm_his_pymnt: inside if",
                f_flistp);
	}
	else
	{
	while((res_out_flistp = PIN_FLIST_ELEM_GET_NEXT
                (b1_flistp, PIN_FLD_RESULTS, &elemid_s,1, &cookie_s, ebufp)) != (pin_flist_t *)NULL)
	{
		//	f_flistp=PIN_FLIST_CREATE(ebufp);
	 array1_flistp = PIN_FLIST_ELEM_ADD (f_flistp, PIN_FLD_RESULTS, elemid_s, ebufp);
	
	 end_t= PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_END_T,0,ebufp);
                memset(buff1, '\0', sizeof(buff1));
                temp_date = localtime(end_t);
                strftime( buff1, 128, "%d-%b-%Y", temp_date );
                 PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, buff1);
		snprintf( buff2, sizeof( buff2 ), "%s", buff1 );
		PIN_FLIST_FLD_SET(array1_flistp,PIN_FLD_NAME,buff2,ebufp)
	tmp_flistp = PIN_FLIST_ELEM_GET(res_out_flistp,PIN_FLD_BAL_IMPACTS ,
                                     0, 1, ebufp);

	PIN_FLIST_FLD_SET(array1_flistp, PIN_FLD_AMOUNT, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_AMOUNT, 0, ebufp)  , ebufp);
	
	}
	}
	*final_flistp=f_flistp;
}

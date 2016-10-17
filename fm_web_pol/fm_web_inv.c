#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_inv.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 *  *  * Contains the service_search_op operation.
 *   *   *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "/opt/app/brm/7.5/include/pcm.h"
#include "/opt/app/brm/7.5/include/pcm_ops.h"
#include "ops/inv.h"
#include "cm_fm.h"
#include "pin_errs.h"
#include "pinlog.h"
#include "cm_cache.h"
#include "fm_utils.h"
#include "pin_inv.h"

#define PIN_ERR_NOT_PROPER_FORMAT 93

EXPORT_OP void
op_inv_search_op(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);


static void
fm_get_inv(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
        //int64                   database ;
        pin_errbuf_t            *ebufp );

void
op_inv_search_op(
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
	pin_flist_t		*final_flistp = NULL;



	if (PIN_ERR_IS_ERR(ebufp))
                return;
        PIN_ERR_CLEAR_ERR(ebufp);

        /***********************************************************
 	 * Insanity check.
       ***********************************************************/
        if (opcode != WEB_OP_BILL_HTML_DETAILS  ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "WEB_OP_BILL_HTML_DETAILS  opcode error", ebufp);
                return;
        }

        /***********************************************************
 	     * Debug: What we got.
       ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_BILL_HTML_DETAILS input flist", i_flistp);


/***********************************************************
*** Get the nameinfo
************************************************************/



	final_flistp = PIN_FLIST_CREATE(ebufp);

	fm_get_inv(ctxp,i_flistp,&final_flistp,ebufp);

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "WEB_OP_BILL_HTML_DETAILS  return flist", final_flistp);

 /***********************************************************
           * Results.
 ***********************************************************/
       if (PIN_ERR_IS_ERR(ebufp))

{                /***************************************************
                   * Log something and return nothing.
                 ***************************************************/

        PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                       "No info Available error ", ebufp);
            PIN_FLIST_DESTROY(final_flistp, NULL);
            *o_flistpp = NULL;

        } else {

                /***************************************************
                * Point the real return flist to the right thing.
                ***************************************************/


                        *o_flistpp = final_flistp;



        PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);

/***************************************************
** Debug: What we're sending back.
*****************************************************/

        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "WEB_OP_BILL_HTML_DETAILS  output", *o_flistpp);

}

}

static void
fm_get_inv(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
//      int64                   database ;
        pin_errbuf_t            *ebufp )

{
	pin_flist_t             *bill_flistp = NULL;
	poid_t                  *db = NULL;
	pin_flist_t             *search_flistp = NULL;
	poid_t                  *s_pdp = NULL;
	poid_t                  *t_pdp = NULL;
	int32                   s_flags = SRCH_DISTINCT;
	char                    s_template[2*BUFSIZ] = {""};
	pin_flist_t             *res1_flistp = NULL;
	pin_flist_t             *arg1_flistp = NULL;
	pin_flist_t             *r_flistp = NULL;
	pin_flist_t             *makeinv_flistp = NULL;
	pin_flist_t             *b_flistp = NULL;
	pin_flist_t             *geninv_flistp = NULL;
	pin_flist_t             *arg2_flistp = NULL;
	pin_flist_t             *c_flistp = NULL;
	pin_flist_t             *tmp_flistp = NULL;
	pin_flist_t             *tmp1_flistp= NULL;
 	pin_flist_t             *inv_flistp= NULL;
 	pin_flist_t             *inv1_flistp= NULL;
	pin_buf_t               *pin_bufp = NULL;
 	pin_flist_t             *d_flistp= NULL;
		

		bill_flistp  = PIN_FLIST_COPY(input_flistp, ebufp);

		/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
		 * 0 PIN_FLD_FLAGS                      INT [0] 256
		 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /bill  where F1 = V1 "
		 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
		 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
		 * 1     PIN_FLD_BILL_NO                STR [0] "B1-9"*/

		db= (poid_t *)PIN_FLIST_FLD_GET(bill_flistp, PIN_FLD_POID,
                                        0, ebufp);
                search_flistp = PIN_FLIST_CREATE(ebufp);
		s_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/search", -1, ebufp);
                PIN_FLIST_FLD_PUT(search_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);

                PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s_template, "select X from /bill where F1 = V1");
                PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_TEMPLATE, (void *)s_template,ebufp);

                 res1_flistp = PIN_FLIST_ELEM_ADD (search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
                 arg1_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,1, ebufp);
		 PIN_FLIST_FLD_SET(arg1_flistp, PIN_FLD_BILL_NO, PIN_FLIST_FLD_GET(bill_flistp,
                                        PIN_FLD_NAME, 0, ebufp)  , ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_inv : search input flist",
                search_flistp);

                PCM_OP(ctxp, PCM_OP_SEARCH, 0, search_flistp, &r_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_inv : search output flist",
                r_flistp);


		makeinv_flistp = PIN_FLIST_CREATE(ebufp);	
		tmp_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS,0, 1, ebufp);
		
		inv_flistp = PIN_FLIST_CREATE(ebufp);
		  /*************************************************
                  *** ERROR HANDLING ***
                 *************************************************/
                if (!tmp_flistp)

                {	inv1_flistp =PIN_FLIST_CREATE(ebufp);
                        PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "No result");
	//		t_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/account", -1, ebufp);
	//		PIN_FLIST_FLD_PUT(inv1_flistp, PIN_FLD_POID, (void *)t_pdp, ebufp);
			PIN_FLIST_FLD_SET(inv1_flistp,PIN_FLD_NAME,"BILL NOT FOUND",ebufp);
			PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
              		"BILL NOT FOUND",
               		inv1_flistp);
			
			*flist_flistp =inv1_flistp;	

		
		}

		else {
		PIN_FLIST_FLD_SET(makeinv_flistp, PIN_FLD_POID, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "make invoice : search input flist",
                makeinv_flistp);

                PCM_OP(ctxp,PCM_OP_INV_MAKE_INVOICE , 0,makeinv_flistp, &b_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "make invoice : search output flist",
              	b_flistp);

		geninv_flistp =PIN_FLIST_CREATE(ebufp);

		PIN_FLIST_FLD_SET(geninv_flistp,PIN_FLD_POID, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_ACCOUNT_OBJ, 0, ebufp)  , ebufp);
		arg2_flistp = PIN_FLIST_ELEM_ADD (geninv_flistp,PIN_FLD_ARGS,1, ebufp);
		PIN_FLIST_FLD_SET(arg2_flistp,PIN_FLD_BILL_NO, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_BILL_NO, 0, ebufp)  , ebufp);

		PIN_FLIST_FLD_SET(arg2_flistp,PIN_FLD_BILL_OBJ, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "view invoice : search input flist",
                geninv_flistp);

                PCM_OP(ctxp,PCM_OP_BILL_VIEW_INVOICE , 0,geninv_flistp, &c_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "view invoice : search output flist",
              	c_flistp);
		
		 tmp1_flistp = PIN_FLIST_ELEM_GET(c_flistp, PIN_FLD_RESULTS,0, 0, ebufp);	
			
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "tmp1_flistp",
              	tmp1_flistp);
		
		pin_bufp = (pin_buf_t *)PIN_FLIST_FLD_GET( tmp1_flistp, PIN_FLD_BUFFER,
                                                   0, ebufp );	
		PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG,pin_bufp->data);	
		PIN_FLIST_FLD_SET(inv_flistp, PIN_FLD_NAME,pin_bufp->data, ebufp);	
		
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "buffer input flist",
	         inv_flistp);
		


	*flist_flistp =inv_flistp;}

 PIN_FLIST_DESTROY_EX(&search_flistp, NULL);		
PIN_FLIST_DESTROY_EX(&makeinv_flistp, NULL);
PIN_FLIST_DESTROY_EX(&geninv_flistp, NULL);



}	

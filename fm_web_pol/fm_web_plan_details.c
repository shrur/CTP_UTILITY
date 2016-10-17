#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_plan_details.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 *  *  * Contains the service_search_op operation.
 *   *   *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "/opt/app/brm/7.5/include/pcm.h"
#include "cm_fm.h"
#include "/opt/app/brm/7.5/include/webplan_ops.h"

/*******************************************************************
 * Routines contained within.
 *******************************************************************/

EXPORT_OP void
op_plandetails_search_op(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);

static void
fm_get_plandetails(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
        //int64                   database ;
        pin_errbuf_t            *ebufp );

void
op_plandetails_search_op(
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
	pin_flist_t		*final_flistp=NULL;
	poid_t			*e_pdp=NULL;
	pin_flist_t		*res1_flistp=NULL;
	poid_t                  *db = NULL;




	if (PIN_ERR_IS_ERR(ebufp))
                return;
        PIN_ERR_CLEAR_ERR(ebufp);

        /***********************************************************
      * Insanity check.
     ***********************************************************/
        if (opcode != CTP_WEB_OP_GET_PLAN_DETAILS   ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "CTP_WEB_OP_GET_PLAN_DETAILS   opcode error", ebufp);
                return;
        }

        /***********************************************************
         * Debug: What we got.
       ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "CTP_WEB_OP_GET_PLAN_DETAILS  input flist", i_flistp);


	 fm_get_plandetails(ctxp,i_flistp,&final_flistp,ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "CTP_WEB_OP_GET_PLAN_DETAILS   return flist", final_flistp);

 /***********************************************************
 * Results.
************************************************************/
       if (PIN_ERR_IS_ERR(ebufp))

{        
	 db= (poid_t *)PIN_FLIST_FLD_GET(i_flistp, PIN_FLD_POID,
                                        0, ebufp);
 
	 e_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/plan", -1, ebufp);

	PIN_FLIST_FLD_PUT(final_flistp, PIN_FLD_POID, (void *)e_pdp, ebufp);
	res1_flistp = PIN_FLIST_ELEM_ADD (final_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
	PIN_FLIST_FLD_SET(res1_flistp,PIN_FLD_NAME,"FAILED",ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "o status flist", final_flistp);
                *o_flistpp = final_flistp;

                PIN_FLIST_DESTROY_EX(&final_flistp, NULL);
                PIN_ERR_CLEAR_ERR(ebufp);
                return;


        } else {

                /***************************************************
                 * Point the real return flist to the right thing.
               ***************************************************/


                        *o_flistpp = final_flistp;



        PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);

                 /***************************************************
                * Debug: What we're sending back.
                ***************************************************/
       PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "CTP_WEB_OP_GET_PLAN_DETAILS   output", *o_flistpp);
}}


static void
fm_get_plandetails(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
//      int64                   database ;
	pin_errbuf_t            *ebufp )
        {

	pin_flist_t		*plan_flistp=NULL;
	poid_t			*db=NULL;
	pin_flist_t		*search_flistp=NULL;
	poid_t		    	*s_pdp=NULL;
	char            	s_template[2*BUFSIZ] = {""};
	pin_flist_t		*arg1_flistp=NULL;
	pin_flist_t		*arg2_flistp=NULL;
	pin_flist_t		*arg3_flistp=NULL;
	poid_t		    	*t_pdp=NULL;
	pin_flist_t		*arg4_flistp=NULL;
	poid_t		    	*d_pdp=NULL;
	pin_flist_t		*res_flistp=NULL;
	pin_flist_t		*mem_flistp=NULL;
	pin_flist_t		*r_flistp=NULL;
	pin_flist_t		*tmp_flistp=NULL;
	pin_flist_t		*res_out_flistp=NULL;
	pin_flist_t		*plan1_flistp=NULL;
	pin_flist_t		*b_flistp=NULL;
	pin_flist_t		*array_flistp=NULL;
	int32			elemid_s=0;
	pin_cookie_t		cookie_s=NULL;
	int32                   s_flags = SRCH_DISTINCT;



	/*0 PIN_FLD_POID           POID [0] 0.0.0.1 /search -1 0
	0 PIN_FLD_FLAGS           INT [0] 256
	0 PIN_FLD_TEMPLATE        STR [0] "select X from /group/$1 where F1 = V1 and F2 = V2 and F3 = V3 and F4 = V4"
	0 PIN_FLD_PARAMETERS      STR [0] "plan_list"
	0 PIN_FLD_ARGS          ARRAY [1] allocated 20, used 1
	1     PIN_FLD_NAME            STR [0] "CSR"
	0 PIN_FLD_ARGS          ARRAY [2] allocated 20, used 1
	1     PIN_FLD_TYPE_STR        STR [0] "new"
	0 PIN_FLD_ARGS          ARRAY [3] allocated 20, used 1
	1     PIN_FLD_POID           POID [0] 0.0.0.1 /group/plan_list -1 0
	0 PIN_FLD_ARGS          ARRAY [4] allocated 20, used 1
	1     PIN_FLD_ACCOUNT_OBJ    POID [0] 0.0.0.1 /account 1 0
	0 PIN_FLD_RESULTS       ARRAY [0] allocated 20, used 2
	1   	  PIN_FLD_POID           POID [0] NULL poid pointer
	1     PIN_FLD_MEMBERS       ARRAY [*]     NULL array ptr*/

		plan_flistp = PIN_FLIST_COPY(input_flistp, ebufp);
        	db= (poid_t *)PIN_FLIST_FLD_GET(plan_flistp, PIN_FLD_POID,
                                        0, ebufp);
        	search_flistp = PIN_FLIST_CREATE(ebufp);

		s_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/search", -1, ebufp);
                PIN_FLIST_FLD_PUT(search_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);

                PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s_template, "select X from /group/$1 where F1 = V1 and F2 = V2 and F3 = V3 and F4 = V4");
                PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_TEMPLATE, (void *)s_template,ebufp);
		PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_PARAMETERS,"plan_list",ebufp);
		arg1_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,1, ebufp);
		PIN_FLIST_FLD_SET(arg1_flistp, PIN_FLD_NAME,"CSR",ebufp);
		arg2_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,2, ebufp);
		PIN_FLIST_FLD_SET(arg2_flistp, PIN_FLD_TYPE_STR,"new",ebufp);
		arg3_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,3, ebufp);
		t_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/group/plan_list", -1, ebufp);
		PIN_FLIST_FLD_PUT(arg3_flistp, PIN_FLD_POID, (void *)t_pdp, ebufp);
		arg4_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,4, ebufp);
		d_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/account", 1, ebufp);
		PIN_FLIST_FLD_PUT(arg4_flistp, PIN_FLD_ACCOUNT_OBJ, (void *)d_pdp, ebufp);
		res_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_RESULTS,0, ebufp);
		mem_flistp = PIN_FLIST_ELEM_ADD (res_flistp,PIN_FLD_MEMBERS,PIN_ELEMID_ANY, ebufp);

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_plandetails : search input flist",
                search_flistp);

		PCM_OP(ctxp, PCM_OP_SEARCH, 0, search_flistp, &r_flistp, ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_plandetails : search output flist",
                r_flistp);
		
		 tmp_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS,
                                                 0, 0, ebufp);

		pin_flist_t *return_flistp = NULL;	
        	return_flistp = PIN_FLIST_CREATE(ebufp);
		
		while((res_out_flistp = PIN_FLIST_ELEM_GET_NEXT
                (tmp_flistp, PIN_FLD_MEMBERS, &elemid_s,1, &cookie_s, ebufp)) != (pin_flist_t *)NULL)
		{
		
		plan1_flistp = PIN_FLIST_CREATE(ebufp);
		PIN_FLIST_FLD_SET(plan_flistp,PIN_FLD_POID ,PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_OBJECT ,0, ebufp)  , ebufp);	
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "plan flistp",
                plan_flistp);
		PCM_OP(ctxp, PCM_OP_READ_OBJ, 0, plan_flistp, &b_flistp, ebufp);
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "plan flistp output",
                b_flistp);

		array_flistp = PIN_FLIST_ELEM_ADD (return_flistp, PIN_FLD_RESULTS, elemid_s, ebufp);
		PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_NAME ,PIN_FLIST_FLD_GET(b_flistp,PIN_FLD_NAME ,0, ebufp)  , ebufp);	
	
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "array_flistp",
                array_flistp);
			
}

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,"final  output",return_flistp);
		*flist_flistp=return_flistp;
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,"final  output123",*flist_flistp);

}


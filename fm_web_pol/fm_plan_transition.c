#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_plan_transition.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 *  *  *  * Contains the service_search_op operation.
 *   *   *   *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "/opt/app/brm/7.5/include/pcm.h"
#include "/opt/app/brm/7.5/include/ops/bill.h"
#include "cm_fm.h"

/*******************************************************************
 *  *  *  * Routines contained within.
 *   *   *   *******************************************************************/

EXPORT_OP void
op_plantransition_search_op(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);

static void
fm_get_plantransition(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
        //int64                   database ;
        pin_errbuf_t            *ebufp );

void
op_plantransition_search_op(
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
	 pin_flist_t             *user2_flistp = NULL;
 	poid_t                  *db1 = NULL;
	 poid_t                  *s1_pdp = NULL;
	int32                   s_flags = SRCH_DISTINCT;
	char                    s1_template[2*BUFSIZ] = {""};
	 pin_flist_t             *res11_flistp = NULL;
	 pin_flist_t             *arg11_flistp = NULL;
	 pin_flist_t             *b1_flistp = NULL;
	 pin_flist_t             *tmpacc_flistp = NULL;


        if (PIN_ERR_IS_ERR(ebufp))
                return;
        PIN_ERR_CLEAR_ERR(ebufp);

        /***********************************************************
 *  *      * Insanity check.
 *   *      ***********************************************************/
        if (opcode != WEB_OP_PLAN_TRANSITION  ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "WEB_OP_PLAN_TRANSITION  opcode error", ebufp);
                return;
        }

        /***********************************************************
 *  *          * Debug: What we got.
 *           ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_PLAN_TRANSITION input flist", i_flistp);


        /***********************************************************
 *  *          * Get the nameinfo
 *   *        ***********************************************************/




          final_flistp = PIN_FLIST_CREATE(ebufp);

        /*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
       * 0 PIN_FLD_FLAGS                      INT [0] 512
         * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /config/user_accounts  where F1 = V1 "
    * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
       * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
      * 1     PIN_FLD_ACCOUNTS             ARRAY [1] allocated 1, used 1
     * 2         PIN_FLD_ACCOUNT_NO         STR [0] "0.0.0.1-141968"*/

        user2_flistp = PIN_FLIST_CREATE(ebufp);

        db1= (poid_t *)PIN_FLIST_FLD_GET(i_flistp, PIN_FLD_POID,
                                        0, ebufp);
        s1_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db1), "/search", -1, ebufp);
        PIN_FLIST_FLD_PUT(user2_flistp, PIN_FLD_POID, (void *)s1_pdp, ebufp);
        PIN_FLIST_FLD_SET(user2_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s1_template, "select X from /account where F1 = V1");
        PIN_FLIST_FLD_SET(user2_flistp, PIN_FLD_TEMPLATE, (void *)s1_template,ebufp);

        res11_flistp = PIN_FLIST_ELEM_ADD (user2_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
        arg11_flistp = PIN_FLIST_ELEM_ADD (user2_flistp,PIN_FLD_ARGS,1, ebufp);
        PIN_FLIST_FLD_SET(arg11_flistp, PIN_FLD_ACCOUNT_NO, PIN_FLIST_FLD_GET(i_flistp,
                                        PIN_FLD_ACCOUNT_NO, 0, ebufp)  , ebufp);

        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "accounts input search flist ",
                user2_flistp);
                PCM_OP(ctxp, PCM_OP_SEARCH, 0,user2_flistp , &b1_flistp, ebufp);
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "accounts  output flist",
                b1_flistp);

         tmpacc_flistp = PIN_FLIST_ELEM_GET(b1_flistp, PIN_FLD_RESULTS,
                                     0, 1, ebufp);


        if (!tmpacc_flistp)

                {
                        PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "No result");
                	PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_NAME,"FAILED",ebufp);
        PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_ACCOUNT_NO,"INVALID ACCOUNT", ebufp);
        PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_FROM_NAME,"INVALID ACCOUNT" , ebufp);
        PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_TO_NAME,"INVALID ACCOUNT"  , ebufp);
		}

        else {
			fm_get_plantransition(ctxp,i_flistp,&final_flistp,ebufp);

	}
 PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_PLAN_TRANSITION  return flist", final_flistp);

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
               "WEB_OP_PLAN_TRANSITION  output", *o_flistpp);
 }}
static void
fm_get_plantransition(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
//      int64                   database 
        pin_errbuf_t            *ebufp )
{
	pin_flist_t             *plan_flistp = NULL;
  pin_flist_t             *search_flistp = NULL;
 poid_t                  *db = NULL;
 poid_t                  *s_pdp = NULL;
 int32                   s_flags = SRCH_DISTINCT;
 char                    s_template[2*BUFSIZ] = {""};
  pin_flist_t             *res1_flistp = NULL;
  pin_flist_t             *arg1_flistp = NULL;
  pin_flist_t             *r_flistp = NULL;
  pin_flist_t             *search1_flistp = NULL;
  pin_flist_t             *res2_flistp = NULL;
  pin_flist_t             *arg2_flistp = NULL;
  pin_flist_t             *r1_flistp = NULL;
  pin_flist_t             *tmp_flistp = NULL;
  pin_flist_t             *tmp1_flistp = NULL;
 pin_flist_t             * tmp3_flistp = NULL;
 pin_flist_t             * tmp5_flistp = NULL;
  pin_flist_t             *tmp6_flistp = NULL;
  pin_flist_t             *search2_flistp = NULL;
 char                    s1_template[2*BUFSIZ] = {""};
  pin_flist_t             *res3_flistp = NULL;
  pin_flist_t             *arg3_flistp = NULL;
  pin_flist_t             *r2_flistp = NULL;
  pin_flist_t             *search3_flistp = NULL;
 char                    s2_template[2*BUFSIZ] = {""};
 pin_flist_t             * res4_flistp = NULL;
 pin_flist_t             * arg4_flistp = NULL;
 pin_flist_t             * r3_flistp = NULL;
 pin_flist_t             * tmp2_flistp = NULL;
  pin_flist_t             *tmp4_flistp = NULL;
  pin_flist_t             *tmp8_flistp = NULL;
  pin_flist_t             *search4_flistp = NULL;
  pin_flist_t             *service_array = NULL;
  pin_flist_t             *from_service = NULL;
  pin_flist_t             *to_service = NULL;
  pin_flist_t             *deal_array = NULL;
  pin_flist_t             *r4_flistp = NULL;
  pin_flist_t             *final_flist = NULL;
  pin_flist_t             *final_flist1 = NULL;
int			trans = 1;
	int		packid = 135;
	plan_flistp  = PIN_FLIST_COPY(input_flistp, ebufp);

			
			search_flistp = PIN_FLIST_CREATE(ebufp);
		
		/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 * 0 PIN_FLD_FLAGS                      INT [0] 256
 * 0 PIN_FLD_OP_CORRELATION_ID          STR [0] "1:DIN35003847:UnknownProgramName:0:AWT-EventQueue-0:7:1472724253:0"
 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /plan  where F1 = V1 "
 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
 * 1     PIN_FLD_NAME                   STR [0] "test8"
 */
		 db= (poid_t *)PIN_FLIST_FLD_GET(plan_flistp, PIN_FLD_POID,
                                        0, ebufp);
		s_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/search", -1, ebufp);
                PIN_FLIST_FLD_PUT(search_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);

                PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s_template, "select X from /plan where F1 = V1");
                PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_TEMPLATE, (void *)s_template,ebufp);

                 res1_flistp = PIN_FLIST_ELEM_ADD (search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
                 arg1_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,1, ebufp);
		PIN_FLIST_FLD_SET(arg1_flistp, PIN_FLD_NAME, PIN_FLIST_FLD_GET(plan_flistp,
                                        PIN_FLD_FROM_NAME, 0, ebufp)  , ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "from plan search flist",
                search_flistp);

                PCM_OP(ctxp, PCM_OP_SEARCH, 0, search_flistp, &r_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "from plan search output flist",
                r_flistp);
		
		/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 * 0 PIN_FLD_FLAGS                      INT [0] 256
 * 0 PIN_FLD_OP_CORRELATION_ID          STR [0] "1:DIN35003847:UnknownProgramName:0:AWT-EventQueue-0:7:1472724253:0"
 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /plan  where F1 = V1 "
 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
 * 1     PIN_FLD_NAME                   STR [0] "test9"*/
		
	search1_flistp = PIN_FLIST_CREATE(ebufp);
		PIN_FLIST_FLD_PUT(search1_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
	 PIN_FLIST_FLD_SET(search1_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);	
	PIN_FLIST_FLD_SET(search1_flistp, PIN_FLD_TEMPLATE, (void *)s_template,ebufp);
		res2_flistp = PIN_FLIST_ELEM_ADD (search1_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
                 arg2_flistp = PIN_FLIST_ELEM_ADD (search1_flistp,PIN_FLD_ARGS,1, ebufp);
                PIN_FLIST_FLD_SET(arg2_flistp, PIN_FLD_NAME, PIN_FLIST_FLD_GET(plan_flistp,
                                        PIN_FLD_TO_NAME, 0, ebufp)  , ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "to plan search flist",
                search1_flistp);

                PCM_OP(ctxp, PCM_OP_SEARCH, 0, search1_flistp, &r1_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "to plan search output flist",
                r1_flistp);

		tmp_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS,
                                                 0, 1, ebufp);
		tmp1_flistp = PIN_FLIST_ELEM_GET(tmp_flistp, PIN_FLD_SERVICES,
                                                 0, 1, ebufp);
		tmp3_flistp = PIN_FLIST_ELEM_GET(r1_flistp, PIN_FLD_RESULTS,
                                                 0, 1, ebufp);
		tmp5_flistp = PIN_FLIST_ELEM_GET(tmp3_flistp, PIN_FLD_SERVICES,
                                                 0, 1, ebufp);
		tmp6_flistp = PIN_FLIST_ELEM_GET(tmp5_flistp, PIN_FLD_DEALS,
                                                 0, 1, ebufp);
		/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 * 0 PIN_FLD_FLAGS                      INT [0] 256
 * 0 PIN_FLD_OP_CORRELATION_ID          STR [0] "1:DIN35003847:UnknownProgramName:0:AWT-EventQueue-0:7:1472724202:0"
 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /service  where F1 = V1 "
 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
 * 1     PIN_FLD_SERVICE_ID             STR [0] "/service/telco31616993"*/
		
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "tmp1_flistp",
                tmp1_flistp);
/*
	search2_flistp = PIN_FLIST_CREATE(ebufp);

	PIN_FLIST_FLD_PUT(search2_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
	 PIN_FLIST_FLD_SET(search2_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
		strcpy ( s1_template, "select X from /service where F1 = V1");
                PIN_FLIST_FLD_SET(search2_flistp, PIN_FLD_TEMPLATE, (void *)s1_template,ebufp);
	res3_flistp = PIN_FLIST_ELEM_ADD (search2_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
         arg3_flistp = PIN_FLIST_ELEM_ADD (search2_flistp,PIN_FLD_ARGS,1, ebufp);
                PIN_FLIST_FLD_SET(arg3_flistp, PIN_FLD_SERVICE_ID, PIN_FLIST_FLD_GET(tmp1_flistp,
                                        PIN_FLD_SERVICE_ID, 0, ebufp)  , ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "service search flist",
                search2_flistp);

                PCM_OP(ctxp, PCM_OP_SEARCH, 0, search2_flistp, &r2_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
		"service search output flist",
                r2_flistp) ;*/

/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 * 0 PIN_FLD_FLAGS                      INT [0] 256
 * 0 PIN_FLD_OP_CORRELATION_ID          STR [0] "1:DIN35003847:UnknownProgramName:0:AWT-EventQueue-0:7:1472724253:0"
 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /account  where F1 = V1 "
 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
 * 1     PIN_FLD_ACCOUNT_NO  			STR [0] "0.0.0.1-145354"*/


		search3_flistp = PIN_FLIST_CREATE(ebufp);
	PIN_FLIST_FLD_PUT(search3_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
 PIN_FLIST_FLD_SET(search3_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
	 strcpy ( s2_template, "select X from /account where F1 = V1");
                PIN_FLIST_FLD_SET(search3_flistp, PIN_FLD_TEMPLATE, (void *)s2_template,ebufp);
	 res4_flistp = PIN_FLIST_ELEM_ADD (search3_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
         arg4_flistp = PIN_FLIST_ELEM_ADD (search3_flistp,PIN_FLD_ARGS,1, ebufp);
                PIN_FLIST_FLD_SET(arg4_flistp, PIN_FLD_ACCOUNT_NO, PIN_FLIST_FLD_GET(plan_flistp,
                                        PIN_FLD_ACCOUNT_NO, 0, ebufp)  , ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "account search flist",
                search3_flistp);

                PCM_OP(ctxp, PCM_OP_SEARCH, 0, search3_flistp, &r3_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "account search output flist",
		r3_flistp);
		tmp2_flistp = PIN_FLIST_ELEM_GET(r3_flistp, PIN_FLD_RESULTS,
                                                 0, 1, ebufp);

		
		 search2_flistp = PIN_FLIST_CREATE(ebufp);

        PIN_FLIST_FLD_PUT(search2_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
         PIN_FLIST_FLD_SET(search2_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s1_template, "select X from /service where F1 = V1");
                PIN_FLIST_FLD_SET(search2_flistp, PIN_FLD_TEMPLATE, (void *)s1_template,ebufp);
        res3_flistp = PIN_FLIST_ELEM_ADD (search2_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
         arg3_flistp = PIN_FLIST_ELEM_ADD (search2_flistp,PIN_FLD_ARGS,1, ebufp);
                PIN_FLIST_FLD_SET(arg3_flistp, PIN_FLD_ACCOUNT_OBJ, PIN_FLIST_FLD_GET(tmp2_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "service search flist",
                search2_flistp);

                PCM_OP(ctxp, PCM_OP_SEARCH, 0, search2_flistp, &r2_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "service search output flist",
                r2_flistp);

		tmp4_flistp = PIN_FLIST_ELEM_GET(r2_flistp, PIN_FLD_RESULTS,0, 1, ebufp);
		/*0 PIN_FLD_POID           POID [0] 0.0.0.1 /account 145354 0
 * 0 PIN_FLD_PROGRAM_NAME    STR [0] "testnap"
 * 0 PIN_FLD_FROM_PLAN      POID [0] 0.0.0.1 /plan 155597 0 
 * 0 PIN_FLD_TO_PLAN        POID [0] 0.0.0.1 /plan 162405 0
 * 0 PIN_FLD_TRANSITION_TYPE   ENUM [0] 1
 * 0 PIN_FLD_SERVICES      ARRAY [0] allocated 20, used 2
 * 1     PIN_FLD_FROM_SERVICE SUBSTRUCT [0] allocated 20, used 1
 * 2         PIN_FLD_SERVICE_OBJ    POID [0] 0.0.0.1 /service/telco 146986 0
 * 1     PIN_FLD_TO_SERVICE   SUBSTRUCT [0] allocated 20, used 5
 * 2         PIN_FLD_SERVICE_OBJ    POID [0] 0.0.0.1 /service/telco -1 0
 * 2         PIN_FLD_DEALS         ARRAY [0] allocated 20, used 1
 * 3             PIN_FLD_PACKAGE_ID      INT  [0] 135
 * 3             PIN_FLD_DEAL_OBJ       POID [0] 0.0.0.1 /deal 143897 0 */

	search4_flistp = PIN_FLIST_CREATE(ebufp);
	PIN_FLIST_FLD_SET(search4_flistp, PIN_FLD_POID, PIN_FLIST_FLD_GET(tmp2_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);
	PIN_FLIST_FLD_SET(search4_flistp,PIN_FLD_PROGRAM_NAME,"testnap",ebufp);	
	PIN_FLIST_FLD_SET(search4_flistp, PIN_FLD_FROM_PLAN, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);
	PIN_FLIST_FLD_SET(search4_flistp, PIN_FLD_TO_PLAN, PIN_FLIST_FLD_GET(tmp3_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);
	PIN_FLIST_FLD_SET(search4_flistp, PIN_FLD_TRANSITION_TYPE ,&trans ,ebufp);
	service_array =PIN_FLIST_ELEM_ADD (search4_flistp, PIN_FLD_SERVICES,0, ebufp);
	
	from_service =PIN_FLIST_SUBSTR_ADD (service_array,PIN_FLD_FROM_SERVICE,ebufp);
	PIN_FLIST_FLD_SET(from_service, PIN_FLD_SERVICE_OBJ, PIN_FLIST_FLD_GET(tmp4_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);
	to_service = PIN_FLIST_SUBSTR_ADD (service_array,PIN_FLD_TO_SERVICE,ebufp);
	PIN_FLIST_FLD_SET(to_service, PIN_FLD_SERVICE_OBJ, PIN_FLIST_FLD_GET(tmp5_flistp,
                                        PIN_FLD_SERVICE_OBJ, 0, ebufp)  , ebufp);
	deal_array =PIN_FLIST_ELEM_ADD (to_service,PIN_FLD_DEALS ,0, ebufp);	
	PIN_FLIST_FLD_SET(deal_array,PIN_FLD_PACKAGE_ID,&packid,ebufp);
	PIN_FLIST_FLD_SET(deal_array, PIN_FLD_DEAL_OBJ, PIN_FLIST_FLD_GET(tmp6_flistp,
                                        PIN_FLD_DEAL_OBJ, 0, ebufp)  , ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "plan transtion input flist",
                search4_flistp);

                PCM_OP(ctxp, PCM_OP_BILL_TRANSITION_PLAN, 0, search4_flistp, &r4_flistp, ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "plan transtion output flist",
                r4_flistp);
	tmp8_flistp = PIN_FLIST_ELEM_GET(r4_flistp, PIN_FLD_RESULTS,
                                                 0, 1, ebufp);	

	final_flist =	PIN_FLIST_CREATE(ebufp);
	if (tmp8_flistp)
{
	PIN_FLIST_FLD_SET(final_flist,PIN_FLD_NAME,"SUCCESS",ebufp);
	PIN_FLIST_FLD_SET(final_flist,PIN_FLD_ACCOUNT_NO,PIN_FLIST_FLD_GET(plan_flistp,
                                        PIN_FLD_ACCOUNT_NO, 0, ebufp)  , ebufp);
	PIN_FLIST_FLD_SET(final_flist,PIN_FLD_FROM_NAME,PIN_FLIST_FLD_GET(plan_flistp,
                                        PIN_FLD_FROM_NAME, 0, ebufp)  , ebufp);
	PIN_FLIST_FLD_SET(final_flist,PIN_FLD_TO_NAME,PIN_FLIST_FLD_GET(plan_flistp,
                                        PIN_FLD_TO_NAME, 0, ebufp)  , ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "Final output",
                final_flist);

	*flist_flistp =final_flist;
}
	final_flist1 =   PIN_FLIST_CREATE(ebufp);
if (!tmp8_flistp)
{
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " inside if");

	PIN_FLIST_FLD_SET(final_flist1,PIN_FLD_NAME,"FAILED",ebufp);
	PIN_FLIST_FLD_SET(final_flist1,PIN_FLD_ACCOUNT_NO,PIN_FLIST_FLD_GET(plan_flistp,
	                                        PIN_FLD_ACCOUNT_NO, 0, ebufp)  , ebufp);
	PIN_FLIST_FLD_SET(final_flist1,PIN_FLD_FROM_NAME,"PLAN NOT ALLOWED FOR TRANSITION",ebufp);
	PIN_FLIST_FLD_SET(final_flist1,PIN_FLD_TO_NAME,"PLAN NOT ALLOWED FOR TRANSITION",ebufp);

PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "Final output",
                final_flist1);
	
*flist_flistp =final_flist1;
}
/*PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "Final output",
                final_flist);*/

//*flist_flistp =final_flist;
}

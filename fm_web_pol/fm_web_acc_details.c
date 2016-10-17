#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_acc_details.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 *  * Contains the service_search_op operation.
 *   *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "/opt/app/brm/7.5/include/pcm.h"
#include "cm_fm.h"


#include "/opt/app/brm/7.5/include/web_acc_details_ops.h"

/*******************************************************************
 *  * Routines contained within.
 *   *******************************************************************/
EXPORT_OP void
op_accdetails_search_op(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);

void
fm_get_accdetails(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
        //int64                   database ;
        pin_errbuf_t            *ebufp );
     
/*void
getmonthfromdate(
        time_t  inTime,
        char    **strmonth)
*/
void
getmonthfromdate(
        time_t  inTime)
{
    struct  tm *temp_date = NULL;
    char        buff[100];
    int         month = 0;
    int         year  = 0;
    int         day   = 0;

	char    **strmonth;
    /* Initializing variable. */
    memset(buff, '\0', sizeof(buff));

    temp_date = localtime(&inTime);
    strftime( buff, 128, "%Y/%m/%d", temp_date );
    sscanf(buff,"%d/%d/%d",&year,&month,&day);
    sprintf(*strmonth, "%d-%d", month,year);

    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, *strmonth);
};

 
void
op_accdetails_search_op(
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
        pin_flist_t             *final_flistp;
	pin_flist_t     	*user_flistp=NULL;
	poid_t			*db=NULL;
	poid_t			*s_pdp=NULL;
	int32			s_flags=SRCH_DISTINCT;
	double			amount=0.00;
	pin_decimal_t  		*totalp = (pin_decimal_t *)NULL;
	char			s_template[2*BUFSIZ] = {""};
	pin_flist_t		*res1_flistp=NULL;
	pin_flist_t		*arg1_flistp=NULL;
	pin_flist_t		*arg2_flistp=NULL;
	pin_flist_t		*b_flistp=NULL;
	pin_flist_t		*tmp_flistp=NULL;
	pin_flist_t		*tmp1_flistp=NULL;
	char			*role=NULL;
	pin_flist_t     	*user1_flistp=NULL;
	poid_t			*e_pdp=NULL;
	poid_t			*r_pdp=NULL;
	pin_flist_t             *user2_flistp=NULL;
        poid_t                  *db1=NULL;
        poid_t                  *s1_pdp=NULL;
        char                    s1_template[2*BUFSIZ] = {""};
        pin_flist_t             *res11_flistp=NULL;
        pin_flist_t             *arg11_flistp=NULL;
        pin_flist_t             *b1_flistp=NULL;
        pin_flist_t             *tmpacc_flistp=NULL;

        if (PIN_ERR_IS_ERR(ebufp))
                return;
        PIN_ERR_CLEAR_ERR(ebufp);

        /***********************************************************
 *      * Insanity check.
 *      ***********************************************************/
        if (opcode != WEB_OP_GET_ACC_DETAILS  ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "WEB_OP_GET_ACC_DETAILS  opcode error", ebufp);
                return;
        }

        /***********************************************************
 *          * Debug: What we got.
         ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_GET_ACC_DETAILS input flist", i_flistp);


        /***********************************************************
 *          * Get the nameinfo
 *        ***********************************************************/



		totalp = pin_decimal("0.0", ebufp);

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
                        r_pdp =(poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/account", -1 ,ebufp);
                        PIN_FLIST_FLD_PUT(final_flistp, PIN_FLD_POID, (void *)r_pdp, ebufp);
                        PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_NAME,"FAILED",ebufp);
                        PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_ACCOUNT_NO,"INVALID ACCOUNT",ebufp);
                         PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_BILL_NO,"INVALID ACCOUNT",ebufp);
                         PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_DESCR,"INVALID ACCOUNT",ebufp);
                        PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_SYS_DESCR,"INVALID ACCOUNT",ebufp);
                        PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_AAC_SOURCE,"INVALID ACCOUNT",ebufp);
                         PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_AMOUNT,totalp,ebufp);
                }
		
	else {
	user_flistp = PIN_FLIST_CREATE(ebufp);
	
	db= (poid_t *)PIN_FLIST_FLD_GET(i_flistp, PIN_FLD_POID,
                                        0, ebufp);
	s_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/search", -1, ebufp);
	PIN_FLIST_FLD_PUT(user_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
	PIN_FLIST_FLD_SET(user_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( s_template, "select X from /config/user_accounts where F1 = V1");
	PIN_FLIST_FLD_SET(user_flistp, PIN_FLD_TEMPLATE, (void *)s_template,ebufp);

	res1_flistp = PIN_FLIST_ELEM_ADD (user_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
	arg1_flistp = PIN_FLIST_ELEM_ADD (user_flistp,PIN_FLD_ARGS,1, ebufp);
	arg2_flistp = PIN_FLIST_ELEM_ADD (arg1_flistp,PIN_FLD_ACCOUNTS,1, ebufp);
	PIN_FLIST_FLD_SET(arg2_flistp, PIN_FLD_ACCOUNT_NO, PIN_FLIST_FLD_GET(i_flistp,
                                        PIN_FLD_ACCOUNT_NO, 0, ebufp)  , ebufp);

	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "user accounts input search flist ",
                user_flistp);
                PCM_OP(ctxp, PCM_OP_SEARCH, 0,user_flistp , &b_flistp, ebufp);
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "user accounts  output flist",
                b_flistp);
		
	 tmp_flistp = PIN_FLIST_ELEM_GET(b_flistp, PIN_FLD_RESULTS,
                                     0, 1, ebufp);
		
		/*	else
		{
			PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Account in input flist doesnt present in user_accounts table");
		}f	
	*/
		 PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "tmp_flistp output flist",
                tmp_flistp);
		
		if(tmp_flistp)	
		{
		tmp1_flistp = PIN_FLIST_ELEM_GET(tmp_flistp, PIN_FLD_ACCOUNTS,
                                                 0, 0, ebufp);
                        role = (char *)PIN_FLIST_FLD_GET(tmp1_flistp, PIN_FLD_ACCOUNT_TAG, 1, ebufp);
                        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,"tmp1_flistp", tmp1_flistp);
                        PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg role");
                        PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, role);
		
		}
		if (tmp_flistp && strcmp((char *)role, "ADMIN") == 0)
		//if (strcmp((char *)role, "ADMIN") == 0)
		{

		PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " inside if");	
		
//		user1_flistp = PIN_FLIST_CREATE(ebufp);
		e_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/account", -1, ebufp);
	        PIN_FLIST_FLD_PUT(final_flistp, PIN_FLD_POID, (void *)e_pdp, ebufp);
		PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_NAME,"SUCCESS",ebufp);
		PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_ACCOUNT_NO,"ADMIN",ebufp);
		PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_BILL_NO,"ADMIN",ebufp);
		PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_DESCR,"ADMIN",ebufp);
		PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_SYS_DESCR,"ADMIN",ebufp);
		PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_AAC_SOURCE,"ADMIN",ebufp);
		PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_AMOUNT,totalp,ebufp);
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                " accounts  output flist",
                user1_flistp);		
//		 *o_flistpp = final_flistp;

		}
		else  
		{     	  
			fm_get_accdetails(ctxp,i_flistp,&final_flistp,ebufp);
		}}
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_GET_ACC_DETAILS  return flist", final_flistp);

 /***********************************************************
 *          * Results.
 ************************************************************/
       if (PIN_ERR_IS_ERR(ebufp))

{                /***************************************************
 *                  * Log something and return nothing.
 *                ***************************************************/

   	PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                       "No info Available error ", ebufp);
            PIN_FLIST_DESTROY(final_flistp, NULL);
            *o_flistpp = NULL;

        } else {

                /***************************************************
 *               * Point the real return flist to the right thing.
 *               ***************************************************/


                        *o_flistpp = final_flistp;



        PIN_FLIST_FLD_COPY(i_flistp,PIN_FLD_POID,*o_flistpp,PIN_FLD_POID,ebufp);

                 /***************************************************
 *                * Debug: What we're sending back.
 *                ***************************************************/

        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
               "WEB_OP_GET_ACC_DETAILS  output", *o_flistpp);

}

}

void
fm_get_accdetails(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
//      int64                   database ;
        pin_errbuf_t            *ebufp )
{

	 pin_flist_t             *acc_flistp = NULL;
	 poid_t                  *db = NULL;
	pin_flist_t             *search_flistp = NULL;
	poid_t                  *s_pdp = NULL;
	char                    s_template[2*BUFSIZ] = {""};
	pin_flist_t             *res1_flistp = NULL;
	pin_flist_t             *arg1_flistp = NULL;
	char                    *account_no = NULL;
	poid_t                  *t_pdp = NULL;
	pin_flist_t             *r_flistp = NULL;
	int32                   s_flags = SRCH_DISTINCT;
	 poid_t                  *a_pdp = NULL;
	pin_flist_t             *bill_search_flistp = NULL;
	 pin_flist_t             *res2_flistp = NULL;
        pin_flist_t             *arg2_flistp = NULL;
	char                    d_template[2*BUFSIZ] = {""};
	pin_flist_t             *b_flistp = NULL;
	pin_flist_t             *tmp_flistp = NULL;
	pin_flist_t             *tmp1_flistp = NULL;
	pin_flist_t             *tmp2_flistp = NULL;
	poid_t                  *db1 = NULL;	
	 pin_flist_t             *billinfo_search_flistp = NULL;
	char                    e_template[2*BUFSIZ] = {""};
	 pin_flist_t             *res3_flistp = NULL;
        pin_flist_t             *arg3_flistp = NULL;
	pin_flist_t             *s_flistp = NULL;
	pin_flist_t             *final1_flistp = NULL;
	void			*due_t = NULL;
	void			*end_t =NULL;
	char 			*due2_t =NULL;
	char			*end2_t = NULL;
	 char                    *bill_no =NULL;
	pin_flist_t             *plan_flistp = NULL;
	char                    p_template[2*BUFSIZ] = {""};
	pin_flist_t             *resp1_flistp = NULL;
	pin_flist_t             *argp1_flistp = NULL;
	pin_flist_t             *argp3_flistp = NULL;
	pin_flist_t             *b1_flistp = NULL;
	pin_flist_t             *plan_tmp_flistp = NULL;
	pin_flist_t             *plan1_flistp = NULL;
	char                    p1_template[2*BUFSIZ] = {""};
	pin_flist_t             *b2_flistp = NULL;
	pin_flist_t             *plan_name_flistp = NULL;
	pin_flist_t             *resp2_flistp = NULL;
	pin_flist_t             *argp2_flistp = NULL;
	 pin_flist_t            *nameinfo_flistp = NULL;
   	struct  tm *temp_date = NULL;
    	char        buff[100];
    	char         month[10];
   	int         year  = 0;
    	int         day   = 0;
    	int         status   = 1;
		 
acc_flistp = PIN_FLIST_COPY(input_flistp, ebufp);
		db= (poid_t *)PIN_FLIST_FLD_GET(acc_flistp, PIN_FLD_POID,
                                        0, ebufp);
		search_flistp = PIN_FLIST_CREATE(ebufp);
		
	/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
	 * 0 PIN_FLD_FLAGS                      INT [0] 256
	 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /account  where F1 = V1 "
	 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
	 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
	 * 1     PIN_FLD_ACCOUNT_NO             STR [0] "0.0.0.1-141968" */

		s_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/search", -1, ebufp);
	        PIN_FLIST_FLD_PUT(search_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
		
		PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
	        strcpy ( s_template, "select X from /account where F1 = V1");
		PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_TEMPLATE, (void *)s_template,ebufp);
		
		 res1_flistp = PIN_FLIST_ELEM_ADD (search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
		 arg1_flistp = PIN_FLIST_ELEM_ADD (search_flistp,PIN_FLD_ARGS,1, ebufp);
		account_no = (char *)PIN_FLIST_FLD_GET(acc_flistp, PIN_FLD_ACCOUNT_NO, 1, ebufp);
	        PIN_FLIST_FLD_SET(arg1_flistp, PIN_FLD_ACCOUNT_NO,
                                      (char *)account_no, ebufp);

 		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
	        "fm_get_accdetails : search input flist",
        	search_flistp);

	        PCM_OP(ctxp, PCM_OP_SEARCH, 0, search_flistp, &r_flistp, ebufp);

        	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
	        "fm_get_accdetails : search output flist",
        	r_flistp);
		
		tmp_flistp = PIN_FLIST_ELEM_GET(r_flistp, PIN_FLD_RESULTS,
                                                 0, 0, ebufp);
		
		nameinfo_flistp = PIN_FLIST_ELEM_GET(tmp_flistp, PIN_FLD_NAMEINFO,
                                                 1, 0, ebufp);

		final1_flistp=PIN_FLIST_CREATE(ebufp);

		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_POID,PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);
		
		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_FIRST_NAME,PIN_FLIST_FLD_GET(nameinfo_flistp,
                                        PIN_FLD_FIRST_NAME, 0, ebufp)  , ebufp);

		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_LAST_NAME,PIN_FLIST_FLD_GET(nameinfo_flistp,
                                        PIN_FLD_LAST_NAME, 0, ebufp)  , ebufp);

		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_ADDRESS,PIN_FLIST_FLD_GET(nameinfo_flistp,
                                        PIN_FLD_ADDRESS, 0, ebufp)  , ebufp);		
		//db1= (poid_t *)PIN_FLIST_FLD_GET(tmp_flistp, PIN_FLD_POID,
                  //                      0, ebufp);	
		//a_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db1), "/account", (poid_t *)db1, ebufp);

	/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 		* 0 PIN_FLD_FLAGS                      INT [0] 256
		* 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /bill  where F1 = V1 "
 		* 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
 		* 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
 		* 1     PIN_FLD_ACCOUNT_OBJ           POID [0] 0.0.0.1 /account 141968 0*/

		bill_search_flistp = PIN_FLIST_CREATE(ebufp);
		PIN_FLIST_FLD_PUT(bill_search_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);

		PIN_FLIST_FLD_SET(bill_search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
		strcpy ( d_template, "select X from /bill where F1 = V1");
		PIN_FLIST_FLD_SET(bill_search_flistp, PIN_FLD_TEMPLATE, (void *)d_template,ebufp);

		res2_flistp = PIN_FLIST_ELEM_ADD (bill_search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
		arg2_flistp = PIN_FLIST_ELEM_ADD (bill_search_flistp,PIN_FLD_ARGS,1, ebufp);
//		PIN_FLIST_FLD_PUT(arg2_flistp,PIN_FLD_ACCOUNT_OBJ, (void *)a_pdp, ebufp);
		PIN_FLIST_FLD_SET(arg2_flistp, PIN_FLD_ACCOUNT_OBJ, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);
		
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_accdetails :bill search input flist",
                bill_search_flistp);
		PCM_OP(ctxp, PCM_OP_SEARCH, 0, bill_search_flistp, &b_flistp, ebufp);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_accdetails : bill search output flist",
                b_flistp);

	/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
	 * 0 PIN_FLD_FLAGS                      INT [0] 256
	 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /billinfo  where F1 = V1 "
	 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
	 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
	 * 1     PIN_FLD_ACCOUNT_OBJ           POID [0] 0.0.0.1 /account 141968 0*/	

		billinfo_search_flistp =PIN_FLIST_CREATE(ebufp);
		 PIN_FLIST_FLD_PUT(billinfo_search_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
		strcpy ( e_template, "select X from /billinfo where F1 = V1");
		PIN_FLIST_FLD_SET(billinfo_search_flistp, PIN_FLD_TEMPLATE, (void *)e_template,ebufp);
		 PIN_FLIST_FLD_SET(billinfo_search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);

		
		res3_flistp = PIN_FLIST_ELEM_ADD (billinfo_search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
		arg3_flistp = PIN_FLIST_ELEM_ADD (billinfo_search_flistp,PIN_FLD_ARGS,1, ebufp);
		PIN_FLIST_FLD_SET(arg3_flistp, PIN_FLD_ACCOUNT_OBJ, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);

		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_accdetails :billinfo search input flist",billinfo_search_flistp);
	
		PCM_OP(ctxp, PCM_OP_SEARCH, 0,billinfo_search_flistp,&s_flistp,ebufp);
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_accdetails ::billinfo search output flist",s_flistp);

		//final1_flistp=PIN_FLIST_CREATE(ebufp);
		//PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_POID,PIN_FLIST_FLD_GET(tmp_flistp,
                  //                      PIN_FLD_POID, 0, ebufp)  , ebufp);		
		
		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_NAME,"SUCCESS",ebufp);
		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_ACCOUNT_NO,PIN_FLIST_FLD_GET(acc_flistp,
                                        PIN_FLD_ACCOUNT_NO, 0, ebufp)  , ebufp);

		tmp1_flistp = PIN_FLIST_ELEM_GET(b_flistp, PIN_FLD_RESULTS,
                                                 0, 0, ebufp);	
		//PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_BILL_NO ,PIN_FLIST_FLD_GET(tmp1_flistp,PIN_FLD_BILL_NO ,0, ebufp)  , ebufp);
		bill_no=PIN_FLIST_FLD_GET(tmp1_flistp,PIN_FLD_BILL_NO ,0, ebufp);
                if ((strcmp((char *)bill_no,"") == 0))
                {
                PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_BILL_NO ,"BILL IN PROGRESS",ebufp);
                }
                else
                {
                PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_BILL_NO ,bill_no,ebufp);
                }

		 PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_accdetails : : final flist1",final1_flistp);	
		 PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "temp4879 list1",tmp1_flistp);	
	
		due_t=PIN_FLIST_FLD_GET(tmp1_flistp,PIN_FLD_DUE_T,0,ebufp);
		PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 2");
    		memset(buff, '\0', sizeof(buff));
    		temp_date = localtime(due_t);
    		strftime( buff, 128, "%d-%b-%Y", temp_date );
		
		if ( strcmp((char *)buff, "01-Jan-1970") == 0) {   
    		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_DESCR,"NA",ebufp);
		}	
		else {
		 PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_DESCR,&buff,ebufp);
		}	
             	end_t=PIN_FLIST_FLD_GET(tmp1_flistp,PIN_FLD_END_T,0,ebufp);
    		memset(buff, '\0', sizeof(buff));
    		temp_date = localtime(end_t);
		strftime( buff, 128, "%d-%b-%Y", temp_date );
		PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, buff);
   
		if ( strcmp((char *)buff, "01-Jan-1970") == 0){
               	PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_SYS_DESCR ,"NA",ebufp)
		}
		else {
                 PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_SYS_DESCR,&buff,ebufp);
                }

		/*char buff[20];
		time_t due1_t=time((time_t *)due_t);
		struct tm *info;
		info = localtime(due_t);
		//strftime(buff, 20, "%d-%B-%Y", localtime(due_t));
		strftime(buff, 20, "%d-%B-%Y", info);
		PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 1");
		PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, buff);
		char buff1[20];
		time_t end1_t=time(end_t);
		strftime(buff1, 20, "%d-%B-%Y", localtime(&end1_t));		
		*/
		
	
		tmp2_flistp = PIN_FLIST_ELEM_GET(s_flistp, PIN_FLD_RESULTS,0, 0, ebufp);
                PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_AMOUNT ,PIN_FLIST_FLD_GET(tmp2_flistp,PIN_FLD_PENDING_RECV,0, ebufp)  , ebufp);	

		/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 		* 0 PIN_FLD_FLAGS                      INT [0] 256
		 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /purchased_product  where F1 = V1 and F2 = V2"
		 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
		 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
		 * 1     PIN_FLD_ACCOUNT_OBJ           POID [0] 0.0.0.1 /account 141968
		 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
		 * 1	 1     PIN_FLD_STATUS                ENUM [0] 1 */

		
		plan_flistp = PIN_FLIST_CREATE(ebufp);
                PIN_FLIST_FLD_PUT(plan_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);

                PIN_FLIST_FLD_SET(plan_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( p_template, "select X from /purchased_product where F1 = V1 and F2 = V2");
                PIN_FLIST_FLD_SET(plan_flistp, PIN_FLD_TEMPLATE, (void *)p_template,ebufp);

                resp1_flistp = PIN_FLIST_ELEM_ADD (plan_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
                argp1_flistp = PIN_FLIST_ELEM_ADD (plan_flistp,PIN_FLD_ARGS,1, ebufp);
                PIN_FLIST_FLD_SET(argp1_flistp, PIN_FLD_ACCOUNT_OBJ, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);
		argp3_flistp = PIN_FLIST_ELEM_ADD (plan_flistp,PIN_FLD_ARGS,2, ebufp);
		PIN_FLIST_FLD_SET(argp3_flistp,PIN_FLD_STATUS,&status,ebufp);

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "plan poid search input flist",
                plan_flistp);
                PCM_OP(ctxp, PCM_OP_SEARCH, 0, plan_flistp, &b1_flistp, ebufp);
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "plan poid search output flist",
                b1_flistp);
		

		/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 		* 0 PIN_FLD_FLAGS                      INT [0] 256
		 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /plan  where F1 = V1 "
		 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
		 * 0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
		 * 1     PIN_FLD_POID           POID [0] 0.0.0.1 /plan 133016 */

		plan_tmp_flistp =PIN_FLIST_ELEM_GET(b1_flistp, PIN_FLD_RESULTS,0, 1, ebufp);
		
if(!plan_tmp_flistp)
{
PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_CODE,"NO PLAN AVAILABLE", ebufp);
PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_AAC_SOURCE,"NO PLAN AVAILABLE", ebufp);
}
else
{

		plan1_flistp =PIN_FLIST_CREATE(ebufp);

		PIN_FLIST_FLD_PUT(plan1_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);

                PIN_FLIST_FLD_SET(plan1_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
                strcpy ( p1_template, "select X from /plan where F1 = V1");
                PIN_FLIST_FLD_SET(plan1_flistp, PIN_FLD_TEMPLATE, (void *)p1_template,ebufp);

                resp2_flistp = PIN_FLIST_ELEM_ADD (plan1_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
                argp2_flistp = PIN_FLIST_ELEM_ADD (plan1_flistp,PIN_FLD_ARGS,1, ebufp);
                PIN_FLIST_FLD_SET(argp2_flistp, PIN_FLD_POID, PIN_FLIST_FLD_GET(plan_tmp_flistp,
                                        PIN_FLD_PLAN_OBJ, 0, ebufp)  , ebufp);
	

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "plan name search input flist",
                plan1_flistp);
                PCM_OP(ctxp, PCM_OP_SEARCH, 0, plan1_flistp, &b2_flistp, ebufp);
	        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "plan name search output flist",
                b2_flistp);

		plan_name_flistp =PIN_FLIST_ELEM_GET(b2_flistp, PIN_FLD_RESULTS,0, 1, ebufp);
		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_CODE ,PIN_FLIST_FLD_GET(plan_name_flistp,PIN_FLD_DESCR,0, ebufp)  , ebufp);
		PIN_FLIST_FLD_SET(final1_flistp,PIN_FLD_AAC_SOURCE ,PIN_FLIST_FLD_GET(plan_name_flistp,PIN_FLD_NAME,0, ebufp)  , ebufp);

}
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_accdetails : : final flist2",final1_flistp);
		*flist_flistp=final1_flistp;

}

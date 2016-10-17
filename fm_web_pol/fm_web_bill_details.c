/*******************************************************************
 *  *
 *  * Copyright (c) 1999, 2012, Oracle and/or its affiliates. All rights reserved.
 *   *
 *    *      This material is the confidential property of Oracle Corporation
 *     *      or its licensors and may be used, reproduced, stored or transmitted
 *      *      only in accordance with a valid Oracle license or sublicense agreement.
 *       *
 *        *******************************************************************/

#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_test.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 *  * Contains the WEB_OP_CUST_COMMIT_CUSTOMER operation.
 *   *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "pcm.h"
#include "cm_fm.h"


#include "/opt/app/brm/7.5/include/webbill_ops.h"

/*******************************************************************
 *  * Routines contained within.
 *   *******************************************************************/
EXPORT_OP void
fm_web_bill_acc(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);

static void
fm_get_accdetails(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_flist_t             **flist_flistp,
        //int64                   database ;
               pin_errbuf_t            *ebufp );
/*
getmonthfromdate(
        time_t  inTime,
        char    **strmonth)
*/
/*void
getmonthfromdate(
        time_t  inTime)
{
    struct  tm *temp_date = NULL;
    char        buff[100];
    int         month = 0;
    int         year  = 0;
    int         day   = 0;

        char    **strmonth;
    // Initializing variable.
   memset(buff, '\0', sizeof(buff));

    temp_date = localtime(&inTime);
    strftime( buff, 128, "%Y/%m/%d", temp_date );
    sscanf(buff,"%d/%d/%d",&year,&month,&day);
    sprintf(*strmonth, "%d-%d", month,year);

    PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, *strmonth);
};*/


void
fm_web_bill_acc(
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
	pin_flist_t             *user_flistp=NULL;
        poid_t                  *db=NULL;
        poid_t                  *s_pdp=NULL;
        int32                   s_flags=SRCH_DISTINCT;
        double                  amount=0.00;
        pin_decimal_t           *totalp = (pin_decimal_t *)NULL;
        char                    s_template[2*BUFSIZ] = {""};
        pin_flist_t             *res1_flistp=NULL;
        pin_flist_t             *arg1_flistp=NULL;
        pin_flist_t             *arg2_flistp=NULL;
        pin_flist_t             *b_flistp=NULL;
        pin_flist_t             *tmp_flistp=NULL;
        pin_flist_t             *tmp1_flistp=NULL;
        char                    *role=NULL;
        pin_flist_t             *user1_flistp=NULL;
        poid_t                  *e_pdp=NULL;
        poid_t                  *r_pdp=NULL;
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
       * Insanity check.
       ***********************************************************/
        if (opcode != WEB_OP_GET_BILL_DETAILS  ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "WEB_OP_GET_BILL_DETAILS  opcode error", ebufp);
                return;
        }

        /***********************************************************
          * Debug: What we got.
            ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_GET_BILL_DETAILS input flist", i_flistp);


        /***********************************************************
           * Get the nameinfo
         ***********************************************************/



		totalp = pin_decimal("0.0", ebufp);
          final_flistp = PIN_FLIST_CREATE(ebufp);
          
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
{			 PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " inside if");			

		e_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/account", -1, ebufp);
                PIN_FLIST_FLD_PUT(final_flistp, PIN_FLD_POID, (void *)e_pdp, ebufp);
                PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_NAME,"SUCCESS",ebufp);
                PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_ACCOUNT_NO,"ADMIN",ebufp);
                PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_BILL_NO,"ADMIN",ebufp);
                PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_DESCR,"ADMIN",ebufp);
                PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_SYS_DESCR,"ADMIN",ebufp);
                PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_AMOUNT,totalp,ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                " accounts  output flist",
                user1_flistp);
			
		} 
		
		else
                {

	
		fm_get_accdetails(ctxp,i_flistp,&final_flistp,ebufp);
		}}

                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "WEB_OP_GET_BILL_DETAILS  return flist", final_flistp);

 /***********************************************************
           * Results.
  ************************************************************/
       if (PIN_ERR_IS_ERR(ebufp))

{                /***************************************************
                   * Log something and return nothing.
                 ***************************************************/
	e_pdp = (poid_t *)PIN_POID_CREATE(PIN_POID_GET_DB(db), "/account", -1, ebufp);
	PIN_FLIST_FLD_PUT(final_flistp, PIN_FLD_POID, (void *)e_pdp, ebufp);
          PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_NAME,"FAILED",ebufp);
          PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_ACCOUNT_NO,"",ebufp);
          PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_BILL_NO,"",ebufp);
          PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_DESCR,"",ebufp);
          PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_SYS_DESCR,"",ebufp);
          PIN_FLIST_FLD_SET(final_flistp,PIN_FLD_AMOUNT,"",ebufp);

	 PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "op_accdetails_search_op status flist", final_flistp);
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
               "WEB_OP_GET_BILL_DETAILS  output", *o_flistpp);

}

}

static void
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
         char                 *a_pdp = NULL;
        pin_flist_t             *bill_search_flistp = NULL;
	 pin_flist_t             *res2_flistp = NULL;
        pin_flist_t             *arg2_flistp = NULL;
        char                    d_template[2*BUFSIZ] = {""};
        pin_flist_t             *b_flistp = NULL;
        pin_flist_t             *tmp_flistp = NULL;
        pin_flist_t             *tmp1_flistp = NULL;
        pin_flist_t             *tmp2_flistp = NULL;
	 void                    *due_t = NULL;
        void                    *end_t =NULL;
	void 			*start_t =NULL;
        char                    *due2_t =NULL;
        char                    *end2_t = NULL;
	char                    *start2_t =NULL;
	char			*bill_no =NULL;
	pin_flist_t             *res_out_flistp =NULL;
	int32                   elemid_s = 0;
        pin_cookie_t            cookie_s = NULL;
        int32                   elemid_s1 = 0;
        pin_cookie_t            cookie_s1 = NULL;
	pin_flist_t 		*array_flistp = NULL;
	struct  tm *temp_date = NULL;
	struct  tm *temp_date1 = NULL;
        char        buff[100];
        char        buff1[100];
        char        buff3[100];
        char         month[10];
        int         year  = 0;
        int         day   = 0;

	 
acc_flistp = PIN_FLIST_COPY(input_flistp, ebufp);
                db= (poid_t *)PIN_FLIST_FLD_GET(acc_flistp, PIN_FLD_POID,
                                        0, ebufp);
                search_flistp = PIN_FLIST_CREATE(ebufp);

 /*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 * 0 PIN_FLD_FLAGS                      INT [0] 256
 * 0 PIN_FLD_TEMPLATE                   STR [0] "select X from /account  where F1 = V1 "
 * 0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
*  0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
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

/*0 PIN_FLD_POID                      POID [0] 0.0.0.1 /search -1 0
 *0 PIN_FLD_FLAGS                      INT [0] 256
 *0 PIN_FLD_TEMPLATE                   STR [0] "select X from /bill  where F1 = V1 "
 *0 PIN_FLD_RESULTS                  ARRAY [*] allocated 0, used 0
 *0 PIN_FLD_ARGS                     ARRAY [1] allocated 1, used 1
 *1     PIN_FLD_ACCOUNT_OBJ           POID [0] 0.0.0.1 /account 141968 0 */

 bill_search_flistp = PIN_FLIST_CREATE(ebufp);
 PIN_FLIST_FLD_PUT(bill_search_flistp, PIN_FLD_POID, (void *)s_pdp, ebufp);
PIN_FLIST_FLD_SET(bill_search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
strcpy ( d_template, "select X from /bill where F1 = V1");
PIN_FLIST_FLD_SET(bill_search_flistp, PIN_FLD_TEMPLATE, (void *)d_template,ebufp);
res2_flistp = PIN_FLIST_ELEM_ADD (bill_search_flistp, PIN_FLD_RESULTS, PIN_ELEMID_ANY, ebufp);
arg2_flistp = PIN_FLIST_ELEM_ADD (bill_search_flistp,PIN_FLD_ARGS,1, ebufp);
// a_pdp =(char *)PIN_FLIST_FLD_GET(tmp_flistp,PIN_FLD_POID, 0, ebufp)
PIN_FLIST_FLD_SET(arg2_flistp, PIN_FLD_ACCOUNT_OBJ, PIN_FLIST_FLD_GET(tmp_flistp,
                                        PIN_FLD_POID, 0, ebufp)  , ebufp);

PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_get_accdetails :bill search input flist",
                bill_search_flistp);

                PCM_OP(ctxp, PCM_OP_SEARCH, 0, bill_search_flistp, &b_flistp, ebufp);
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,

                "fm_get_accdetails : bill search output flist",
                b_flistp);

// final1_flistp =PIN_FLIST_CREATE(ebufp);
while((res_out_flistp = PIN_FLIST_ELEM_GET_NEXT
                (b_flistp, PIN_FLD_RESULTS, &elemid_s,1, &cookie_s, ebufp)) != (pin_flist_t *)NULL)
{

 array_flistp = PIN_FLIST_ELEM_ADD (*flist_flistp, PIN_FLD_RESULTS, elemid_s, ebufp);

 // get = (char *) PIN_FLIST_FLD_GET(  array_flistp, PIN_FLD_BILL_NO, 0, ebufp);

// PIN_FLIST_FLD_SET(array_flistp, PIN_FLD_ACCOUNT_NO,get, ebufp);

PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_POID ,PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_POID ,0, ebufp)  , ebufp);
//PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_BILL_NO ,PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_BILL_NO ,0, ebufp)  , ebufp);

		bill_no=PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_BILL_NO ,0, ebufp);
		if ((strcmp((char *)bill_no,"") == 0))
		{
		PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_BILL_NO ,"BILL IN PROGRESS",ebufp);
		}
		else
		{
		PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_BILL_NO ,bill_no,ebufp);
		}
		PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_AMOUNT ,PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_CURRENT_TOTAL ,0, ebufp)  , ebufp);

		due_t=PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_DUE_T,0,ebufp);
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 2");
                memset(buff, '\0', sizeof(buff));
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 3");
                temp_date = localtime(due_t);
                strftime( buff, 128, "%d-%b-%Y", temp_date );

                if ( strcmp((char *)buff, "01-Jan-1970") == 0) {
                PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_DESCR,"NA",ebufp);
                }
                else {
                 PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_DESCR,&buff,ebufp);
                }
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 3.1");
              

	 	start_t=PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_START_T,0,ebufp);
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 4");
                memset(buff3, '\0', sizeof(buff3));
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 5");
                temp_date1 = localtime(start_t);
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 5.1");
                strftime( buff3, 128, "%d-%b-%Y", temp_date1 );
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Printg buffer 7");
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, buff3);

                /*if ( strcmp((char *)buff, "01-Jan-1970") == 0){
                PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_SYS_DESCR ,"NA",ebufp)
                }
                else {
                 PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_SYS_DESCR,&buff,ebufp);
                }*/

		end_t=PIN_FLIST_FLD_GET(res_out_flistp,PIN_FLD_END_T,0,ebufp);
                memset(buff1, '\0', sizeof(buff1));
                temp_date = localtime(end_t);
                strftime( buff1, 128, "%d-%b-%Y", temp_date );
		 PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, buff1);
		//char *buff2 = strcat(buff3, buff1);
		char buff2[100];
		if ( strcmp(buff1, "01-Jan-1970") == 0){
		snprintf( buff2, sizeof( buff2 ), "%s","NA" );
		}
		else {
		snprintf( buff2, sizeof( buff2 ), "%s%s%s", buff3,  "-",  buff1 );
		}
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, buff2);
		 if ( strcmp(buff2, "01-Jan-1970|01-Jan-1970") == 0){
                PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_SYS_DESCR ,"NA",ebufp)
                }
                else {
                PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, "Inside else");
                 PIN_FLIST_FLD_SET(array_flistp,PIN_FLD_SYS_DESCR,buff2,ebufp);
                }


	}

}

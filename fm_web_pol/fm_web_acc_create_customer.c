/*******************************************************************
 *
* Copyright (c) 1999, 2012, Oracle and/or its affiliates. All rights reserved.
 *
 *      This material is the confidential property of Oracle Corporation
 *      or its licensors and may be used, reproduced, stored or transmitted
 *      only in accordance with a valid Oracle license or sublicense agreement.
 *
 *******************************************************************/

#ifndef lint
static char Sccs_id[] = "@(#)$Id: fm_web_act_pol_customer.c /cgbubrm_7.5.0.rwsmod/2 2012/02/14 15:36:59 bmaturi Exp $";

#endif

/*******************************************************************
 * Contains the WEB_OP_CUST_COMMIT_CUSTOMER operation.
 *******************************************************************/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "pcm.h"
#include "cm_fm.h"


#include "/opt/app/brm/7.5/include/web_acc.h"

/*******************************************************************
 * Routines contained within.
 *******************************************************************/
EXPORT_OP void
fm_web_acc_creation(
                cm_nap_connection_t     *connp,
                int32                   opcode,
                int32                   flags,
                pin_flist_t             *i_flistp,
                pin_flist_t             **o_flistpp,
                pin_errbuf_t            *ebufp);

static pin_flist_t *
fm_web_get_prod_details(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_errbuf_t            *ebufp );

static void
fm_web_pol_prep_serv_array(
	pcm_context_t		*ctxp,
	pin_flist_t		*plan_flistp,
	pin_flist_t		*out_flistp,
	pin_errbuf_t		*ebufp);


static void
fm_web_pol_prep_nameinfo_array(
        pcm_context_t           *ctxp,
        pin_flist_t             *i_flistp,
        pin_flist_t             *out_flistp,
        pin_errbuf_t            *ebufp);



void
fm_web_acc_creation(
        cm_nap_connection_t     *connp,
        int32                   opcode,
        int32                   flags,
        pin_flist_t             *i_flistp,
        pin_flist_t             **o_flistpp,
        pin_errbuf_t            *ebufp)
{
	//pcm_context_t           *ctxp = connp->dm_ctx;
     	pcm_context_t		*ctxp = NULL;
	int64			database;
	pin_errbuf_t		ebuf;
	pin_flist_t             *r_flistp = NULL;
	pin_flist_t             *in_cust_comm_flist = NULL;
	pin_flist_t		*out_cust_comm_flist = NULL;
     	pin_flist_t             *plan_flistp = NULL;
	pin_flist_t		*out_status_flistp = NULL;
	pin_flist_t		*ro_flistp = NULL;
	pin_flist_t             *ro_outflistp = NULL;
	pin_flist_t		*name_iflistp = NULL;

        if (PIN_ERR_IS_ERR(ebufp))
                return;
        PIN_ERR_CLEAR_ERR(ebufp);

 /***********************************************************
         * Insanity check.
         ***********************************************************/
        if (opcode != WEB_OP_ACC_CREATE ) {
                pin_set_err(ebufp, PIN_ERRLOC_FM,
                        PIN_ERRCLASS_SYSTEM_DETERMINATE,
                        PIN_ERR_BAD_OPCODE, 0, 0, opcode);
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "fm_web_acc_creation opcode error", ebufp);
                return;
        }

        /***********************************************************
         * Debug: What we got.
         ***********************************************************/
        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_web_acc_creation input flist", i_flistp);

	//PCM_CONNECT(&ctxp, &database, &ebuf);
	PCM_CONTEXT_OPEN(&ctxp, (pin_flist_t *)NULL, ebufp);


    /***********************************************************
         * Get the Deal name & Deal poid from the plan.
         ***********************************************************/
  	plan_flistp = fm_web_get_prod_details(ctxp,i_flistp,ebufp);

        PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_web_acc_creation return flist", plan_flistp);

	in_cust_comm_flist = PIN_FLIST_CREATE(ebufp);

	fm_web_pol_prep_serv_array(ctxp,plan_flistp,in_cust_comm_flist,ebufp);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Test1");

	fm_web_pol_prep_nameinfo_array(ctxp,i_flistp,in_cust_comm_flist,ebufp);
	
      if (PIN_ERR_IS_ERR(ebufp)) {
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "fm_web_pol_prep_nameinfo_array-i55 error", ebufp);
        }

	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Test2");

	PCM_OP(ctxp, PCM_OP_CUST_COMMIT_CUSTOMER, 0, in_cust_comm_flist, &out_cust_comm_flist, ebufp);

      if (PIN_ERR_IS_ERR(ebufp)) {
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "PCM_OP_CUST_COMMIT_CUSTOMER-i55 error", ebufp);
        }

	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,"out_cust_comm_flist", out_cust_comm_flist);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Test3");
	
	out_status_flistp = PIN_FLIST_CREATE(ebufp);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG, " Test4");

	if (PIN_ERR_IS_ERR(ebufp))
	{	
		PIN_FLIST_FLD_PUT(out_status_flistp, PIN_FLD_POID,PIN_POID_CREATE(1, "/account", -1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(out_status_flistp,PIN_FLD_NAME,"FAILED",ebufp);
		PIN_FLIST_FLD_SET(out_status_flistp,PIN_FLD_ACCOUNT_NO,"",ebufp);
		PIN_FLIST_FLD_SET(out_status_flistp,PIN_FLD_LAST_NAME,"",ebufp);
		PIN_FLIST_FLD_SET(out_status_flistp,PIN_FLD_FIRST_NAME,"",ebufp);
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_web_acc_creation status flist", out_status_flistp);
		*o_flistpp = out_status_flistp;

		PIN_FLIST_DESTROY_EX(&out_status_flistp, NULL);
		PIN_ERR_CLEAR_ERR(ebufp);
                return;
	}

	PIN_FLIST_FLD_PUT(out_status_flistp,PIN_FLD_POID,PIN_FLIST_FLD_GET(out_cust_comm_flist,
                                PIN_FLD_ACCOUNT_OBJ, 1, ebufp), ebufp);
	PIN_FLIST_FLD_SET(out_status_flistp,PIN_FLD_NAME,"SUCCESS",ebufp);

	ro_flistp = PIN_FLIST_CREATE(ebufp);
	PIN_FLIST_FLD_PUT(ro_flistp, PIN_FLD_POID ,PIN_FLIST_FLD_GET(out_cust_comm_flist,
        	PIN_FLD_ACCOUNT_OBJ, 1, ebufp), ebufp);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,"fm_web_acc_creation PCM_OP_READ_OBJ \
        	input flist", ro_flistp);
	PCM_OP(ctxp, PCM_OP_READ_OBJ, 0, ro_flistp, &ro_outflistp, ebufp);
	PIN_FLIST_FLD_SET(out_status_flistp, PIN_FLD_ACCOUNT_NO, PIN_FLIST_FLD_GET(ro_outflistp,
        	PIN_FLD_ACCOUNT_NO, 0, ebufp), ebufp);
	
	name_iflistp = PIN_FLIST_ELEM_GET(ro_outflistp,
                                        PIN_FLD_NAMEINFO, 1, 0, ebufp);
//	PIN_FLIST_FLD_SET(out_status_flistp, PIN_FLD_NAMEINFO, PIN_FLIST_FLD_GET(ro_outflistp,
  //                                      PIN_FLD_NAMEINFO, 0, ebufp), ebufp);
	PIN_FLIST_FLD_SET(out_status_flistp, PIN_FLD_LAST_NAME, PIN_FLIST_FLD_GET(name_iflistp,
                                        PIN_FLD_LAST_NAME, 0, ebufp), ebufp);
	PIN_FLIST_FLD_SET(out_status_flistp, PIN_FLD_FIRST_NAME, PIN_FLIST_FLD_GET(name_iflistp,
                                        PIN_FLD_FIRST_NAME, 0, ebufp), ebufp);		
	
	PIN_FLIST_FLD_SET(out_status_flistp, PIN_FLD_EMAIL_ADDR, PIN_FLIST_FLD_GET(name_iflistp,
                                        PIN_FLD_EMAIL_ADDR, 0, ebufp), ebufp);
	PIN_FLIST_FLD_SET(out_status_flistp,PIN_FLD_VALUE,PIN_FLIST_FLD_GET(out_cust_comm_flist,
                                PIN_FLD_NAME, 1, ebufp), ebufp);
	
//	PIN_FLIST_FLD_SET(out_status_flistp,PIN_FLD_VALUE,"OEC 9900 Elite - Equipment" ,ebufp);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
                "fm_web_acc_creation status flist", out_status_flistp);
	
	*o_flistpp = out_status_flistp;

	if (plan_flistp) PIN_FLIST_DESTROY_EX(&plan_flistp, NULL);

	return;

}

/***************************************************
* Function Name: fm_web_get_prod_details
* Function Description: This function fetches 
* the product,deal,plan details from the plan name 
* Function Parameters:
* @param [IN] . ctxp            : Context pointer
* @param [IN] . i_flistp        : input flist pointer
* @param [IN] . ebufp           : Error Buffer
* Function Return Value: returns an flist with all
* product,deal,plan details
*****************************************************/

static pin_flist_t *
fm_web_get_prod_details(
        pcm_context_t           *ctxp,
        pin_flist_t             *input_flistp,
        pin_errbuf_t            *ebufp )
		
{
	pin_flist_t             *prod_flistp = NULL;
	pin_flist_t             *search_flistp = NULL;
	pin_flist_t             *arg_flistp = NULL;
	pin_flist_t             *res_flistp = NULL;
	pin_flist_t             *ser_flistp = NULL;
	pin_flist_t     	*r_flistp =NULL;
	pin_flist_t     	*plan_pdp =NULL;
	pin_flist_t     	*serv_out_flistp =NULL;
	pin_flist_t     	*deal_pdp =NULL;
	pin_flist_t     	*res_out_flistp =NULL;
	pin_flist_t		*robj_dealiflistp;
	pin_flist_t		*robj_dealoflistp;
	pin_flist_t             *prod_search_flistp = NULL;
	pin_flist_t		*cust_commit_servflistp = NULL;
	pin_flist_t		*cust_commit_dealflistp = NULL;
	pin_flist_t		*cust_commit_iflistp = NULL;
	pin_flist_t		*cust_commit_prodflistp = NULL;
	pin_flist_t		*return_flist = NULL;
	pin_flist_t		*tmp_flistp = NULL;
	char			*plan_name = NULL;	
	poid_t          	*a_pdp = NULL;
    	poid_t          	*s_pdp = NULL;
	poid_t          	*pro_pdp = NULL;
	int32           	s_flags = SRCH_DISTINCT;
	pin_cookie_t    	cookie_s = NULL;
	pin_cookie_t    	cookie_ser = NULL;
    	int32           	elemid_s = 0;
	int32           	elemid_ser = 0;
	char            	s_template[2*BUFSIZ] = {""};
	char            	*permitted = NULL;
	char 			*deal_name = NULL;
	void            	*vp = NULL;
	poid_t                  *prod_poid = NULL;
	char 			*name = NULL;
	char 			*tag = "GE PLAN";
	
	
//	prod_flistp = PIN_FLIST_CREATE(ebufp);
	prod_flistp = PIN_FLIST_COPY(input_flistp, ebufp);
	
	plan_name =  PIN_FLIST_FLD_GET(prod_flistp,
                                PIN_FLD_NAME, 0, ebufp);
								
	search_flistp = PIN_FLIST_CREATE(ebufp);
	
	a_pdp = (poid_t *)PIN_FLIST_FLD_GET(prod_flistp, PIN_FLD_POID,
                                        0, ebufp);

	s_pdp = PIN_POID_CREATE(PIN_POID_GET_DB(a_pdp), "/search",
                                 -1, ebufp);
        if( s_pdp ) {
                PIN_FLIST_FLD_PUT(search_flistp, PIN_FLD_POID,
                                      (void *)s_pdp, ebufp);
        }

    	PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_FLAGS, (void *)&s_flags, ebufp);
	strcpy ( s_template, "select X from /plan where F1 = V1 ");
	PIN_FLIST_FLD_SET(search_flistp, PIN_FLD_TEMPLATE, (void *)s_template,
                                        ebufp);
	arg_flistp = PIN_FLIST_ELEM_ADD (search_flistp, PIN_FLD_ARGS, 1, ebufp);
//	PIN_FLIST_FLD_SET(arg_flistp, PIN_FLD_NAME, (void *)plan_name,
  //                                      ebufp);
	name = (char *)PIN_FLIST_FLD_GET(input_flistp,PIN_FLD_NAME , 1, ebufp);	
	//if ((*name == "GE PLAN"))
	if (strcmp(name, tag) == 0)
	{
	PIN_FLIST_FLD_SET(arg_flistp, PIN_FLD_NAME,"OEC 9900 Elite - Equipment", ebufp);
	}
	else 
	{
//	PIN_FLIST_FLD_SET(arg_flistp, PIN_FLD_NAME,"monthly trial", ebufp);
	PIN_FLIST_FLD_SET(arg_flistp, PIN_FLD_NAME,(void *)name, ebufp);
	}	
	PIN_FLIST_ELEM_ADD (search_flistp, PIN_FLD_RESULTS, 0, ebufp);

	res_flistp = PIN_FLIST_ELEM_ADD (search_flistp,  PIN_FLD_RESULTS,
                         0, ebufp);
						 
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
        "fm_web_get_prod_details : search input flist",
        search_flistp);
	
	PCM_OP(ctxp, PCM_OP_SEARCH, 0, search_flistp, &r_flistp, ebufp);
	
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,
        "fm_web_get_prod_details : search output flist",
        r_flistp);

	//return_flist = PIN_FLIST_COPY(r_flistp, ebufp);
	return_flist = PIN_FLIST_CREATE(ebufp);

	while((res_out_flistp = PIN_FLIST_ELEM_GET_NEXT
		(r_flistp, PIN_FLD_RESULTS, &elemid_s,1, &cookie_s, ebufp)) != (pin_flist_t *)NULL)
		{
					
					//Plan_poid
					PIN_FLIST_FLD_SET(return_flist, PIN_FLD_POID, PIN_FLIST_FLD_GET(res_out_flistp,
                                        	PIN_FLD_POID, 0, ebufp), ebufp);			

					//Plan_name
					PIN_FLIST_FLD_SET(return_flist, PIN_FLD_CODE, PIN_FLIST_FLD_GET(res_out_flistp,
                                        PIN_FLD_CODE, 0, ebufp), ebufp);
			
				while((	serv_out_flistp = PIN_FLIST_ELEM_GET_NEXT(
								res_out_flistp, PIN_FLD_SERVICES, &elemid_ser,
									1, &cookie_ser, ebufp)) != (pin_flist_t *)NULL)
{
					deal_pdp = (poid_t *)PIN_FLIST_FLD_GET(serv_out_flistp,
                                        PIN_FLD_DEAL_OBJ, 0, ebufp);

					//deal_obj
					PIN_FLIST_FLD_SET(return_flist, PIN_FLD_DEAL_OBJ, PIN_FLIST_FLD_GET(serv_out_flistp,
                                        PIN_FLD_DEAL_OBJ, 0, ebufp), ebufp);

					robj_dealiflistp = PIN_FLIST_CREATE(ebufp);
					PIN_FLIST_FLD_SET(robj_dealiflistp, PIN_FLD_POID, (void *)deal_pdp, ebufp);
					PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,"fm_web_get_prod_details PCM_OP_READ_OBJ \
						input flist", robj_dealiflistp);
					PCM_OP(ctxp, PCM_OP_READ_OBJ, 0, robj_dealiflistp, &robj_dealoflistp, ebufp);	
					PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG,"fm_web_get_prod_details PCM_OP_READ_OBJ : \
						output flist", robj_dealoflistp);

					if( PIN_ERR_IS_ERR( ebufp )) {
                               				PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_DEBUG,
                                       				"fm_web_get_prod_details: PCM_OP_READ_OBJ error", ebufp);
							}

					//deal_name
					PIN_FLIST_FLD_SET(return_flist, PIN_FLD_NAME, PIN_FLIST_FLD_GET(robj_dealoflistp,
                                        PIN_FLD_NAME, 0, ebufp), ebufp);			

					//permitted
					PIN_FLIST_FLD_SET(return_flist, PIN_FLD_PERMITTED, PIN_FLIST_FLD_GET(robj_dealoflistp,
                                       	PIN_FLD_PERMITTED, 0, ebufp), ebufp);			

					tmp_flistp = PIN_FLIST_ELEM_GET(robj_dealoflistp, PIN_FLD_PRODUCTS,	
						 0, 0, ebufp);
					//product_poid



					PIN_FLIST_FLD_SET(return_flist, PIN_FLD_PRODUCT_OBJ, PIN_FLIST_FLD_GET(tmp_flistp, 
					PIN_FLD_PRODUCT_OBJ, 0, ebufp)  , ebufp);	

					/*Action get the product name and set it*/

					if (robj_dealiflistp) PIN_FLIST_DESTROY_EX(&robj_dealiflistp, NULL);
					
					if (robj_dealoflistp) PIN_FLIST_DESTROY_EX(&robj_dealoflistp, NULL);
	return return_flist;	} 
}
	//return return_flist; 
	
}
/***************************************************
* Function Name: fm_web_pol_prep_serv_array
* Function Description: This function creates the
* service array by taking the input from 
* fm_web_get_prod_details function for creating
* creating flist for PCM_OP_CUST_COMMIT_CUSTOMER
* Function Parameters:
* @param [IN] . ctxp            : Context pointer
* @param [IN] . ebufp           : Error Buffer
* @param [IN] . plan_flistp        : input flist pointer
* @param [OUT] . in_cust_comm_flist : output flist pointer
* Function Return Value: None
*****************************************************/


static void
fm_web_pol_prep_serv_array(
	pcm_context_t		*ctxp,
	pin_flist_t		*plan_flistp,
	pin_flist_t		*in_cust_comm_flist,
	pin_errbuf_t		*ebufp)
{
	pin_flist_t		*tmp_flistp = NULL;
	pin_flist_t		*acct_flistp = NULL;
	pin_flist_t		*bill_flistp = NULL;
	pin_flist_t		*icur_flistp = NULL;
	pin_flist_t		*amount_flistp = NULL;
	pin_flist_t		*sec_flistp = NULL;
	pin_flist_t		*rsec_flistp = NULL;

	int32			elemid = 0;
	pin_cookie_t		cookie = NULL;
	int32			secondary_currency = 0;
	poid_t			*b_pdp = NULL;
	poid_t			*deal_obj = NULL;
	poid_t			*serv_obj = NULL;
	void			*vp = NULL;
	pin_flist_t		*tmp_serv_flistp = NULL;
	pin_flist_t		*tmp_deals_flistp = NULL;
	pin_flist_t		*tmp_products_flistp = NULL;
	pin_flist_t		*tmp_dealinfo_flistp = NULL;
	u_int			status = 1;
	u_int32			d_flag = 0;
	pin_decimal_t *quantity = pbo_decimal_from_double(1.0,ebufp);
	pin_decimal_t *usage_discount = pbo_decimal_from_double(0.0,ebufp);


	if (PIN_ERR_IS_ERR(ebufp))
		return;
	PIN_ERR_CLEAR_ERR(ebufp);

	/***********************************************************
	 * Debug what we got.
	 ***********************************************************/
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "fm_web_pol_prep_serv_array" \
	 "  (plan_flistp)", plan_flistp);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG," Before");

	//plan name

	/*
	0 PIN_FLD_POID           POID [0] 0.0.0.1 /plan 380322 0
	0 PIN_FLD_CODE            STR [0] "1000_minutes_voice_plan"
	0 PIN_FLD_DEAL_OBJ       POID [0] 0.0.0.1 /deal 379298 0
	0 PIN_FLD_NAME            STR [0] "1000_minutes_deal"
	0 PIN_FLD_PERMITTED       STR [0] "/service/telco"
	0 PIN_FLD_PRODUCT_OBJ    POID [0] 0.0.0.1 /product 380706 0
	*/

		PIN_FLIST_FLD_SET(in_cust_comm_flist, PIN_FLD_NAME, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_CODE, 1, ebufp), ebufp);

		PIN_FLIST_FLD_SET(in_cust_comm_flist, PIN_FLD_POID, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_POID, 1, ebufp), ebufp);
		//service array

		tmp_serv_flistp = PIN_FLIST_ELEM_ADD (in_cust_comm_flist,PIN_FLD_SERVICES,
             			0, ebufp);
		
		PIN_FLIST_FLD_SET(tmp_serv_flistp, PIN_FLD_PASSWD_CLEAR, "XXXX", ebufp);
		
		time_t	unique_login = pin_virtual_time(NULL);
	 /* 	char*tempstr; 
		sprintf(tempstr,"%d",unique_login);
*/
		char buffer [33];
		sprintf(buffer,"%d",unique_login);
		PIN_FLIST_FLD_SET(tmp_serv_flistp, PIN_FLD_LOGIN,buffer, ebufp);

		deal_obj = PIN_FLIST_FLD_GET(plan_flistp,
                                PIN_FLD_DEAL_OBJ, 1, ebufp);
								
		serv_obj = PIN_POID_CREATE(PIN_POID_GET_DB(deal_obj),PIN_FLIST_FLD_GET(plan_flistp,
                                PIN_FLD_PERMITTED, 1, ebufp),-1,ebufp);

		PIN_FLIST_FLD_SET(tmp_serv_flistp,PIN_FLD_SERVICE_OBJ,serv_obj,ebufp);
		
		//deals array

		tmp_deals_flistp = PIN_FLIST_ELEM_ADD (tmp_serv_flistp,PIN_FLD_DEALS,
           			0, ebufp);

		PIN_FLIST_FLD_SET(tmp_deals_flistp, PIN_FLD_DEAL_OBJ, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_DEAL_OBJ, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_deals_flistp, PIN_FLD_NAME, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_NAME, 1, ebufp), ebufp);	
		PIN_FLIST_FLD_SET(tmp_deals_flistp, PIN_FLD_PERMITTED, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_PERMITTED, 1, ebufp), ebufp);
		
		//products array

		tmp_products_flistp = PIN_FLIST_ELEM_ADD (tmp_deals_flistp,PIN_FLD_PRODUCTS,
           			0, ebufp);
	
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_PURCHASE_END_T, (void *)0, ebufp);						 
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_PURCHASE_START_T, (void *)0, ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_QUANTITY,  quantity , ebufp);						 
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_USAGE_DISCOUNT, (void *) usage_discount , ebufp);	
		
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_PRODUCT_OBJ, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_PRODUCT_OBJ, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_STATUS, &status, ebufp);

		
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_USAGE_END_T, NULL, ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_USAGE_START_T, NULL, ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_CYCLE_END_T, NULL, ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_CYCLE_START_T, NULL, ebufp);

		PIN_FLIST_FLD_SET(tmp_deals_flistp, PIN_FLD_END_T, NULL, ebufp);
		PIN_FLIST_FLD_SET(tmp_deals_flistp, PIN_FLD_START_T, NULL, ebufp);
		
		//dealinfo array
		tmp_dealinfo_flistp = PIN_FLIST_ELEM_ADD (tmp_serv_flistp,PIN_FLD_DEAL_INFO,
           			0, ebufp);

		tmp_products_flistp = PIN_FLIST_ELEM_ADD (tmp_dealinfo_flistp,PIN_FLD_PRODUCTS,
           			0, ebufp);

		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_PURCHASE_END_T, (void *)0, ebufp);						 
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_PURCHASE_START_T, (void *)0, ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_QUANTITY, (void *) quantity , ebufp);						 
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_USAGE_DISCOUNT, (void *) usage_discount , ebufp);	
		
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_PRODUCT_OBJ, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_PRODUCT_OBJ, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_STATUS, &status, ebufp);

		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_USAGE_END_T, NULL, ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_USAGE_START_T, NULL, ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_CYCLE_END_T, NULL, ebufp);
		PIN_FLIST_FLD_SET(tmp_products_flistp, PIN_FLD_CYCLE_START_T, NULL, ebufp);

		PIN_FLIST_FLD_SET(tmp_dealinfo_flistp, PIN_FLD_NAME, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_NAME, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_dealinfo_flistp, PIN_FLD_END_T , (void *)0, ebufp);
		PIN_FLIST_FLD_SET(tmp_dealinfo_flistp,PIN_FLD_FLAGS,(void *)&d_flag,ebufp);

		PIN_FLIST_FLD_SET(tmp_dealinfo_flistp, PIN_FLD_START_T , (void *)0, ebufp);
		PIN_FLIST_FLD_SET(tmp_dealinfo_flistp, PIN_FLD_POID, PIN_FLIST_FLD_GET(plan_flistp, 
				PIN_FLD_DEAL_OBJ, 1, ebufp), ebufp);

	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG," Before");
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "fm_web_pol_prep_serv_array" \
	 "  in_cust_comm_flist....", in_cust_comm_flist);
	PIN_ERR_LOG_MSG(PIN_ERR_LEVEL_DEBUG," After");
}

/***************************************************
* Function Name: fm_web_pol_prep_nameinfo_array
* Function Description: This function creates the
* nameinfo,accountinfo and payinfo array to create
* input flist for PCM_OP_CUST_COMMIT_CUSTOMER
* Function Parameters:
* @param [IN] . ctxp            : Context pointer
* @param [IN] . ebufp           : Error Buffer
* @param [IN] . i_flistp        : input flist pointer
* @param [OUT] . in_cust_comm_flist : output flist pointer
* Function Return Value: None
*****************************************************/


static void
fm_web_pol_prep_nameinfo_array(
        pcm_context_t           *ctxp,
        pin_flist_t             *i_flistp,
        pin_flist_t             *in_cust_comm_flist,
        pin_errbuf_t            *ebufp)
{

	pin_flist_t		*tmp_nameinfo_flistp = NULL;
	pin_flist_t		*tmp_phone_flistp = NULL;	
	pin_flist_t		*tmp_accinfo_flistp = NULL;
	pin_flist_t		*tmp_payinfo_flistp;
	pin_flist_t		*tmp_info_subs = NULL;
	pin_flist_t		*tmp_invinfo_flistp = NULL;
	pin_flist_t             *tmp_invinfo_flistp1 = NULL;
	u_int                   p_type = 1;
	u_int			del_pre = 0;
	u_int			pay_type = 10001;
	poid_t                  *acc_pdp = NULL;
	poid_t			*inv_pdp = NULL;

	
	//nameinfo array
	/*0 PIN_FLD_NAMEINFO      ARRAY [1] allocated 20, used 10
        1     PIN_FLD_LAST_NAME       STR [0] "JOHN"
        1     PIN_FLD_FIRST_NAME      STR [0] "JACK"
        1     PIN_FLD_PHONES        ARRAY [0] allocated 20, used 2
        2         PIN_FLD_PHONE           STR [0] "9966551122"
        2         PIN_FLD_TYPE            ENUM [0] 1
        1     PIN_FLD_EMAIL_ADDR      STR [0] "abc@g.com"
        1     PIN_FLD_COUNTRY         STR [0] "IN"
        1     PIN_FLD_ZIP             STR [0] "560048"
        1     PIN_FLD_CITY            STR [0] "BANGALORE"
	1     PIN_FLD_STATE           STR [0] "CA"
        1     PIN_FLD_ADDRESS         STR [0] "PSN" */

         	tmp_nameinfo_flistp = PIN_FLIST_ELEM_ADD (in_cust_comm_flist,PIN_FLD_NAMEINFO,
                                1, ebufp);
		PIN_FLIST_FLD_SET(tmp_nameinfo_flistp, PIN_FLD_LAST_NAME, PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_LAST_NAME, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_nameinfo_flistp, PIN_FLD_FIRST_NAME, PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_FIRST_NAME, 1, ebufp), ebufp);

		tmp_phone_flistp = PIN_FLIST_ELEM_ADD(tmp_nameinfo_flistp,PIN_FLD_PHONES,
				0, ebufp);
		PIN_FLIST_FLD_SET(tmp_phone_flistp, PIN_FLD_PHONE, PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_PHONE, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_phone_flistp,PIN_FLD_TYPE,(void *)&p_type, ebufp);
		PIN_FLIST_FLD_SET(tmp_nameinfo_flistp, PIN_FLD_EMAIL_ADDR ,PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_EMAIL_ADDR, 1, ebufp), ebufp);	
		PIN_FLIST_FLD_SET(tmp_nameinfo_flistp,PIN_FLD_COUNTRY, PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_COUNTRY, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_nameinfo_flistp,PIN_FLD_ZIP, PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_ZIP, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_nameinfo_flistp, PIN_FLD_STATE ,"CA", ebufp);
		PIN_FLIST_FLD_SET(tmp_nameinfo_flistp,PIN_FLD_CITY, PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_CITY, 1, ebufp), ebufp);
		PIN_FLIST_FLD_SET(tmp_nameinfo_flistp,PIN_FLD_ADDRESS, PIN_FLIST_FLD_GET(i_flistp,
                                PIN_FLD_ADDRESS, 1, ebufp), ebufp);

	//accountinfo array

	/*0 PIN_FLD_ACCTINFO      ARRAY [0] allocated 20, used 4
	1     PIN_FLD_POID           POID [0] 0.0.0.1 /account -1 0*/
		
		tmp_accinfo_flistp = PIN_FLIST_ELEM_ADD (in_cust_comm_flist,PIN_FLD_ACCTINFO,
                                0, ebufp);

		acc_pdp = PIN_FLIST_FLD_GET(i_flistp,PIN_FLD_POID, 1, ebufp);
		PIN_FLIST_FLD_SET(tmp_accinfo_flistp, PIN_FLD_POID,PIN_POID_CREATE(1, "/account", -1, ebufp), ebufp);

	//Action payinfo array	

	/*0 PIN_FLD_PAYINFO       ARRAY [0] allocated 20, used 6
	1     PIN_FLD_POID           POID [0] 0.0.0.1 /payinfo/invoice -1 0
	1     PIN_FLD_INHERITED_INFO SUBSTRUCT [0] allocated 20, used 1
	2         PIN_FLD_INV_INFO      ARRAY [0] allocated 20, used 10
	3             PIN_FLD_DELIVERY_PREFER   ENUM [0] 0
	1     PIN_FLD_PAY_TYPE       ENUM [0] 10001*/
	

		tmp_payinfo_flistp = PIN_FLIST_ELEM_ADD (in_cust_comm_flist,PIN_FLD_PAYINFO,
                                0, ebufp);
                PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "fm_web_pol_prep_nameinfo_array-1" , tmp_payinfo_flistp);


		inv_pdp = PIN_POID_CREATE(PIN_POID_GET_DB(acc_pdp), "/payinfo/invoice",
                                 -1, ebufp);

		PIN_FLIST_FLD_SET(tmp_payinfo_flistp,PIN_FLD_POID,(void *)inv_pdp, ebufp);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "fm_web_pol_prep_nameinfo_array-2" , tmp_payinfo_flistp);
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "fm_web_pol_prep_nameinfo_array-2-in_cust_comm_flist" , in_cust_comm_flist);

		tmp_info_subs = PIN_FLIST_SUBSTR_ADD(tmp_payinfo_flistp,PIN_FLD_INHERITED_INFO,
				ebufp);
        if (PIN_ERR_IS_ERR(ebufp)) {
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "fm_web_pol_prep_nameinfo_array-i33 error", ebufp);
        }

	tmp_invinfo_flistp = PIN_FLIST_ELEM_ADD(tmp_info_subs,PIN_FLD_INV_INFO,
				0, ebufp);

       if (PIN_ERR_IS_ERR(ebufp)) {
                PIN_ERR_LOG_EBUF(PIN_ERR_LEVEL_ERROR,
                        "fm_web_pol_prep_nameinfo_array-i44 error", ebufp);
        }


	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "fm_web_pol_prep_nameinfo_array-3" , tmp_payinfo_flistp);
		PIN_FLIST_FLD_SET(tmp_invinfo_flistp, PIN_FLD_DELIVERY_PREFER,&del_pre, ebufp);		
	PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "fm_web_pol_prep_nameinfo_array-4" , tmp_payinfo_flistp);
	PIN_FLIST_FLD_SET(tmp_payinfo_flistp, PIN_FLD_PAY_TYPE,&pay_type, ebufp);
		PIN_ERR_LOG_FLIST(PIN_ERR_LEVEL_DEBUG, "fm_web_pol_prep_nameinfo_array" \
         	"in_cust_comm_flist", in_cust_comm_flist);	
	tmp_invinfo_flistp1 = PIN_FLIST_ELEM_ADD(in_cust_comm_flist,PIN_FLD_BILLINFO,
                                0, ebufp);
	PIN_FLIST_FLD_SET(tmp_invinfo_flistp1, PIN_FLD_PAY_TYPE,&pay_type, ebufp);

}

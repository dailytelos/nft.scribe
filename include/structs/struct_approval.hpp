// a vector of these stores the approvals for a system activity
struct struct_approval {
    name approver;
    uint8_t level;         //records current level of user approver
    uint8_t status;        //status update applied by approver
    uint64_t orgid;        //organisation id of approval
    time_point_sec tstamp; //time of approval

    struct_approval() {
        approver    = name("name.nullaaaa");
        level       = 0;
        status      = 0;
        orgid       = 0;
        tstamp      = time_point_sec(0);
    };

    struct_approval(name i_approver, uint8_t i_level, uint8_t i_status, uint64_t i_orgid, time_point_sec i_tstamp) {
        approver    = i_approver;
        level       = i_level;
        status      = i_status;
        orgid       = i_orgid;
        tstamp      = i_tstamp;
    };

    struct_approval(name i_approver, uint8_t i_level, uint8_t i_status, uint64_t i_orgid) {
        approver    = i_approver;
        level       = i_level;
        status      = i_status;
        orgid       = i_orgid;
        tstamp      = time_point_sec(current_time_point().sec_since_epoch());
    };

    struct_approval(tuple<eosio::name, uint8_t, uint8_t, uint64_t> &tplApprove) {
        approver   = get<0>(tplApprove);
        level      = get<1>(tplApprove);
        status     = get<2>(tplApprove);
        orgid      = get<3>(tplApprove);
        tstamp     = time_point_sec(current_time_point().sec_since_epoch());
    };

    EOSLIB_SERIALIZE(struct_approval, (approver)(level)(status)(orgid)(tstamp));
};

void _csink_producer_appr(const name& approver, const uint64_t& csinkid) {

    //get production id out from csinkid
    data_index _data_table_cs( get_self(), DATA_TYPE_CERT_SNK.value );
    auto data_itr_cs = _data_table_cs.find(csinkid);

    check(data_itr_cs != _data_table_cs.end(), "C-Sink ID data not found. ");

    struct_data cCsink = data_itr_cs->d;
    uint64_t prodid = (uint64_t) cCsink.get_var_as_uint("u_prod_certn");

    data_index _data_table_pr( get_self(), DATA_TYPE_CERT_PRO.value );
    auto data_itr_pr = _data_table_pr.find(prodid);

    struct_data cProd = data_itr_pr->d;

    check(data_itr_pr != _data_table_pr.end(), "Production ID data not found. ");

    uint64_t prod_org = cProd.header.orgid;

    //check for datasubmit auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, prod_org);

    check( (auth == AUTH_LEVEL_CORP_APPROVE) ||
            (auth == AUTH_LEVEL_CORP_ADMIN) ||
            (auth == AUTH_ADMIN_MANL_APPROVE) ||
            (auth == AUTH_ADMIN_MASTER) ||
            (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
};

void _csink_check_prod(const uint64_t& csinkid) {
    
    //get production id out from csinkid
    data_index _data_table_cs( get_self(), DATA_TYPE_CERT_SNK.value );
    auto data_itr_cs = _data_table_cs.find(csinkid);

    check(data_itr_cs != _data_table_cs.end(), "C-Sink ID data not found. ");

    struct_data cCsink = data_itr_cs->d;
    uint64_t prodid = (uint64_t) cCsink.get_var_as_uint("u_prod_certn");

    data_index _data_table_pr( get_self(), DATA_TYPE_CERT_PRO.value );
    auto data_itr_pr = _data_table_pr.find(prodid);

    check(data_itr_pr != _data_table_pr.end(), "Production ID data not found. ");

    struct_data cProd = data_itr_pr->d;

    uint64_t prod_org = cProd.header.orgid;

    asset aCSink = cCsink.get_var_as_asset("a_gross");
    asset aProd  = cProd.get_var_as_asset("a_tproduced");
    asset aSunk  = cProd.get_var_as_asset("a_tcsunk");
    asset aRemain = asset((aProd.amount - aSunk.amount), symbol(aProd.symbol.code(), aProd.symbol.precision())); 

    check(aCSink.amount <= aRemain.amount, "Production run lacks remaining gross tonnage for you to sink, production run contains a remaining gross amount of: " + aRemain.to_string());
    
};


void _csink_apply_prod(const uint64_t& csinkid) {
    
    //get production id out from csinkid
    data_index _data_table_cs( get_self(), DATA_TYPE_CERT_SNK.value );
    auto data_itr_cs = _data_table_cs.find(csinkid);

    check(data_itr_cs != _data_table_cs.end(), "C-Sink ID data not found. ");

    struct_data cCsink = data_itr_cs->d;
    uint64_t prodid = (uint64_t) cCsink.get_var_as_uint("u_prod_certn");

    data_index _data_table_pr( get_self(), DATA_TYPE_CERT_PRO.value );
    auto data_itr_pr = _data_table_pr.find(prodid);

    struct_data cProd = data_itr_pr->d;

    check(data_itr_pr != _data_table_pr.end(), "Production ID data not found. ");

    uint64_t prod_org = cProd.header.orgid;

    asset aCSink = cCsink.get_var_as_asset("a_gross");
    asset aTokenIss = cCsink.get_var_as_asset("a_tavg");
    asset aProd  = cProd.get_var_as_asset("a_tproduced");
    asset aSunk  = cProd.get_var_as_asset("a_tcsunk");
    asset aRemain = asset((aProd.amount - aSunk.amount), symbol(aProd.symbol.code(), aProd.symbol.precision())); 

    //production update for csink
    _data_table_pr.modify( data_itr_pr, get_self(), [&]( auto& row ) {
        
        //update a_tcsunk and check against a_tproduced
        asset newSunk = asset((aCSink.amount + aSunk.amount), symbol(aProd.symbol.code(), aProd.symbol.precision())); 
        check(newSunk.amount <= aProd.amount, "You cannot csink an amount greater than the production run. ");
        row.d.set_var("a_tcsunk",newSunk.to_string());

        //update a_tissued which is a token multiplied (COXC)
        asset aOldIss   = cProd.get_var_as_asset("a_tissued");
        asset newIssued = asset((aTokenIss.amount + aOldIss.amount), symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec")))); 

        row.d.set_var("a_tissued",newIssued.to_string());

        check(row.d.header.status >= STATUS_DATA_ADMIN_APPROVED, "Unable to apply production without admin approval to the production run. ");

        if(row.d.header.status == STATUS_DATA_ADMIN_APPROVED) {
            //Executed means that at least some C-Sink has been performed on production cert, so the production is now finalized
            row.d.header.executed(true);
        }
    });

    //finalize ebc certificate
    uint64_t ebcid = (uint64_t) cCsink.get_var_as_uint("u_ebc_certn");

    data_index _data_table_ebc( get_self(), DATA_TYPE_CERT_EBC.value );
    auto data_itr_ebc = _data_table_ebc.find(ebcid);

    check(data_itr_ebc != _data_table_ebc.end(), "EBC ID data not found. ");

    _data_table_ebc.modify( data_itr_ebc, get_self(), [&]( auto& row ) {

        check(row.d.header.status >= STATUS_DATA_ADMIN_APPROVED, "Unable to apply C-Sink to EBC #, because the EBC has not been admin approved. ");        

        if(row.d.header.status == STATUS_DATA_ADMIN_APPROVED) {
            //Executed means that it has been used some
            row.d.header.executed(true);
        }

    });

    //start token issuance
    asset aTokenIssue = asset(aTokenIss.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
    string sMemo = "Issuance of C-Sink # " + to_string(csinkid) + " for " + aCSink.to_string() + " to the amount of " + aTokenIssue.to_string();

    //issue token supply (will still need to be claimed)
    action(
        permission_level{ getcontract(), "active"_n},
        getcontract(),
        "issue"_n,
        std::make_tuple(
            getcontract(),
            aTokenIssue,
            sMemo
        )
    ).send();


 /* INFO
    This action controls a direct update to a RAM variable, this is great for oracles to use, or more advanced users. It is specifically to handle updates to variables. It provides for finer controls of uploading multiple operations at once and having vectors of uint128_t / int128_t / std::string / asset uploaded to the blockchain in one action.

    signor (name) - EOSIO account to pay for all RAM, the authorized signor to update the variable.
    scope (name) - EOSIO account (default contract setup must equal signor) to which the variable is scoped
    varname (name) - EOSIO name used to register the variable in regvar ACTION
    header (vectorstd::string) - Provides a column header description for the data in uval, sval, nval, aval, currently does nothing in the contract
    operation (vectorstd::string) - Valid operations to perform on all data: set + - * / % min max
    index (uint8_t) - Index of where to apply value and operation into uval sval and nval
    uval (vector<uint128_t>) - Vector of uint128_t
    sval (vectorstd::string) - Vector of std::string
    nval (vector<int128_t>) - Vector of int128_t
    aval (vectoreosio::asset) - Vector of asset
    */
    string s_type = "m";
    string x_type = "x";
    uint64_t tlimit = 36;
    uint8_t vlimit = 8;
    vector <std::string> s_oper;
    s_oper.push_back("+");
    s_oper.push_back("+");

    uint8_t index = 0;


   //global
   vector <uint128_t> uval;
   vector <std::string> sval;
   vector <int128_t> nval;
   vector <eosio::asset> aval;

   aval.push_back(aTokenIssue);
   aval.push_back(asset(0, symbol("COXC", 4)));  
   

    action(
        permission_level{ get_self(), "active"_n},
        "data.scribe"_n,
        "regvar"_n,
        std::make_tuple(
            get_self(),
            get_self(),
            name("gcxstats"),
            name("gcx"),
            s_type,
            tlimit,
            vlimit
        )
    ).send();

   vector <std::string> s_memo_this;
   s_memo_this.push_back("COXC Total Global Issue");
   s_memo_this.push_back("COXC Total Global Retire");

      action(
        permission_level{ get_self(), "active"_n},
        "data.scribe"_n,
        "update"_n,
        std::make_tuple(
            get_self(),
            get_self(),
            name("gcxstats"),
            s_memo_this,
            s_oper,
            index,
            uval,
            sval,
            nval,
            aval
        )
    ).send();

   //organisation
    action(
        permission_level{ get_self(), "active"_n},
        "data.scribe"_n,
        "regvar"_n,
        std::make_tuple(
            get_self(),
            name(prod_org),
            "cxtotals"_n,
            "cxt"_n,
            x_type,
            tlimit,
            vlimit
        )
    ).send();

   s_memo_this.empty();
   s_memo_this.push_back("COXC Total Org Issue");
   s_memo_this.push_back("COXC Total Org Retire");

    action(
        permission_level{ get_self(), "active"_n},
        "data.scribe"_n,
        "update"_n,
        std::make_tuple(
            get_self(),
            name(prod_org),
            "cxtotals"_n,
            s_memo_this,
            s_oper,
            index,
            uval,
            sval,
            nval,
            aval
        )
    ).send();

    //regvar for dates
    /*
    Registers a new variable in TABLE vregister using varname under the scope. When specifying a type other than x, the signor can create RAM variable logging / mathematical operations by hour, day, month, year. This could be used to perform daily totals, monthly averages, daily lows, daily highs, etc. vardgt must be the unique 3 digit identifier for this variable.

    signor (name) - EOSIO account to pay for all RAM, the authorized signor to update the variable.
    scope (name) - EOSIO account (default contract setup must equal signor) to which the variable is scoped
    varname (name) - EOSIO name used to register the variable in regvar ACTION
    vardgt (name) - EOSIO name that is exactly 3 digits long when converted to string format. Inside TABLE vartable these digits will follow the type character in the row id: tXXXyymmddhh (the XXX position). As such, the three digits must be unique.
    type (std::string) - x normal, h hourly, d daily, m monthly, n yearly. This value will comprise the first character of the id in TABLE vartable .
    tlimit (uint64_t) - Max number of rows assigned to this variable (pertains to type = h d m n)
    vlimit (uint8_t) - Inside each row, this is the maximum vector size for uval sval and nval
    */

    
    action(
        permission_level{ get_self(), "active"_n},
        "data.scribe"_n,
        "regvar"_n,
        std::make_tuple(
            get_self(),
            name(prod_org),
            "cxchart"_n,
            "cxc"_n,
            s_type,
            tlimit,
            vlimit
        )
    ).send();

    s_memo_this.empty();
    s_memo_this.push_back("COXC Monthly Issue");
    s_memo_this.push_back("COXC Monthly Retire");

    action(
        permission_level{ get_self(), "active"_n},
        "data.scribe"_n,
        "update"_n,
        std::make_tuple(
            get_self(),
            name(prod_org),
            "cxchart"_n,
            s_memo_this,
            s_oper,
            index,
            uval,
            sval,
            nval,
            aval
        )
    ).send();

};

bool is_action_valid(const uint8_t& level, const uint64_t& auth_org, const uint8_t& status) {

    if(auth_org == ORG_ADMIN_ID) {
        if(level == AUTH_ADMIN_AUTO_APPROVE) {
            if( (status == STATUS_DATA_DELETION) ||
                (status == STATUS_DATA_LOCKED) ||
                (status == STATUS_DATA_ADMIN_APPROVED) ||
                (status == STATUS_DATA_EXECUTED) )
                { return true; }
        } else if (level == AUTH_ADMIN_MANL_APPROVE) {
            if( (status == STATUS_DATA_DELETION) ||
                (status == STATUS_DATA_LOCKED) ||
                (status == STATUS_DATA_ADMIN_APPROVED) ||
                (status == STATUS_DATA_EXECUTED) )
                { return true; }        
        } else if ((level == AUTH_ADMIN_MASTER) || (level == AUTH_LEVEL_ROOTADMIN)) {
            return true; 
        } else {
            return false;
        }
    } else {
        if(level == AUTH_LEVEL_CORP_SUBMIT) {
            if( (status == STATUS_DATA_DRAFT) || 
                (status == STATUS_DATA_SUBMIT) ||
                (status == STATUS_DATA_DELETION) ) { return true; }
        } else if (level == AUTH_LEVEL_CORP_APPROVE){
            if( (status == STATUS_DATA_CORP_APPROVED) ||
                (status == STATUS_DATA_DELETION) ) { return true; }
        } else if (level == AUTH_LEVEL_CORP_ADMIN) {
            if( (status == STATUS_DATA_DRAFT) || 
                (status == STATUS_DATA_SUBMIT) ||
                (status == STATUS_DATA_DELETION) ||
                (status == STATUS_DATA_CORP_APPROVED)  ) { return true; }
        } else {
            return false;
        }
    }

    return false;
}

//performs organisation match, where (approver orgid) == (from orgid)
// executes this when drafting, when submitting, when executing send transactions
void _check_send_from(const name& approver, const name& from) {
    
    //check for approver orgid
    uint64_t auth_org_a = get_org_id(approver);
    uint8_t auth_a = get_auth_by_org(approver, auth_org_a);

    //check for approver orgid
    uint64_t auth_org_f = get_org_id(from);
    uint8_t auth_f = get_auth_by_org(from, auth_org_f);

    if(auth_a < AUTH_ADMIN_AUTO_APPROVE) {
        check(auth_org_a == auth_org_f, "Unable to process send transaction, approver org id does not equal sender org id. ");
    }

    check(auth_a >= AUTH_LEVEL_CORP_SUBMIT, "Unable to process send transaction, approver must be able to at least submit transaction. ");
}
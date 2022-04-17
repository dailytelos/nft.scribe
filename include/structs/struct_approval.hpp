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
    uint64_t prodid = (uint64_t) cCsink.get_var_as_int("n_prod_certn");

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
    uint64_t prodid = (uint64_t) cCsink.get_var_as_int("n_prod_certn");

    data_index _data_table_pr( get_self(), DATA_TYPE_CERT_PRO.value );
    auto data_itr_pr = _data_table_pr.find(prodid);

    struct_data cProd = data_itr_pr->d;

    check(data_itr_pr != _data_table_pr.end(), "Production ID data not found. ");

    uint64_t prod_org = cProd.header.orgid;

    asset aCSink = cCsink.get_var_as_asset("a_gross");
    asset aProd  = cProd.get_var_as_asset("a_tproduced");
    asset aIssu  = cProd.get_var_as_asset("a_tissued");
    asset aRemain = asset((aProd.amount - aIssu.amount), symbol(aProd.symbol.code(), aProd.symbol.precision())); 

    check(aCSink.amount <= aRemain.amount, "Production run lacks remaining gross tonnage for you to sink, production run contains a remaining gross amount of: " + aRemain.to_string());
};


void _csink_apply_prod(const uint64_t& csinkid) {
    
    //get production id out from csinkid
    data_index _data_table_cs( get_self(), DATA_TYPE_CERT_SNK.value );
    auto data_itr_cs = _data_table_cs.find(csinkid);

    check(data_itr_cs != _data_table_cs.end(), "C-Sink ID data not found. ");

    struct_data cCsink = data_itr_cs->d;
    uint64_t prodid = (uint64_t) cCsink.get_var_as_int("n_prod_certn");

    data_index _data_table_pr( get_self(), DATA_TYPE_CERT_PRO.value );
    auto data_itr_pr = _data_table_pr.find(prodid);

    struct_data cProd = data_itr_pr->d;

    check(data_itr_pr != _data_table_pr.end(), "Production ID data not found. ");

    uint64_t prod_org = cProd.header.orgid;

    asset aCSink = cCsink.get_var_as_asset("a_gross");
    asset aProd  = cProd.get_var_as_asset("a_tproduced");
    asset aIssu  = cProd.get_var_as_asset("a_tissued");
    asset aRemain = asset((aProd.amount - aIssu.amount), symbol(aProd.symbol.code(), aProd.symbol.precision())); 

    //production update for csink
    _data_table_pr.modify( data_itr_pr, get_self(), [&]( auto& row ) {
        
        asset newIssued = asset((aCSink.amount + aIssu.amount), symbol(aProd.symbol.code(), aProd.symbol.precision())); 
        check(newIssued.amount <= aProd.amount, "You cannot csink an amount greater than the production run. ");
        row.d.set_var("a_tissued",newIssued.to_string());

        check(row.d.header.status >= STATUS_DATA_CORP_APPROVED, "You may only execute data when organisation has approved. ");
        
        //apply execution to production cert if remaining amount is reduced to zero
        if(aRemain.amount == 0) {
            row.d.header.executed(true);
        }
    });

    asset aTokenIssue = asset(aCSink.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
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
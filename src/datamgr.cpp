



ACTION carboncert::datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id) {
    checkfreeze();
    require_auth(creator);

    //check for draftcert auth
    uint64_t auth_org = get_org_id(creator);
    uint8_t auth = get_auth_by_org(creator, auth_org);

    //validate authority
    bool bCerts = (type == DATA_TYPE_CERT_EBC) || (type == DATA_TYPE_CERT_PRO) || (type == DATA_TYPE_CERT_SNK) || (type == DATA_TYPE_PORTF);
    bool bSend  = (type == DATA_TYPE_ACT_SEND);

    if(bCerts || bSend) {
        check( (auth == AUTH_LEVEL_CORP_SUBMIT) ||
            (auth == AUTH_LEVEL_CORP_ADMIN) ||
            (auth == AUTH_ADMIN_MANL_APPROVE) ||
            (auth == AUTH_ADMIN_MASTER) ||
            (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
    } else {
        check(false, "Invalid activity specified. ");
    }

    _datadraft(creator, type, strid, data, token, edit, id);
}


ACTION carboncert::datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type) {
    checkfreeze();
    require_auth(approver);

    //check for datasubmit auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    //validate authority
    bool bCerts = (type == DATA_TYPE_CERT_EBC) || (type == DATA_TYPE_CERT_PRO) || (type == DATA_TYPE_CERT_SNK) || (type == DATA_TYPE_PORTF);
    bool bSend  = (type == DATA_TYPE_ACT_SEND);

    if(bCerts || bSend) {
        if(appr_type == "submit") {
            check( (auth == AUTH_LEVEL_CORP_SUBMIT) ||
            (auth == AUTH_LEVEL_CORP_ADMIN) ||
            (auth == AUTH_ADMIN_MANL_APPROVE) ||
            (auth == AUTH_ADMIN_MASTER) ||
            (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
        } else if(appr_type == "corp_approve") { 

            //for csink, corp approval references the production corp, not the csink creator
            if(type == DATA_TYPE_CERT_SNK) {
                _csink_producer_appr(approver, id);
                _csink_check_prod(id);
            } else {
                check( ((auth == AUTH_LEVEL_CORP_ADMIN) ||
                (auth == AUTH_ADMIN_MANL_APPROVE) ||
                (auth == AUTH_ADMIN_MASTER) ||
                (auth == AUTH_LEVEL_ROOTADMIN)), "You lack the neccessary authorisation to perform this action. ");
            }

        } else if(appr_type == "admin_approve") {
            check( ((auth == AUTH_ADMIN_AUTO_APPROVE) ||
            (auth == AUTH_ADMIN_MANL_APPROVE) ||
            (auth == AUTH_ADMIN_MASTER) ||
            (auth == AUTH_LEVEL_ROOTADMIN)), "You lack the neccessary authorisation to perform this action. ");
        } else { check(false, "Invalid appr_type specified."); }

    } else {
        check(false, "Invalid activity specified. ");
    }

    _datasubmit(approver, type, id, appr_type);
}



ACTION carboncert::execute(const name& approver, const name& type, const uint64_t& id) {
    checkfreeze();
    require_auth(approver);

    //check for datasubmit auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    //validate authority
    bool bCerts = (type == DATA_TYPE_CERT_EBC) || (type == DATA_TYPE_CERT_PRO) || (type == DATA_TYPE_CERT_SNK) || (type == DATA_TYPE_PORTF);
    bool bSend  = (type == DATA_TYPE_ACT_SEND);

    if(bCerts || bSend) {
        if(type == DATA_TYPE_CERT_EBC) {  //admin executes to finalize
            check( (auth == AUTH_ADMIN_MANL_APPROVE) ||
                   (auth == AUTH_ADMIN_MASTER) ||
                   (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
        } else if(type == DATA_TYPE_CERT_PRO) { //cannot happen on this type, execute when first csink happens
            check(false, "You cannot execute a declaration of production. ");
            check( (auth == AUTH_LEVEL_CORP_APPROVE) ||
                   (auth == AUTH_LEVEL_CORP_ADMIN) ||
                   (auth == AUTH_ADMIN_MANL_APPROVE) ||
                   (auth == AUTH_ADMIN_MASTER) ||
                   (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
        } else if(type == DATA_TYPE_CERT_SNK) { //issues token, also finalizes the DATA_TYPE_CERT_PRO with execution status
                                                //production cert holder, finalizes csink
            _csink_producer_appr(approver, id);

        } else if(type == DATA_TYPE_PORTF) {  //admin uses to finalize
            check( (auth == AUTH_ADMIN_AUTO_APPROVE) ||
                   (auth == AUTH_ADMIN_MANL_APPROVE) ||
                   (auth == AUTH_ADMIN_MASTER) ||
                   (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
        } else if(type == DATA_TYPE_ACT_SEND) { //sends the token
            check( (auth == AUTH_ADMIN_AUTO_APPROVE) ||
                   (auth == AUTH_ADMIN_MANL_APPROVE) ||
                   (auth == AUTH_ADMIN_MASTER) ||
                   (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
        } else { check(false, "Invalid appr_type specified."); }

    } else {
        check(false, "Invalid activity specified. ");
    }

    _execute(approver, type, id);
}

ACTION carboncert::claim(const name& approver, const name& type, const uint64_t& id) {
    checkfreeze();
    require_auth(approver);

    //check for datasubmit auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    //validate authority
    bool bCerts = (type == DATA_TYPE_CERT_EBC) || (type == DATA_TYPE_CERT_PRO) || (type == DATA_TYPE_CERT_SNK) || (type == DATA_TYPE_PORTF);
    bool bSend  = (type == DATA_TYPE_ACT_SEND);

    if(bCerts || bSend) {
        if(type == DATA_TYPE_CERT_SNK) {

         _csink_producer_appr(approver, id);

        } else { check(false, "Invalid appr_type specified."); }

    } else {
        check(false, "Invalid activity specified. ");
    }

    _claim(approver, type, id);
}

ACTION carboncert::retire(const name& approver, const asset& quant) {
    checkfreeze();
    require_auth(approver);

    //check for datasubmit auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    check(auth >= AUTH_LEVEL_CORP_ADMIN, "You lack the required authority to perform the retire action. ");

    _retire(approver, quant);
}


// csinkclaim(const name& approver, const uint64_t& id)
//   Allows 
//
//    approver - wholesaler doing the csink (must be authorised wholesaler)
//    id       - production id
//    sunk     - qty in tonn sunk
//
/*ACTION carboncert::csinkclaim(const name& approver, const uint64_t& id, const asset& sunk) {
    checkfreeze();
    require_auth(approver);

    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    tuple<string, uint64_t, uint8_t, uint8_t, uint8_t> tplOrg = get_org_tuple(auth_org);

    string   orgname    = get<0>(tplOrg);
    uint64_t ebccertn   = get<1>(tplOrg);
    uint8_t  producer   = get<2>(tplOrg);
    uint8_t  supplier   = get<3>(tplOrg);
    uint8_t  retire     = get<4>(tplOrg);

    check(supplier == 1, "Organisation must have authorisation to perform c-sink activity, contact system administrator. ");


}*/
/*
ACTION carboncert::issuecredits(const name& approver, const uint64_t& id) {
    checkfreeze();
    require_auth(approver);

    //check for datasubmit auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);



    _issuecredits(approver, id);
}

ACTION carboncert::claimcredits(const name& approver, const uint64_t& id) {
    checkfreeze();
    require_auth(approver);

    //check for datasubmit auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    _claimcredits(approver, id);
}


ACTION retirefunds(const name& sender, const asset& supply) {
    checkfreeze();
    require_auth(approver);


}*/

void carboncert::_datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id) {

    name scope = type;
    name countvar = name_null;
    vector<string> verify = {};
    
    if(type.value == DATA_TYPE_CERT_EBC.value) {
        countvar = GLOBAL_COUNT_EBC;
        verify = VARDEF_CERT_EBC;
    } else if (type.value == DATA_TYPE_CERT_PRO.value) {
        countvar = GLOBAL_COUNT_PRO;
        verify = VARDEF_CERT_PROD;
    } else if (type.value == DATA_TYPE_CERT_SNK.value) {
        countvar = GLOBAL_COUNT_SNK;
        verify = VARDEF_CERT_CSNK;
    } else if (type.value == DATA_TYPE_PORTF.value) {
        countvar = GLOBAL_COUNT_PRT;
        verify = VARDEF_DATA_PORTF;
    } else if (type.value == DATA_TYPE_ACT_SEND.value) {
        countvar = GLOBAL_COUNT_SND;
        verify = VARDEF_ACT_SEND;
    } else if (type.value == GLOBAL_COUNT_RET.value) {
        countvar = GLOBAL_COUNT_RET;
        verify = VARDEF_ACT_SEND;
    } else { check(false, "Specified type for draft is invalid. "); }

    uint64_t count = (edit == 1) ? id : getglobalint(countvar) + 1;
    string sMemo   = "";

    data_index _data_table( get_self(), scope.value );
    auto data_itr = _data_table.find(count);

    //check for draftcert auth
    uint64_t auth_org = get_org_id(creator);
    uint8_t auth = get_auth_by_org(creator, auth_org);

    struct_approval cApprove = struct_approval(creator, auth, STATUS_DATA_DRAFT, auth_org);

    //emplace new data
    if(data_itr == _data_table.end()){

        _data_table.emplace( get_self(), [&]( auto& row ) {
            row.id = count;
            row.d  = struct_data(
                        struct_header(count, strid, type, creator, auth_org, STATUS_NONE),
                        data,
                        token
                   );

            row.d.is_data_valid(verify);

            row.d.header.draft(true);
        
            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was drafted by " + creator.to_string() + " (strid: " + strid + ")";
            print(sMemo);
        });

        setglobalint(countvar, count);
    } else { //update record
        check(data_itr->d.header.status < STATUS_DATA_CORP_APPROVED, "You cannot edit data that was already approved. ");

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {
            row.id = count;
            row.d  = struct_data(
                        struct_header(count, strid, type, creator, auth_org, STATUS_NONE),
                        data,
                        token
                   );

            row.d.is_data_valid(verify);

            row.d.header.draft(true);
        
            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was re-drafted by " + creator.to_string() + " (strid: " + strid + ")";
            print(sMemo);
        });
    }

    if(type == DATA_TYPE_ACT_SEND) {
        struct_data cData = data_itr->d;
        _check_send_from(creator, name(cData.get_var("s_from")));
    }
}


void carboncert::_datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type) {

    name scope = type;
    name countvar = name_null;
    vector<string> verify = {};
    
    if(type.value == DATA_TYPE_CERT_EBC.value) {
        countvar = GLOBAL_COUNT_EBC;
        verify = VARDEF_CERT_EBC;
    } else if (type.value == DATA_TYPE_CERT_PRO.value) {
        countvar = GLOBAL_COUNT_PRO;
        verify = VARDEF_CERT_PROD;
    } else if (type.value == DATA_TYPE_CERT_SNK.value) {
        countvar = GLOBAL_COUNT_SNK;
        verify = VARDEF_CERT_CSNK;
    } else if (type.value == DATA_TYPE_PORTF.value) {
        countvar = GLOBAL_COUNT_PRT;
        verify = VARDEF_DATA_PORTF;
    } else if (type.value == DATA_TYPE_ACT_SEND.value) {
        countvar = GLOBAL_COUNT_SND;
        verify = VARDEF_ACT_SEND;
    } else { check(false, "Specified type for draft is invalid. "); }

    uint64_t count = id;
    string sMemo   = "";

    data_index _data_table( get_self(), scope.value );
    auto data_itr = _data_table.find(count);

    //check for draftcert auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    struct_approval cApprove = struct_approval(approver, auth, STATUS_DATA_SUBMIT, auth_org);

    //test if record exists
    if(data_itr == _data_table.end()){
        check(false, "Supplied identification for data record does not exist. ");
    } else { //update record
        check(data_itr->d.header.status < STATUS_DATA_CORP_APPROVED, "You cannot submit data that was already approved. ");

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {
            
            row.d.is_data_valid(verify);

            if (type.value == DATA_TYPE_CERT_PRO.value) {
                row.d.set_var("a_tissued","0.0000 T");
            } else if (type.value == DATA_TYPE_CERT_SNK.value) {
                row.d.set_var("n_claimed","0");
                row.d.set_var("a_qtyretired","0.0000 T");
                row.d.set_var("n_retired","0");
            } else if (type.value == DATA_TYPE_PORTF.value) {
                row.d.set_var("a_csinks","0.0000 T");
                row.d.set_var("a_retired","0.0000 T");
            }

            if(appr_type == "submit") {
                row.d.header.submit(true);
            } else if(appr_type == "corp_approve") {
                row.d.header.corp_approve(true);
            } else if(appr_type == "admin_approve") {
                row.d.header.admin_approve(true);
            } else { check(false, "Invalid appr_type specified."); }

            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was submitted by " + approver.to_string() + " (strid: " + row.d.header.strid + ")";
            print(sMemo);
        });
    }

    if(type == DATA_TYPE_ACT_SEND) {
        struct_data cData = data_itr->d;
        _check_send_from(approver, name(cData.get_var("s_from")));
    }
}



/*
void carboncert::isebccertvalid(const uint64_t& certid) {
    // standard row update
    data_index _data_table( get_self(), DATA_TYPE_CERT_EBC.value);
    auto data_itr = _data_table.find(certid);

    check(data_itr != _data_table.end(), "Contract error, certid number does not exist in isebccertvalid! ");

    struct_data cData = data_itr->d;

    time_point_sec t_issue = cData.get_var_as_time("t_issue");
    time_point_sec t_expire = cData.get_var_as_time("t_expire");
    time_point_sec t_now = time_point_sec(current_time_point().sec_since_epoch());

    check(t_now.sec_since_epoch() >= t_issue.sec_since_epoch(), "EBC Certification has not yet become active, it becomes valid on: " + t_issue.to_string());
    check(t_now.sec_since_epoch() < t_expire.sec_since_epoch(), "EBC Certification has already expired on: " + t_expire.to_string());
}

void carboncert::_issuecredits(const name& approver, const uint64_t& id) {
    // standard row update
    data_index _data_table( get_self(), DATA_TYPE_CERT_SNK.value);
    auto data_itr = _data_table.find(id);

    check(data_itr != _data_table.end(), "Contract error, csink id  does not exist in _issuecredits! ");

    struct_data cData = data_itr->d;

    check(cData.header.status == STATUS_DATA_ADMIN_APPROVED, "Contract error, csink id was not approved by administrator.  Must be approved before tokens are issued. ");
    
    uint64_t n_prod_certn = cData.get_var_as_int("n_prod_certn");
    uint64_t n_port_certn = cData.get_var_as_int("n_port_certn");
    string s_loc = cData.get_var("s_loc");
    string s_type = cData.get_var("s_type");
    string s_desc = cData.get_var("s_desc");
    asset a_gross = cData.get_var_as_asset("a_gross");
    asset a_humidity = cData.get_var_as_asset("a_humidity");
    asset a_tmin = cData.get_var_as_asset("a_tmin");
    asset a_tmax = cData.get_var_as_asset("a_tmax");
    asset a_tavg = cData.get_var_as_asset("a_tavg");  //<--- CSink is issued off of this metric.
    uint64_t n_ystart = cData.get_var_as_int("n_ystart");
    uint64_t n_yend = cData.get_var_as_int("n_yend");
    uint64_t n_issued = cData.get_var_as_int("n_issued");
    uint64_t n_claimed = cData.get_var_as_int("n_claimed");
    asset a_qtyretired = cData.get_var_as_asset("a_qtyretired");
    uint64_t n_retired = cData.get_var_as_int("n_retired");

    check(n_retired == 0, "Contract error, this certificate claims to have been retired. ");
    check(n_claimed == 0, "Contract error, csink id has already been claimed. ");
    check(n_issued == 0, "Contract error, csink id has already been issued. ");

    //issue token supply (will still need to be claimed)
    asset issue = asset(a_tavg.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
    asset blank_retire = asset(0, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
    string sMemo = "Issuing " + a_tavg.to_string() + " from CSink cert# " + to_string(id) + " as " + issue.to_string() + ". ";

    _data_table.modify( data_itr, get_self(), [&]( auto& row ) {
        row.d.set_var("n_issued","1");
        row.d.set_var("a_qtyretired", blank_retire.to_string());
    });

    action(
        permission_level{ getcontract(), "active"_n},
        getcontract(),
        "issue"_n,
        std::make_tuple(
            getcontract(),
            issue,
            sMemo
        )
    ).send();

    //swap over to new table
    // standard row update
    uint64_t nCertIssCount = getglobalint(name("issuecertn"));
    data_index _data_table_i( get_self(), GLOBAL_COUNT_SNK_ISS.value);
    auto data_itr_i = _data_table_i.find(nCertIssCount);

    _data_table_i.emplace( get_self(), [&]( auto& row ) {
        row.id = nCertIssCount;
        row.d = data_itr->d;
    });

    nCertIssCount = nCertIssCount + 1;
    setglobalint(name("issuecertn"), nCertIssCount);

    data_itr = _data_table.erase(data_itr);
}


void carboncert::_claimcredits(const name& approver, const uint64_t& id) {
    // standard row update
    data_index _data_table( get_self(), GLOBAL_COUNT_SNK_ISS.value);
    auto data_itr = _data_table.find(id);

    check(data_itr != _data_table.end(), "Contract error, csink id  does not exist in _claimcredits! ");

    struct_data cData = data_itr->d;

    check(cData.header.status == STATUS_DATA_ADMIN_APPROVED, "Contract error, csink id was not approved by administrator.  Must be approved before tokens are claimed. ");
    
    uint64_t n_prod_certn = cData.get_var_as_int("n_prod_certn");
    uint64_t n_port_certn = cData.get_var_as_int("n_port_certn");
    string s_loc = cData.get_var("s_loc");
    string s_type = cData.get_var("s_type");
    string s_desc = cData.get_var("s_desc");
    asset a_gross = cData.get_var_as_asset("a_gross");
    asset a_humidity = cData.get_var_as_asset("a_humidity");
    asset a_tmin = cData.get_var_as_asset("a_tmin");
    asset a_tmax = cData.get_var_as_asset("a_tmax");
    asset a_tavg = cData.get_var_as_asset("a_tavg");  //<--- CSink is issued off of this metric.
    uint64_t n_ystart = cData.get_var_as_int("n_ystart");
    uint64_t n_yend = cData.get_var_as_int("n_yend");
    uint64_t n_issued = cData.get_var_as_int("n_issued");
    uint64_t n_claimed = cData.get_var_as_int("n_claimed");
    asset a_qtyretired = cData.get_var_as_asset("a_qtyretired");
    uint64_t n_retired = cData.get_var_as_int("n_retired");

    check(n_retired == 0, "Contract error, this certificate claims to have been retired. ");
    check(n_claimed == 0, "Contract error, csink id has already been claimed. ");
    check(n_issued == 1, "Contract error, csink id has not yet been issued. ");

    //issue token supply (will still need to be claimed)
    asset claim = asset(a_tavg.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
    string sMemo = "Claiming " + a_tavg.to_string() + " from CSink cert# " + to_string(id) + " as " + claim.to_string() + ". ";

    _data_table.modify( data_itr, get_self(), [&]( auto& row ) {
        row.d.set_var("n_claimed","1");
    });

    action(
        permission_level{ getcontract(), "active"_n},
        getcontract(),
        "transfer"_n,
        std::make_tuple(
            getcontract(),
            approver,
            claim,
            sMemo
        )
    ).send();
}

void carboncert::_retirefunds(const name& sender, const asset& supply) {

    check(supply.amount > 0, "Unable to retire 0 amount given in variable 'supply'. ");

    //check for sufficient deposit and reduce
    string sMemo   = "Retiring funds " + supply.to_string();
    name sAcct     = sender;
    asset aRetAmt  = supply;
    subdeposit(sAcct, aRetAmt, sMemo);

    // standard row update
    uint64_t nRetCount = getglobalint(name("retirecertn"));
    data_index _data_table( get_self(), GLOBAL_COUNT_SNK_ISS.value);
    auto data_itr = _data_table.find(nRetCount);

    check(data_itr != _data_table.end(), "Contract error, csink id  does not exist in _retirefunds! ");

    struct_data cData = data_itr->d;

    check(cData.header.status == STATUS_DATA_ADMIN_APPROVED, "Contract error, csink id was not approved by administrator.  Must be approved before tokens are retired. ");
    
    uint64_t n_prod_certn = cData.get_var_as_int("n_prod_certn");
    uint64_t n_port_certn = cData.get_var_as_int("n_port_certn");
    string s_loc          = cData.get_var("s_loc");
    string s_type         = cData.get_var("s_type");
    string s_desc         = cData.get_var("s_desc");
    asset a_gross         = cData.get_var_as_asset("a_gross");
    asset a_humidity      = cData.get_var_as_asset("a_humidity");
    asset a_tmin          = cData.get_var_as_asset("a_tmin");
    asset a_tmax          = cData.get_var_as_asset("a_tmax");
    asset a_tavg          = cData.get_var_as_asset("a_tavg");        //<--- CSink is issued off of this metric.
    uint64_t n_ystart     = cData.get_var_as_int("n_ystart");
    uint64_t n_yend       = cData.get_var_as_int("n_yend");
    uint64_t n_issued     = cData.get_var_as_int("n_issued");
    uint64_t n_claimed    = cData.get_var_as_int("n_claimed");
    asset a_qtyretired    = cData.get_var_as_asset("a_qtyretired");
    uint64_t n_retired    = cData.get_var_as_int("n_retired");

    //check(n_retired == 0, "Contract error, this certificate claims to have been retired already. ");
    //check(n_claimed == 1, "Contract error, this csink has not been claimed. ");
    check(n_issued == 1, "Contract error, csink id has not yet been issued. ");

    int16_t nIndex    = 0;
    asset aRetRemain  = supply;
    asset aCertRemain;

    while((aRetRemain.amount > 0) && (nIndex < 30)) {
        cData        = data_itr->d;
        a_tavg       = cData.get_var_as_asset("a_tavg");
        a_qtyretired = cData.get_var_as_asset("a_qtyretired");

        aCertRemain  = asset(a_tavg.amount - a_qtyretired.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));

        if(aCertRemain.amount > aRetRemain.amount) {  //increase a_qtyretired, but do not mark as retired cert nor increment retired count
            aCertRemain.amount = aCertRemain.amount - aRetRemain.amount;
            aRetRemain.amount = 0;
        } else {
            aRetRemain.amount = aRetRemain.amount - aCertRemain.amount;
            aCertRemain.amount = 0;
        }

        //update variables
        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {
            row.d.set_var("n_claimed","1");
        });

        nIndex++;
        data_itr = _data_table.find(nRetCount + nIndex);
    }
}



void carboncert::retiredep(const name& sender, const asset& supply, const uint64_t& orgid) { 
    
    int64_t nAmtToRetire = supply.amount;
    symbol_code cUnit = supply.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = supply.symbol.precision();

    asset aAmtToRetire = asset(nAmtToRetire, symbol(cUnit, 4));

    check(nAmtToRetire > 0, "E-t152 - Certificate must have a positive supply value. ");
    check(sUnit == getglobalstr(name("tokensymbol")), "E-t153 - Symbol specified is incorrect for this contract. ");
    check(nPrec == getglobalint(name("tokenprec")), "E-t154 - Issue precision of token is mis-match with contract precision.  ");

    //get globals
    uint64_t certcount   = getglobalint(name("certcount"));
    uint64_t retirecount = getglobalint(name("retirecount"));

    check(retirecount <= certcount, "E-t155 - Unable to process transaction (contract error retirecount <= certcount.) ");

    certs_index _certs( get_self(), get_self().value );
    auto cert_itr = _certs.find(retirecount);

    int64_t nSupply = cert_itr->supply.amount;
    int64_t nRetired = cert_itr->qtyretired.amount;

    check(nRetired < nSupply, "E-t156 - Unable to process transaction, no remaining amount to retire on certn (" + to_string(retirecount) + "). ");

    int64_t nPriorAmt = 0;
    uint8_t nIndex = 0;

    vector<carboncert::retiredcert>    retCerts;
    carboncert::retiredcert retCert;

    while((aAmtToRetire.amount > 0) && (nIndex < 20)) {
        retCert.certnum = cert_itr->certnum;
        retCert.certid = cert_itr->certid;
        
        nPriorAmt = aAmtToRetire.amount;

        //check if exceeds certcount
        check((retirecount + nIndex) <= certcount, "E-t192 - Unable to retire such a large supply, choose to retire less credits. ");

        aAmtToRetire = retireamount((retirecount + nIndex), aAmtToRetire);
        retCert.qtyretired = asset(nPriorAmt - aAmtToRetire.amount, symbol(cUnit, 4));

        retCerts.push_back(retCert);

        nIndex++;
        cert_itr = _certs.find(retirecount + nIndex);
    }

    check(nIndex < 50, "E-t51 - Unable to retire such a large supply, choose to retire less credits. ");

    //add entry to: typedef multi_index<name("retired"), retirements> retire_index;
    uint64_t nRetCt = getglobalint(name("retiredtblct")) + 1; 
    retire_index _retired( get_self(), get_self().value );
    auto ret_itr = _retired.find(nRetCt);

    check(ret_itr == _retired.end(), "E-t162 - Value error with retiredtblct global row already exists. ");

    _retired.emplace( get_self(), [&]( auto& ret_row ) {
        ret_row.retirenum = nRetCt;
        ret_row.orgid = getorgid(sender);
        ret_row.account = sender;
        ret_row.supplyret = supply;
        ret_row.certs = retCerts;
        ret_row.retiredate = time_point_sec(current_time_point().sec_since_epoch());
    });

    setglobalint(name("retiredtblct"), nRetCt);

    string sMemo = "Token retirement from " + get_self().to_string();
    
    action(
            permission_level{ get_self(), "active"_n},
            getcontract(),
            "transfer"_n,
            std::make_tuple(
                get_self(),
                getcontract(),
                supply,
                sMemo
            )
        ).send();
    
    //update typedef multi_index<name("retirestat"), certstats> retirestat_index;
    retirestat(orgid, supply);
}*/

void carboncert::_execute(const name& approver, const name& type, const uint64_t& id) {

    //execute updates status and cements the cert

    name scope = type;
    name countvar = name_null;
    vector<string> verify = {};
    
    if(type.value == DATA_TYPE_CERT_EBC.value) {
        countvar = GLOBAL_COUNT_EBC;
        verify = VARDEF_CERT_EBC;
    } else if (type.value == DATA_TYPE_CERT_PRO.value) {
        countvar = GLOBAL_COUNT_PRO;
        verify = VARDEF_CERT_PROD;
    } else if (type.value == DATA_TYPE_CERT_SNK.value) {
        countvar = GLOBAL_COUNT_SNK;
        verify = VARDEF_CERT_CSNK;
    } else if (type.value == DATA_TYPE_PORTF.value) {
        countvar = GLOBAL_COUNT_PRT;
        verify = VARDEF_DATA_PORTF;
    } else if (type.value == DATA_TYPE_ACT_SEND.value) {
        countvar = GLOBAL_COUNT_SND;
        verify = VARDEF_ACT_SEND;
    } else { check(false, "Specified type for draft is invalid. "); }

    uint64_t count = id;
    string sMemo   = "";

    data_index _data_table( get_self(), scope.value );
    auto data_itr = _data_table.find(count);

    //check for draftcert auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    struct_approval cApprove = struct_approval(approver, auth, STATUS_DATA_EXECUTED, auth_org);

    //test if record exists
    if(data_itr == _data_table.end()){
        check(false, "Supplied identification for data record does not exist. ");
    } else { //update record

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {

            row.d.is_data_valid(verify);

            if(type.value == DATA_TYPE_CERT_EBC.value) {
                check(row.d.header.status == STATUS_DATA_ADMIN_APPROVED, "You may only execute data when admin has approved. ");
            } else if (type.value == DATA_TYPE_CERT_PRO.value) {
                check(false, "Execution of production occurs when C-Sink happens. ");
            } else if (type.value == DATA_TYPE_CERT_SNK.value) {
                check(row.d.header.status == STATUS_DATA_ADMIN_APPROVED, "You may only execute data when admin has approved. ");
            } else if (type.value == DATA_TYPE_PORTF.value) {
                check(row.d.header.status == STATUS_DATA_ADMIN_APPROVED, "You may only execute data when admin has approved. ");
            } else if (type.value == DATA_TYPE_ACT_SEND.value) {
                check(row.d.header.status == STATUS_DATA_ADMIN_APPROVED, "You may only execute data when admin has approved. ");
            } else { check(false, "Specified type for draft is invalid. "); }

            row.d.header.executed(true);

            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was executed by " + approver.to_string() + " (strid: " + row.d.header.strid + ") ";
            print(sMemo);
        });
    }
    
    if (type.value == DATA_TYPE_CERT_SNK.value) {
        
        //update production cert for cSink
        _csink_apply_prod(id);

        //generate new CSINK_ISS
        name countvar_ci = GLOBAL_COUNT_SNKI;
        vector<string> verify_ci = VARDEF_CERT_CSNK_ISS;

        uint64_t count_ci = getglobalint(countvar_ci) + 1;
    
        data_index _data_table_ci( get_self(), DATA_TYPE_CERT_SNKI.value );
        auto data_itr_ci = _data_table_ci.find(count_ci);

        struct_data sData_CI = data_itr->d;

        _data_table_ci.emplace( get_self(), [&]( auto& row ) {
            row.id = count_ci;
            row.d  = sData_CI;

            row.d.is_data_valid(verify_ci);
        
            sMemo = "C-Sink " + DATA_TYPE_CERT_SNKI.to_string() + " #" + to_string(count_ci) + "  was finalized and issued by " + approver.to_string() + " (strid: " + row.d.header.strid + ")";
            print(sMemo);
        });

        setglobalint(countvar_ci, count_ci);
    
    } else if(type.value == DATA_TYPE_ACT_SEND.value) {

        struct_data cData = data_itr->d;
        _check_send_from(approver, name(cData.get_var("s_from")));
        
        //transfer
        action(
            permission_level{ getcontract(), "active"_n},
            getcontract(),
            "transfer"_n,
            std::make_tuple(
                name(cData.get_var("s_from")),
                name(cData.get_var("s_to")),
                cData.get_var_as_asset("a_qty"),
                cData.get_var("s_memo")
            )
        ).send();
    } else {
    }

    //add functions to datamgr for csink ---   .csink_issue()  and for producer   .csink_issue() verison
}



void carboncert::_claim(const name& approver, const name& type, const uint64_t& id) {

    //execute updates status and cements the cert

    name scope = type;
    name countvar = name_null;
    vector<string> verify = {};
    
    if (type.value == DATA_TYPE_CERT_SNK.value) {
        countvar = GLOBAL_COUNT_SNK;
        verify = VARDEF_CERT_CSNK;
    } else { check(false, "Specified type for draft is invalid. "); }

    uint64_t count = id;
    string sMemo   = "";

    data_index _data_table( get_self(), scope.value );
    auto data_itr = _data_table.find(count);

    //check for draftcert auth
    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    //test if record exists
    if(data_itr == _data_table.end()){
        check(false, "Supplied identification for data record does not exist. ");
    } else { //update record

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {

            row.d.is_data_valid(verify);

            if (type.value == DATA_TYPE_CERT_SNK.value) {
                check(row.d.header.status == STATUS_DATA_EXECUTED, "You may only claim after csink is executed. ");
            } else { check(false, "Specified type for draft is invalid. "); }

            row.d.set_var("n_claimed","1");

            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was claimed by " + approver.to_string() + " (strid: " + row.d.header.strid + ") ";
            print(sMemo);
        });
    }

    struct_data cCsink = data_itr->d;
    asset aCSink = cCsink.get_var_as_asset("a_gross");
    asset aTokenIssue = asset(aCSink.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));

    sMemo = sMemo + " Claimed: " + aTokenIssue.to_string();

    //transfer
    action(
        permission_level{ getcontract(), "active"_n},
        getcontract(),
        "transfer"_n,
        std::make_tuple(
            getcontract(),
            approver,
            aTokenIssue,
            sMemo
        )
    ).send();
}


//send token to deposit in this contract
void carboncert::_retire(const name& approver, const asset& quant) {

    bool bAllRetired = false;

    name countvar         = name_null;
    vector<string> verify = {};
   
    countvar = GLOBAL_COUNT_SNKI;
    verify   = VARDEF_CERT_CSNK_ISS;

    uint64_t count = getglobalint(countvar);

    data_index _data_table( get_self(), DATA_TYPE_CERT_SNKI.value );
    auto data_itr = _data_table.find(count);

    //while loop to reduce DATA_TYPE_CERT_SNKI
    asset aRetireRemain = quant;
    string sData_Ret = "";

    while(!bAllRetired) {
        //decrement csink amount

            asset aRetired;
            asset aRetiredNew;
            asset aAvg;
            int64_t nCertRemain;

            asset aCertRemain;
            int64_t nQtyRetired;

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {

            row.d.is_data_valid(verify);

            aRetired      = row.d.get_var_as_asset("a_qtyretired");
            aRetiredNew   = aRetired;
            aAvg          = row.d.get_var_as_asset("a_tavg");
            nCertRemain   = (aAvg.amount - aRetired.amount);
            nQtyRetired = 0;


            if((aRetireRemain.amount - nCertRemain) > 0) {
                nQtyRetired = nCertRemain;
                aCertRemain = asset(0, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                aRetiredNew = aAvg;
                row.d.set_var("n_retired","1");
                aRetireRemain = asset(aRetireRemain.amount - nCertRemain, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                count++;
                setglobalint(countvar, count);
                data_itr = _data_table.find(count);
            } else if ((aRetireRemain.amount - nCertRemain) == 0) {
                nQtyRetired = nCertRemain;
                aCertRemain = asset(0, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                aRetiredNew = aAvg;
                row.d.set_var("n_retired","1");
                aRetireRemain = asset(0, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                count++;
                setglobalint(countvar, count);
                data_itr = _data_table.find(count);
                bAllRetired = true;
            } else {
                nQtyRetired = aRetireRemain.amount;
                aCertRemain = asset(nCertRemain - aRetireRemain.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                aRetiredNew = asset(aRetired.amount + aRetireRemain.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                aRetireRemain = asset(0, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                bAllRetired = true;
            }

            row.d.set_var("a_qtyretired",aRetiredNew.to_string());

            sData_Ret = sData_Ret + to_string(row.id) + "," + asset(nQtyRetired, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec")))).to_string() + "," + row.d.get_var("n_prod_certn") + "*";
    
        });
    }

    //updates retirement count tracker
    setglobalint(countvar, count);

    sData_Ret = chop(sData_Ret);

    //emplace new retirement
        name countvar_ret         = name_null;
        vector<string> verify_ret = {};
    
        countvar_ret = GLOBAL_COUNT_RET;
        verify_ret   = VARDEF_ACT_RETIRE;

        uint64_t count_ret = getglobalint(countvar_ret) + 1;

        data_index _data_table_ret( get_self(), DATA_TYPE_ACT_RETR.value );
        auto data_itr_ret = _data_table_ret.find(count_ret);

        check(data_itr_ret != _data_table_ret.end(), "Retirement error, contact administrator. ");

        string sSTRID = "";
        string sDataRow = "a_retired|" + quant.to_string() + "|s_data|" + sData_Ret;
        string sToken = "|";

        uint64_t auth_org = get_org_id(approver);
        uint8_t auth = get_auth_by_org(approver, auth_org);

        _data_table_ret.emplace( get_self(), [&]( auto& row ) {
            row.id = count_ret;
            row.d  = struct_data(
                        struct_header(count_ret, sSTRID, DATA_TYPE_ACT_RETR, approver, auth_org, STATUS_DATA_EXECUTED),
                        sDataRow,
                        sToken
                   );

            row.d.is_data_valid(verify);
        });

        setglobalint(countvar_ret, count_ret);

    //destroy token from deposited funds
    name depositor = approver;
    asset reduce = quant;
    subdeposit(depositor, reduce);

    string sMemo = "Sent " + quant.to_string() + " to token contract (" + getcontract().to_string() + ") for retirement of credits. ";

    //send amount off to COXC to later retire
    action(
        permission_level{ getcontract(), "active"_n},
        getcontract(),
        "transfer"_n,
        std::make_tuple(
            get_self(),
            getcontract(),
            quant,
            sMemo
        )
    ).send();
}
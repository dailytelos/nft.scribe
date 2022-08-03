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
    bool bCerts = (type == DATA_TYPE_CERT_EBC) || (type == DATA_TYPE_CERT_PRO) || (type == DATA_TYPE_CERT_SNK) || (type == DATA_TYPE_CERT_SNKI) || (type == DATA_TYPE_PORTF);
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


ACTION carboncert::zzdeleterow(const name& type, uint64_t& id) {

    require_auth(get_self());

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
    } else if (type.value == DATA_TYPE_CERT_SNKI.value) {
        countvar = GLOBAL_COUNT_SNKI;
        verify = VARDEF_CERT_CSNK_ISS;
    } else if (type.value == DATA_TYPE_PORTF.value) {
        countvar = GLOBAL_COUNT_PRT;
        verify = VARDEF_DATA_PORTF;
    } else if (type.value == DATA_TYPE_ACT_SEND.value) {
        countvar = GLOBAL_COUNT_SND;
        verify = VARDEF_ACT_SEND;
    } else if (type.value == DATA_TYPE_ACT_RETR.value) {
        countvar = GLOBAL_COUNT_RET;
        verify = VARDEF_ACT_RETIRE;
    } else { check(false, "Specified type for draft is invalid. "); }

    uint64_t count = id;
    string sMemo   = "";

    data_index _data_table( get_self(), scope.value );
    auto data_itr = _data_table.find(count);

    //emplace new data
    if(data_itr != _data_table.end()){

        _data_table.erase(data_itr);

    }  else {
        check(false, "Failed to erase data row. ");
    }
}


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

    uint64_t count = (edit == 1) ? id : getglobalint(countvar) - 1;
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
                        struct_header(count, strid, type, creator, auth_org, 0, STATUS_NONE),
                        data,
                        token
                   );

            row.d.is_data_valid(verify);

            //other verifications
            if(type.value == DATA_TYPE_CERT_PRO.value) {
                //grab EBC data
                uint64_t nEBC_num    = row.d.get_var_as_uint("u_ebc_certn");

                struct_data cDataEBC = _get_data_by_id(DATA_TYPE_CERT_EBC, nEBC_num);

                check(cDataEBC.header.status >= STATUS_DATA_ADMIN_APPROVED, "The EBC Cert has not yet been given final approval. ");
            } else if (type.value == DATA_TYPE_CERT_SNK.value) {
                //grab Producer data
                uint64_t nPROD_Num    = row.d.get_var_as_uint("u_prod_certn");
                struct_data cDataPROD = _get_data_by_id(DATA_TYPE_CERT_PRO, nPROD_Num);

                row.d.header.reforgid = cDataPROD.header.orgid;

                check(cDataPROD.header.status >= STATUS_DATA_ADMIN_APPROVED, "The production declaration has not yet been given final approval. ");
            }

            row.d.header.draft(true);
        
            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was drafted by " + creator.to_string() + " (strid: " + strid + ")";
            print(sMemo);
        });

        setglobalint(countvar, count);
    } else { //update record
        check(data_itr->d.header.status < STATUS_DATA_CORP_APPROVED, "You cannot edit data that was already approved. ");

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {

            row.d.check_org_match(auth_org);

            row.id = count;
            row.d  = struct_data(
                        struct_header(count, strid, type, creator, auth_org, 0, STATUS_NONE),
                        data,
                        token
                   );

            row.d.is_data_valid(verify);

            //other verifications
            if(type.value == DATA_TYPE_CERT_PRO.value) {
                //grab EBC data
                uint64_t nEBC_num    = row.d.get_var_as_uint("u_ebc_certn");
                struct_data cDataEBC = _get_data_by_id(DATA_TYPE_CERT_EBC, nEBC_num);

                check(cDataEBC.header.status >= STATUS_DATA_ADMIN_APPROVED, "The EBC Cert has not yet been given final approval. ");
            } else if (type.value == DATA_TYPE_CERT_SNK.value) {
                //grab Producer data
                uint64_t nPROD_Num    = row.d.get_var_as_uint("u_prod_certn");
                struct_data cDataPROD = _get_data_by_id(DATA_TYPE_CERT_PRO, nPROD_Num);

                row.d.header.reforgid = cDataPROD.header.orgid;

                check(cDataPROD.header.status >= STATUS_DATA_ADMIN_APPROVED, "The production declaration has not yet been given final approval. ");
            }

            row.d.header.draft(true);
        
            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was re-drafted by " + creator.to_string() + " (strid: " + strid + ")";
            print(sMemo);
        });
    }

    if(type == DATA_TYPE_ACT_SEND) {
        struct_data cData_SEND = data_itr->d;
        _check_send_from(creator, name(cData_SEND.get_var("s_from")));
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

        struct_data cData;
        struct_data cDataEBC;
        struct_data cDataPROD;
        asset a_csink_pers, a_csink_gross, a_csink_net;

        //load up ebc cert data if dealing with a CSink Submission
        if (type.value == DATA_TYPE_CERT_SNK.value) {
            cData = data_itr->d;

            //grab EBC data
            uint64_t nEBC_Num = (uint64_t) cData.get_var_as_uint("u_ebc_certn");
            cDataEBC = _get_data_by_id(DATA_TYPE_CERT_EBC, nEBC_Num);

            //grab Producer data
            uint64_t nPROD_Num = (uint64_t) cData.get_var_as_uint("u_prod_certn");
            cDataPROD = _get_data_by_id(DATA_TYPE_CERT_PRO, nPROD_Num);
        }


        check(data_itr->d.header.status <= STATUS_DATA_ADMIN_APPROVED, "You cannot submit data that was already fully approved. ");

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {

            row.d.check_org_match(auth_org);
            
            row.d.is_data_valid(verify);

            if (type.value == DATA_TYPE_CERT_EBC.value) {
                asset a_csink_gross = row.d.get_var_as_asset("a_csink_gross"); //"a_csink_gross","a_csink_net","a_csink_pers"
                asset a_csink_net = row.d.get_var_as_asset("a_csink_net");
                asset a_csink_pers = row.d.get_var_as_asset("a_csink_pers");

                check(a_csink_gross.symbol.code().to_string() == "T", "Invalid symbol supplied for 'a_csink_gross', must be 'T'.  ");
                check(a_csink_net.symbol.code().to_string() == "T", "Invalid symbol supplied for 'a_csink_net', must be 'T'.  ");
                check(a_csink_pers.symbol.code().to_string() == "T", "Invalid symbol supplied for 'a_csink_pers', must be 'T'.  ");

                //set active ebc certificate
                if(appr_type == "admin_approve") {
                    _setorgcert(row.d.header.orgid, id);
                }

            } else if (type.value == DATA_TYPE_CERT_PRO.value) {
                //validate EBC Cert # is same company as production

                //check asset symbols
                asset a_tproduced = row.d.get_var_as_asset("a_tproduced");
                check(a_tproduced.symbol.code().to_string() == "T", "Invalid symbol supplied for 'a_tproduced', must be 'T'.  ");

                //set defaults
                row.d.set_var("a_tcsunk","0.0000 T");
                row.d.set_var("a_tissued","0.0000 "+getglobalstr(name("tokensymbol")));
                
            } else if (type.value == DATA_TYPE_CERT_SNK.value) {

                //verify remaining production available to c-sink
                _csink_check_prod(id);

                //{"u_prod_certn","u_ebc_certn","n_dbid","s_loc","s_type","s_application","a_gross","a_tmin","a_tmax","a_tavg","n_ystart","n_yend","n_claimed","a_qtyretired","n_retired"};
                //grab u_ebc_certn from Production directly, don't trust user input for u_ebc_certn
                row.d.set_var("u_ebc_certn", cDataPROD.get_var("u_ebc_certn"));

                //validate numbers 
                check(row.d.get_var_as_int("n_ystart") > 2020, "Start year must be greater than 2020. ");

                //derive n_yend based on start year + 100
                int64_t n_yend = row.d.get_var_as_int("n_ystart") + 100;
                row.d.set_var("n_yend",to_string(n_yend));

                //check asset symbols
                asset a_gross = row.d.get_var_as_asset("a_gross");
                check(a_gross.symbol.code().to_string() == "T", "Invalid symbol supplied for 'a_gross', must be 'T'.  ");

                //set defaults
                row.d.set_var("n_claimed","0");
                row.d.set_var("a_qtyretired",asset(0, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec")))).to_string());
                row.d.set_var("n_retired","0");

                //set min, max, avg based on what was supplied to EBC
                //first must have EBC data from EBC Cert
                asset a_csink_pers_2 = cDataEBC.get_var_as_asset("a_csink_pers");
                asset a_csink_gross_2 = cDataEBC.get_var_as_asset("a_csink_gross");
                asset a_csink_net_2 = cDataEBC.get_var_as_asset("a_csink_net");
                asset a_tmin = asset((int64_t) (a_csink_pers_2.amount * a_gross.amount) / 10000, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                asset a_tmax = asset((int64_t) (a_csink_gross_2.amount * a_gross.amount) / 10000, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                asset a_tavg = asset((int64_t)  ((((a_csink_net_2.amount - a_csink_pers_2.amount)/2) + a_csink_pers_2.amount) * a_gross.amount) / 10000, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                
                row.d.set_var("a_tmin", a_tmin.to_string()); // "a_csink_pers"
                row.d.set_var("a_tmax", a_tmax.to_string()); // "a_csink_gross"
                row.d.set_var("a_tavg", a_tavg.to_string()); // (("a_csink_net" - "a_csink_pers") / 2) + "a_csink_pers"

            } else if (type.value == DATA_TYPE_PORTF.value) {
                row.d.set_var("a_csinks","0.0000 T");
                row.d.set_var("a_retired","0.0000 T");
            }


            //other verifications - duplicated in draft
            if(type.value == DATA_TYPE_CERT_PRO.value) {
                //grab EBC data
                uint64_t nEBC_num    = row.d.get_var_as_uint("u_ebc_certn");
                struct_data cDataEBC = _get_data_by_id(DATA_TYPE_CERT_EBC, nEBC_num);

                check(cDataEBC.header.status >= STATUS_DATA_ADMIN_APPROVED, "The EBC Cert has not yet been given final approval. ");
            } else if (type.value == DATA_TYPE_CERT_SNK.value) {
                //grab Producer data
                uint64_t nPROD_Num    = row.d.get_var_as_uint("u_prod_certn");
                struct_data cDataPROD = _get_data_by_id(DATA_TYPE_CERT_PRO, nPROD_Num);

                row.d.header.reforgid = cDataPROD.header.orgid;

                check(cDataPROD.header.status >= STATUS_DATA_ADMIN_APPROVED, "The production declaration has not yet been given final approval. ");
            }

            if(appr_type == "submit") {
                row.d.header.submit(true);

                if(type.value == DATA_TYPE_CERT_SNK.value)  {
                    row.d.header.corp_approve(true); //for C-Sink Submission, it is auto-approved at the corporate level, we kick it to the admin for final approval
                }
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
        struct_data cData_SEND = data_itr->d;
        _check_send_from(approver, name(cData_SEND.get_var("s_from")));
    }
}


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

            if (type.value == DATA_TYPE_CERT_SNK.value) {
                row.d.check_reforg_match(auth_org); //producer claims the token, this is reforg
            } else {
                row.d.check_org_match(auth_org);
            }
            
            row.d.is_data_valid(verify);

            if(type.value == DATA_TYPE_CERT_EBC.value) {
                check(false, "Execution of ebc occurs when C-Sink happens. ");
            } else if (type.value == DATA_TYPE_CERT_PRO.value) {
                check(false, "Execution of production occurs when C-Sink happens. ");
            } else if (type.value == DATA_TYPE_CERT_SNK.value) {
                check(row.d.header.status == STATUS_DATA_ADMIN_APPROVED, "Data certificate has wrong status code to perform this action. ");
            } else if (type.value == DATA_TYPE_PORTF.value) {
                check(row.d.header.status == STATUS_DATA_ADMIN_APPROVED, "Data certificate has wrong status code to perform this action. ");
            } else if (type.value == DATA_TYPE_ACT_SEND.value) {
                check(row.d.header.status == STATUS_DATA_ADMIN_APPROVED, "Data certificate has wrong status code to perform this action. ");
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

        uint64_t count_ci = getglobalint(countvar_ci) - 1;
    
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

    name receiver = approver;

    //test if record exists
    if(data_itr == _data_table.end()){
        check(false, "Supplied identification for data record does not exist. ");
    } else { //update record

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {

            row.d.check_reforg_match(auth_org); //producer claims the token, this is reforg

            row.d.is_data_valid(verify);

            if (type.value == DATA_TYPE_CERT_SNK.value) {
                check(row.d.header.status == STATUS_DATA_EXECUTED, "You may only claim after csink is executed. ");
            } else { check(false, "Specified type for draft is invalid. "); }

            //update claimed = 1
            check(row.d.get_var_as_int("n_claimed") == 0, "This C-Sink already has  claimed flag set as true. ");
            row.d.set_var("n_claimed","1");

            if(auth_org < 100) {
                receiver = row.d.header.creator;
            }

            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was claimed by " + approver.to_string() + " (strid: " + row.d.header.strid + ") ";
            print(sMemo);
        });
    }

    struct_data cCsink = data_itr->d;
    asset aCSink = cCsink.get_var_as_asset("a_tavg");
    asset aTokenIssue = asset(aCSink.amount, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));

    sMemo = sMemo + " Claimed: " + aTokenIssue.to_string();

    //transfer
    action(
        permission_level{ getcontract(), "active"_n},
        getcontract(),
        "transfer"_n,
        std::make_tuple(
            getcontract(),
            receiver,
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
   
    countvar = GLOBAL_COUNT_RETTAIL;
    verify   = VARDEF_CERT_CSNK_ISS;

    uint64_t count = getglobalint(countvar);

    data_index _data_table( get_self(), DATA_TYPE_CERT_SNKI.value );
    auto data_itr = _data_table.find(count);

    //while loop to reduce DATA_TYPE_CERT_SNKI
    asset aRetireRemain = quant;
    string sData_Ret = "";

    uint64_t auth_org = get_org_id(approver);
    uint8_t auth = get_auth_by_org(approver, auth_org);

    uint8_t limit_counter = 0;

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
                count--;
                setglobalint(countvar, count);
                data_itr = _data_table.find(count);
            } else if ((aRetireRemain.amount - nCertRemain) == 0) {
                nQtyRetired = nCertRemain;
                aCertRemain = asset(0, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                aRetiredNew = aAvg;
                row.d.set_var("n_retired","1");
                aRetireRemain = asset(0, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec"))));
                count--;
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

            sData_Ret = sData_Ret + to_string(row.id) + "," + asset(nQtyRetired, symbol(symbol_code(getglobalstr(name("tokensymbol"))), (uint8_t) getglobalint(name("tokenprec")))).to_string() + "," + row.d.get_var("u_prod_certn") + "," + row.d.get_var("u_ebc_certn") + "*";
    
        });

        limit_counter++;

        if(limit_counter >= 40) {
            check(false, "The current que of C-Sinks to retire contains many small balances.  The maximum limit of 40 CSinks has been hit.  Try retiring a smaller value to properly execute this retirement. ");
        }
    }

    //updates retirement count tracker
    setglobalint(countvar, count);

    sData_Ret = chop(sData_Ret);

    //emplace new retirement
        name countvar_ret         = name_null;
        vector<string> verify_ret = {};
    
        countvar_ret = GLOBAL_COUNT_RET;
        verify_ret   = VARDEF_ACT_RETIRE;

        uint64_t count_ret = getglobalint(countvar_ret) - 1;

        data_index _data_table_ret( get_self(), DATA_TYPE_ACT_RETR.value );
        auto data_itr_ret = _data_table_ret.find(count_ret);

        check(data_itr_ret == _data_table_ret.end(), "Retirement error, contact administrator. ");

        string sSTRID = "";
        string sDataRow = "a_retired|" + quant.to_string() + "|s_data|" + sData_Ret;
        string sToken = "|";

        _data_table_ret.emplace( get_self(), [&]( auto& row ) {
            row.id = count_ret;
            row.d  = struct_data(
                        struct_header(count_ret, sSTRID, DATA_TYPE_ACT_RETR, approver, auth_org, 0, STATUS_DATA_CORP_APPROVED),
                        sDataRow,
                        sToken
                   );

            row.d.header.admin_approve(true);
            row.d.header.executed(true);

            row.d.is_data_valid(verify_ret);
        });

        setglobalint(countvar_ret, count_ret);

    //destroy token from deposited funds
    name depositor = approver;
    asset reduce = quant;
    subdeposit(depositor, reduce);

    string sMemo = "Sent " + quant.to_string() + " to token contract (" + getcontract().to_string() + ") for retirement of credits. ";

    //send amount off to COXC to later retire
    action(
        permission_level{ get_self(), "active"_n},
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


carboncert::struct_data carboncert::_get_data_by_id(const name& type, uint64_t& id) {

    struct_data cData;

    name scope = type;
    uint64_t count = id;

    data_index _data_table( get_self(), scope.value );
    auto data_itr = _data_table.find(count);

    //test if record exists
    if(data_itr == _data_table.end()){
        check(false, "Supplied identification for data record does not exist in _get_data_by_id(" + type.to_string() + ", " + to_string(id) + "). ");
    } else { //return record

        return data_itr->d;
    }

    return cData;
}
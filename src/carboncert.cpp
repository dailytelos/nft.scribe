#include <carboncert.hpp>

#include "./manage.cpp"
#include "./auths.cpp"


ACTION carboncert::datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id) {
    checkfreeze();
    require_auth(creator);

    //check for draftcert auth
    uint64_t auth_org = get_org_id(creator);
    uint8_t auth = get_org_auth(creator, auth_org);

    //validate authority
    bool bCerts = (type == DATA_TYPE_CERT_EBC) || (type == DATA_TYPE_CERT_PRO) || (type == DATA_TYPE_CERT_SNK) || (type == DATA_TYPE_PORTF);
    bool bSend  = (type == DATA_TYPE_ACT_SEND);

    if(bCerts) {
        check( (auth == AUTH_LEVEL_CORP_CERTS) ||
            (auth == AUTH_LEVEL_CORP_APPROVE) ||
            (auth == AUTH_LEVEL_CORP_ADMIN) ||
            (auth == AUTH_ADMIN_CERTS) ||
            (auth == AUTH_ADMIN_APPROVALS) ||
            (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
    } else if (bSend) {
        check( (auth == AUTH_LEVEL_CORP_SEND) ||
            (auth == AUTH_LEVEL_CORP_APPROVE) ||
            (auth == AUTH_LEVEL_CORP_ADMIN) ||
            (auth == AUTH_ADMIN_SEND) ||
            (auth == AUTH_ADMIN_APPROVALS) ||
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
    uint8_t auth = get_org_auth(approver, auth_org);

    //validate authority
    bool bCerts = (type == DATA_TYPE_CERT_EBC) || (type == DATA_TYPE_CERT_PRO) || (type == DATA_TYPE_CERT_SNK) || (type == DATA_TYPE_PORTF);
    bool bSend  = (type == DATA_TYPE_ACT_SEND);

    if(bCerts) {
        check( (auth == AUTH_LEVEL_CORP_CERTS) ||
            (auth == AUTH_LEVEL_CORP_APPROVE) ||
            (auth == AUTH_LEVEL_CORP_ADMIN) ||
            (auth == AUTH_ADMIN_CERTS) ||
            (auth == AUTH_ADMIN_APPROVALS) ||
            (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
    } else if (bSend) {
        check( (auth == AUTH_LEVEL_CORP_SEND) ||
            (auth == AUTH_LEVEL_CORP_APPROVE) ||
            (auth == AUTH_LEVEL_CORP_ADMIN) ||
            (auth == AUTH_ADMIN_SEND) ||
            (auth == AUTH_ADMIN_APPROVALS) ||
            (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");
    } else {
        check(false, "Invalid activity specified. ");
    }

    _datasubmit(approver, type, id, appr_type);
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
    } else { check(false, "Specified type for draft is invalid. "); }

    uint64_t count = (edit == 1) ? id : getglobalint(countvar) + 1;
    string sMemo = "";

    data_index _data_table( get_self(), scope.value );
    auto data_itr = _data_table.find(count);

    tuple<name, uint8_t, uint64_t> tplApr = get_auth_row(creator);
    strctapproval cApprove = strctapproval(creator, get<1>(tplApr), get_status_auth(creator, type, false, false), get<2>(tplApr), time_point_sec(current_time_point().sec_since_epoch()));
    
    //update existing data
    if(data_itr == _data_table.end()){
        _data_table.emplace( get_self(), [&]( auto& row ) {
            row.id = count;
            row.d  = struct_data(
                        strctheader(count, strid, type, creator, get_org_id(creator), STATUS_NONE),
                        data,
                        token
                   );

            row.d.header.add_approval(cApprove, "draft");

            row.d.is_data_valid(verify);
        
            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was drafted by " + creator.to_string() + " (strid: " + strid + ")";
            print(sMemo);
        });

        setglobalint(countvar, count);
    } else { //emplace new data
        check(data_itr->d.header.status < STATUS_CERT_CORP_APPROVED, "You cannot edit data that was already approved. ");

        _data_table.modify( data_itr, get_self(), [&]( auto& row ) {
            row.d       = struct_data(
                                strctheader(count, strid, type, creator, get_org_id(creator), STATUS_NONE),
                                data,
                                token
                        );

            row.d.header.add_approval(cApprove, "draft");

            row.d.is_data_valid(verify);
        
            sMemo = "Data type " + type.to_string() + " #" + to_string(count) + "  was re-drafted by " + creator.to_string() + " (strid: " + strid + ")";
            print(sMemo);
        });
    }
}

void carboncert::_datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type) {

    check(appr_type != "draft", "Invalid appr_type supplied draft to _datasubmit. ");

    data_index _data_table( get_self(), type.value);
    auto data_itr = _data_table.find(id);

    check(data_itr != _data_table.end(), "Contract error, id number does not exist! ");

    tuple<name, uint8_t, uint64_t> tplApr = get_auth_row(approver);
    strctapproval cApprove = strctapproval(approver, get<1>(tplApr), get_status_auth(approver, type, true, (appr_type == "approve")), get<2>(tplApr), time_point_sec(current_time_point().sec_since_epoch()));

    _data_table.modify( data_itr, get_self(), [&]( auto& row ) {
        row.d.header.add_approval(cApprove, appr_type);
    });
}

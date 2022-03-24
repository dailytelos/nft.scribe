// a vector of these stores the approvals for a system activity
struct strctapproval {
    name approver;
    uint8_t level;         //records current level of user approver
    uint8_t status;        //status update applied by approver
    uint64_t orgid;        //organisation id of approval
    time_point_sec tstamp; //time of approval

    strctapproval() {
        approver    = name("name.nullaaaa");
        level       = 0;
        status      = 0;
        orgid       = 0;
        tstamp      = time_point_sec(0);
    };

    strctapproval(name i_approver, uint8_t i_level, uint8_t i_status, uint64_t i_orgid, time_point_sec i_tstamp) {
        approver    = i_approver;
        level       = i_level;
        status      = i_status;
        orgid       = i_orgid;
        tstamp      = i_tstamp;
    };

    strctapproval(tuple<eosio::name, uint8_t, uint8_t, uint64_t> &tplApprove) {
        approver   = get<0>(tplApprove);
        level      = get<1>(tplApprove);
        status     = get<2>(tplApprove);
        orgid      = get<3>(tplApprove);
        tstamp     = time_point_sec(current_time_point().sec_since_epoch());
    };

    EOSLIB_SERIALIZE(strctapproval, (approver)(level)(status)(orgid)(tstamp));
};

// header structure for primary row data for certificates and also system send funds requests
struct strctheader {

    uint64_t  id;       //numeric id of activity

    string    strid;   //full string id, limit to 256 characters, (contain numeric id inside strid)

    name      type;     //type of table object: "production", "bfcert", "send"
    name      creator;  //account that drafted the table object
    uint64_t  orgid;    //orgid to which creator belongs
    uint8_t   status;   //value that denotes status of activity: STATUS_SEND_DRAFT, STATUS_CERT_DRAFT, etc.

    time_point_sec draftdate;  //when drafted in the system
    time_point_sec submitdate; //when submitted in the system
    time_point_sec approvedate; //when cert becomes active due to final approval / authorisation
    time_point_sec deletedate; //when it was deleted
    time_point_sec lockdate;  //when it was locked

    vector<carboncert::strctapproval> approvals;  //records approvals as they occur

    strctheader(){};

    strctheader(uint64_t i_id, string i_strid, name i_type, name i_creator, uint64_t i_orgid, uint8_t i_status) {
        id        = i_id;
        strid     = i_strid;
        type      = i_type;
        creator   = i_creator;
        orgid     = i_orgid;
        status    = i_status;
        draftdate = time_point_sec(current_time_point().sec_since_epoch());
    };

    strctheader(uint64_t i_id, string i_strid, name i_type, name i_creator, uint64_t i_orgid, uint8_t i_status, time_point_sec i_draftdate) {
        id        = i_id;
        strid     = i_strid;
        type      = i_type;
        creator   = i_creator;
        orgid     = i_orgid;
        status    = i_status;
        draftdate = i_draftdate;
    };

    void add_approval(carboncert::strctapproval &approval, string appr_type) {

        uint8_t nStatus = approval.status;

            if(appr_type == "draft") {
                nStatus = 40;
                draftdate = time_point_sec(current_time_point().sec_since_epoch());
            } else if (appr_type == "submit") {
                nStatus = 41;
                submitdate = time_point_sec(current_time_point().sec_since_epoch());
            } else if (appr_type == "approve") {
                approvedate = time_point_sec(current_time_point().sec_since_epoch());
            } else if (appr_type == "del.status") {
                deletedate = time_point_sec(current_time_point().sec_since_epoch());
            } else {
                check(false, "Invalid appr_type in add_approval. ");
            }

        if(approval.status < 200) {
            check(approval.orgid == orgid, "Approval from orgid does not match header data orgid. ");
            
            if (appr_type == "approve") {
                nStatus = 61;
            }
        }

        if(status == 4) {
            check(false, "Entries flagged for deletion may not be updated further. ");
        }

        check(nStatus > status, "Approval is identical or less than current authorisation. ");

        approvals.push_back(approval);
        status = nStatus;
    };

    EOSLIB_SERIALIZE(strctheader, (id)(strid)(type)(creator)(orgid)(status)(draftdate)(submitdate)(approvedate)(deletedate)(lockdate)(approvals));
};
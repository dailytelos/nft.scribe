// a vector of these stores the approvals for a system activity
struct strctapproval {
    name approver;
    uint8_t level;         //records current level of user approver
    uint64_t orgid;        //organisation id of approval
    time_point_sec tstamp; //time of approval

    strctapproval() {
        approver    = name("name.nullaaaa");
        level       = 0;
        orgid       = 0;
        tstamp      = time_point_sec(0);
    };

    strctapproval(name i_approver, uint8_t i_level, uint64_t i_orgid, time_point_sec i_tstamp) {
        approver    = i_approver;
        level       = i_level;
        orgid       = i_orgid;
        tstamp      = i_tstamp;
    };

    strctapproval(tuple<eosio::name, uint8_t, uint64_t> &tplApprove) {
        approver   = get<0>(tplApprove);
        level      = get<1>(tplApprove);
        orgid      = get<2>(tplApprove);
        tstamp     = time_point_sec(current_time_point().sec_since_epoch());
    };

    EOSLIB_SERIALIZE(strctapproval, (approver)(level)(orgid)(tstamp));
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

    void add_approval(carboncert::strctapproval &approval) {
        check(approval.orgid == orgid, "Approval does not belong to the same organisation. ");
        approvals.push_back(approval);
    };

    EOSLIB_SERIALIZE(strctheader, (id)(strid)(type)(creator)(orgid)(status)(draftdate)(submitdate)(approvedate)(approvals));
};
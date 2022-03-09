// a vector of these stores the approvals for a system activity
struct strctapproval {
    name approver;
    uint8_t level; //records current level of user approver
    time_point_sec tstamp; //time of approval

    EOSLIB_SERIALIZE(strctapproval, (approver)(level)(tstamp));
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

    EOSLIB_SERIALIZE(strctheader, (id)(strid)(type)(creator)(orgid)(status)(draftdate)(submitdate)(approvedate)(approvals));
};

// header structure for primary row data for certificates and also system send funds requests
struct struct_header {

     /*
     const uint8_t STATUS_NONE                 = 0;

    //CERT STATUS
     const uint8_t STATUS_DATA_LOCKED          = 3;
     const uint8_t STATUS_DATA_DELETION        = 4;

     const uint8_t STATUS_DATA_DRAFT           = 40;
     const uint8_t STATUS_DATA_SUBMIT          = 41;
     const uint8_t STATUS_DATA_CORP_APPROVED   = 61;
     const uint8_t STATUS_DATA_ADMIN_APPROVED  = 201;
     const uint8_t STATUS_DATA_EXECUTED        = 255;*/

    uint64_t  id;       //numeric id of activity

    string    strid;    //full string id, limit to 256 characters, (contain numeric id inside strid)

    name      type;     //type of table object: "production", "bfcert", "send"
    name      creator;  //account that drafted the table object
    uint64_t  orgid;    //orgid to which creator belongs
    uint64_t  reforgid; //orgid to which is referenced as relational (such as c-sink producer)
    uint8_t   status;   //value that denotes status of activity: STATUS_DATA_DRAFT, STATUS_DATA_CORP_APPROVED, etc.
    bool      locked;   //if true, record is locked and will not be modified

    time_point_sec draftdate;  //when drafted in the system
    time_point_sec submitdate; //when submitted in the system
    time_point_sec corp_appr_date; //when corporation approved
    time_point_sec admin_appr_date; //when administrator approved
    time_point_sec deletedate; //when it was deleted
    time_point_sec lockdate;  //when it was locked
    time_point_sec executed_date; //date when activity is executed (unable to revert or delete)

    struct_header(){
        status = 0;
        locked = false;
    };

    struct_header(uint64_t i_id, string i_strid, name i_type, name i_creator, uint64_t i_orgid, uint64_t i_reforgid, uint8_t i_status) {
        id        = i_id;
        strid     = i_strid;
        type      = i_type;
        creator   = i_creator;
        orgid     = i_orgid;
        reforgid  = i_reforgid;
        status    = i_status;
        locked    = false;
        draftdate = time_point_sec(current_time_point().sec_since_epoch());
    };

    struct_header(uint64_t i_id, string i_strid, name i_type, name i_creator, uint64_t i_orgid, uint64_t i_reforgid, uint8_t i_status, time_point_sec i_draftdate) {
        id        = i_id;
        strid     = i_strid;
        type      = i_type;
        creator   = i_creator;
        orgid     = i_orgid;
        reforgid  = i_reforgid;
        status    = i_status;
        locked    = false;
        draftdate = i_draftdate;
    };

    void can_update() {
        check(!locked, "Unable to modify record, struct_header.can_update() failed due to lock status, unlock record first. ");
        check(status != 4, "Unable to modify record, struct_header.can_update() failed due to deletion status. ");
        check(status != 255, "Unable to modify record, struct_header.can_update() failed due to execution status. ");
    };

    void lock(bool bSet) {
        if(bSet) {
            lockdate = time_point_sec(current_time_point().sec_since_epoch());
            locked = true;
        } else {
            lockdate = time_point_sec(0);
            locked = false;
        }
    };

    void delete_status(bool bSet) {
        can_update();
        if(bSet) {
            deletedate = time_point_sec(current_time_point().sec_since_epoch());
            status = 4;
        } else {
             check(false, "Operation struct_header.delete_status(...) can not be reverted. ");
        }
    };

    void draft(bool bSet) {
        can_update();
        if(bSet) {
            draftdate = time_point_sec(current_time_point().sec_since_epoch());
            status = 40;
        } else {
            check(false, "Operation struct_header.draft(...) makes no sense for the context. ");
        }
    };

    void submit(bool bSet) {
        can_update();
        if(bSet) {
            check(status == 40, "Unable to submit non-draft status data. ");
            submitdate = time_point_sec(current_time_point().sec_since_epoch());
            status = 41;
        } else {
            if(status == 41) {
                submitdate = time_point_sec(0);
                status = 40;
            } else {
                check(false, "Operation struct_header.submit(...) makes no sense for the context. ");
            }
        }
    };

    void corp_approve(bool bSet) {
        can_update();
        if(bSet) {
            check(status == 41, "Unable to corp approve non-submitted status data. ");
            corp_appr_date = time_point_sec(current_time_point().sec_since_epoch());
            status         = 61;
        } else {
            if(status == 61) {
                corp_appr_date = time_point_sec(0);
                status = 41;
            } else {
                check(false, "Operation struct_header.corp_approve(...) makes no sense for the context. ");
            }
        }
    };

    void admin_approve(bool bSet) {
        can_update();
        if(bSet) {
            check(status == 61, "Unable to admin approve when organisation has not approved. ");
            admin_appr_date = time_point_sec(current_time_point().sec_since_epoch());
            status          = 201;
        } else {
            check(false, "Operation struct_header.admin_approve(...) is unable to revert, delete certificate instead. ");
        }
    };

    void executed(bool bSet) {
        can_update();
        if(bSet) {
            check(status == 201, "Unable to execute activity without system approval. ");
            executed_date = time_point_sec(current_time_point().sec_since_epoch());
            status          = 255;
        } else {
            check(false, "Operation struct_header.executed(...) is unable to revert or delete execution, you may only lock the record. ");
        }
    };

    EOSLIB_SERIALIZE(struct_header, (id)(strid)(type)(creator)(orgid)(reforgid)(status)(locked)(draftdate)(submitdate)(corp_appr_date)(admin_appr_date)(deletedate)(lockdate)(executed_date));
};
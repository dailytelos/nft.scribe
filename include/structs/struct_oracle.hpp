//struct_oracle.hpp

// The structure for describing a network oracle
struct struct_oracle {
    name id;            //name of the Telos Native account setup as an oracle
    name network_id;    //network id, example: "eth.mainnet", also functions as the scope of the oracles table
    
    int8_t active;           // 0 = not active, 1 = active, -1 = temp kicked status, -2 = banned status
    uint64_t cnt_updates;    // number of times update() was called by oracle
    uint64_t trx_voted;      // number of times oracle voted on trx
    uint64_t trx_posted;     // number of times oracle originally posted trx

    string apisource;        //root URL / IP address / "localhost" where the oracle is deriving its API information, exclude tokens and authentication 

    time_point_sec registration;    //date when the oracle first registered
    time_point_sec last_update;     //last time the oracle updated its active status / posted to the contract
    time_point_sec last_claim;      //last time the oracle claimed rewards
    time_point_sec last_sanction;   //last time kicked and/or banned
    time_point_sec kick_expires;    //the time in which the kick status expires

    struct_oracle() {
        id           = name(NAME_NULL);
        network_id   = name(NAME_NULL);
        active       = ORACLE_STATUS_INACTIVE;
        cnt_updates  = 0;
        trx_voted    = 0;
        trx_posted   = 0;
        registration    = time_point_sec(0);
        last_update     = time_point_sec(0);
        last_claim      = time_point_sec(0);
        last_sanction   = time_point_sec(0);
        kick_expires    = time_point_sec(0);
    };

    struct_oracle(name i_id, name i_network_id, int8_t i_active, uint64_t i_cnt_updates, uint64_t i_trx_voted, uint64_t i_trx_posted, string i_apisource, time_point_sec i_registration, time_point_sec i_last_update, time_point_sec i_last_claim, time_point_sec i_last_sanction, time_point_sec i_kick_expires) {
        id            = i_id;
        network_id    = i_network_id;
        active        = i_active;
        cnt_updates   = i_cnt_updates;
        trx_voted     = i_trx_voted;
        trx_posted    = i_trx_posted;
        apisource     = i_apisource;
        registration  = i_registration;
        last_update   = i_last_update;
        last_claim    = i_last_claim;
        last_sanction = i_last_sanction;
        kick_expires  = i_kick_expires;
    };

    bool is_kicked() {
        uint32_t tNow = current_time_point().sec_since_epoch();
        uint32_t tKickExp = kick_expires.sec_since_epoch();

        if(tNow >= tKickExp) { return false; }

        return (active == ORACLE_STATUS_KICKED);
    }

    bool is_banned() {
        return (active == ORACLE_STATUS_BANNED);
    }

    bool is_active() {
        return (active == ORACLE_STATUS_ACTIVE);
    }

    void set_not_active() {
        if(is_active()) { 
            active = ORACLE_STATUS_INACTIVE;
        }
    }

    
    void update() {
        if(is_kicked()) { check(false, "Oracle update not possible, oracle is kicked. "); }
        if(is_banned()) { check(false, "Oracle update not possible, oracle is banned. "); }
        check(is_active(), "Oracle is not registered as active, unable to update. ");

        last_update = time_point_sec(current_time_point().sec_since_epoch());
        cnt_updates = cnt_updates + 1;
    }

    void voted() {
        if(is_kicked()) { check(false, "Oracle update not possible, oracle is kicked. "); }
        if(is_banned()) { check(false, "Oracle update not possible, oracle is banned. "); }
        check(is_active(), "Oracle is not registered as active, unable to vote. ");

        last_update = time_point_sec(current_time_point().sec_since_epoch());
        trx_voted = trx_voted + 1;
    }

    void posted() {
        if(is_kicked()) { check(false, "Oracle update not possible, oracle is kicked. "); }
        if(is_banned()) { check(false, "Oracle update not possible, oracle is banned. "); }
        check(is_active(), "Oracle is not registered as active, unable to post. ");

        last_update = time_point_sec(current_time_point().sec_since_epoch());
        trx_posted = trx_posted + 1;
    }

    EOSLIB_SERIALIZE(struct_oracle, (id)(network_id)(active)(cnt_updates)(trx_voted)(trx_posted)(apisource)(registration)(last_update)(last_claim)(last_sanction)(kick_expires));
};
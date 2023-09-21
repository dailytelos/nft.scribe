//oracles.cpp
    
    //get_self() or a BP who has surpassed the vote threshold
    ACTION nftscribe::orcregister(const name& auth, const name& oracle_id, const name& network_id, const string& apisource) {
        require_auth(auth);

        if(auth.value == get_self().value) {
            //can register any oracle
            _orcregister(auth, oracle_id, network_id, apisource);
        } else {
            check(auth.value == oracle_id.value, "Your authorization cannot register on behalf of another account. ");

            name scope = network_id; 
            oracle_index _orc_table( get_self(), scope.value );
            auto orc_itr = _orc_table.find(oracle_id.value);
            
            uint64_t oracle_tier = get_orctier(oracle_id);

            check(oracle_tier >= 1, "Your authorization is unable to register itself, you lack sufficient votes, oracles must multi-sig to register this account. ");

            _orcregister(auth, oracle_id, network_id, apisource);
        }
    }

    void nftscribe::_orcregister(const name& auth, const name& oracle_id, const name& network_id, const string& apisource) {
    
        name scope = network_id; 
        oracle_index _orc_table( get_self(), scope.value );
        auto orc_itr = _orc_table.find(oracle_id.value);

        if(orc_itr == _orc_table.end()){
            _orc_table.emplace( get_self(), [&]( auto& row ) {
                row.id = oracle_id;
                row.oracle = struct_oracle(oracle_id, network_id, 0, 0, 0, 0, apisource,  time_point_sec(current_time_point().sec_since_epoch()), time_point_sec(0), time_point_sec(0), time_point_sec(0),  time_point_sec(0));
            });

        } else { //update existing apisource
            _orc_table.modify( orc_itr, get_self(), [&]( auto& row ) {
                row.oracle.apisource = apisource;
            });
        }
    }


    //requires the authority of the oracle itself
    ACTION nftscribe::orcstatus(const name& oracle_id, const name& network_id, const int8_t& active) {
        require_auth(oracle_id);

        _orcstatus(oracle_id, network_id, active);
    }

    void nftscribe::_orcstatus(const name& oracle_id, const name& network_id, const int8_t& active) {

        check((active >= ORACLE_STATUS_BANNED) && (active <= ORACLE_STATUS_ACTIVE), "Invalid active value passed to _orcstatus(...). ");

        name scope = network_id; 
        oracle_index _orc_table( get_self(), scope.value );
        auto orc_itr = _orc_table.find(oracle_id.value);

        if(orc_itr == _orc_table.end()){
            check(false, "No such oracle_id is registered on this network. ");
        } else { 
            
            _orc_table.modify( orc_itr, get_self(), [&]( auto& row ) {
                if(row.oracle.is_banned()) { check(false, "Oracle has been banned, unable to update status. "); }
                if(row.oracle.is_kicked()) { check(false, "Oracle has been kicked, unable to update status. "); }

                row.oracle.active = active;
                row.oracle.kick_expires = time_point_sec(0);
            });
        }
    }

    ACTION nftscribe::orckick(const name& auth, const name& oracle_id, const name& network_id, const uint32_t& hours) {
        require_auth(auth);

        if(auth.value == get_self().value) {
            check(hours <= 9000, "The maximum kick duration is 9000 hours, you should proceed to ban for longer kick durations. ");
            _orckick(auth, oracle_id, network_id, hours);
        } else {
            check(false, "Only contract authority get_self() may kick an oracle; try implementing a multisig to kick. ");
        }
    }

    void nftscribe::_orckick(const name& auth, const name& oracle_id, const name& network_id, const uint32_t& hours) {
    
        name scope = network_id; 
        oracle_index _orc_table( get_self(), scope.value );
        auto orc_itr = _orc_table.find(oracle_id.value);

        if(orc_itr == _orc_table.end()){
            check(false, "No such oracle_id is registered on this network. ");
        } else { 
        
            _orc_table.modify( orc_itr, get_self(), [&]( auto& row ) {
                if(row.oracle.is_banned()) { check(false, "Oracle has been banned, unable to update status. "); }

                row.oracle.active = ORACLE_STATUS_KICKED;
                uint32_t tNow = current_time_point().sec_since_epoch();
                uint32_t tKickExp = tNow + (hours * 3600);
                row.oracle.last_sanction = time_point_sec(tNow);
                row.oracle.kick_expires = time_point_sec(tKickExp);
            });
        }
    }

    ACTION nftscribe::orcban(const name& auth, const name& oracle_id, const name& network_id, const uint8_t& ban) {
        require_auth(auth);
    
        if(auth.value == get_self().value) {
            _orcban(auth, oracle_id, network_id, ban);
        } else {
            check(false, "Only contract authority get_self() may ban an oracle; try implementing a multisig to ban. ");
        }
    }

    void nftscribe::_orcban(const name& auth, const name& oracle_id, const name& network_id, const uint8_t& ban) {

        check( (ban == 0) || (ban == 1), "Invalid uint8_t, the ban parameter must be set to 0 or 1. ");

        name scope = network_id; 
        oracle_index _orc_table( get_self(), scope.value );
        auto orc_itr = _orc_table.find(oracle_id.value);

        if(orc_itr == _orc_table.end()){
            check(false, "No such oracle_id is registered on this network. ");
        } else {
            _orc_table.modify( orc_itr, get_self(), [&]( auto& row ) {
               
                if(ban == 1) {
                    check(!row.oracle.is_banned(), "Unable to ban, oracle_id is already banned. ");
                    uint32_t tNow = current_time_point().sec_since_epoch();
                    row.oracle.active = ORACLE_STATUS_BANNED;
                    row.oracle.last_sanction = time_point_sec(tNow);
                } else {
                    check(row.oracle.is_banned(), "Unable to remove a ban, when oracle_id is not currently banned. ");
                    row.oracle.active = ORACLE_STATUS_INACTIVE;
                }
            });
        }
    }

    ACTION nftscribe::orcrefresh(const name& auth, const name& oracle_id, const name& network_id) {
        require_auth(auth);
    
        check(auth.value == oracle_id.value, "The action orcrefresh can only be called where auth == oracle_id. ");

        _orcrefresh(auth, oracle_id, network_id);
    }

    void nftscribe::_orcrefresh(const name& auth, const name& oracle_id, const name& network_id) {

        name scope = network_id; 
        oracle_index _orc_table( get_self(), scope.value );
        auto orc_itr = _orc_table.find(oracle_id.value);

        if(orc_itr == _orc_table.end()){
            check(false, "No such oracle_id is registered on this network. ");
        } else {
            _orc_table.modify( orc_itr, get_self(), [&]( auto& row ) {
                row.oracle.update();
            });
        }
    }

    ACTION nftscribe::orcpost(const name& oracle_id, const struct_post& cPost) {
        require_auth(oracle_id);

        _orcpost(oracle_id, cPost);
    }

    void nftscribe::_orcpost(const name& oracle_id, const struct_post& cPost) {
        //scope is cPost.network_id

        // ** add code here
    }

    nftscribe::struct_oracle nftscribe::get_oracle(const name& oracle_id, const name& network_id) {
        name scope = network_id; 
        oracle_index _orc_table( get_self(), scope.value );
        auto orc_itr = _orc_table.find(oracle_id.value);

        if(orc_itr == _orc_table.end()){
            check(false, "No such oracle_id is registered on this network. ");
        }

        struct_oracle cOracle = orc_itr->oracle;
        return cOracle;
    }

    uint64_t nftscribe::get_orcbpvotes(const name& oracle_id) {
        //check BP Votes
        name scope = name("eosio"); 
        producers_table _prod_table( get_self(), scope.value );
        auto prod_itr = _prod_table.find(oracle_id.value);
        
        return (uint64_t) prod_itr->total_votes;

                uint64_t total_votes = (uint64_t) prod_itr->total_votes;
                uint64_t tier_one = getglobalint(name("orc.tierone"));
    }

    uint64_t nftscribe::get_orctier(const name& oracle_id) {

        uint64_t total_votes = get_orcbpvotes(oracle_id);

        uint64_t tier_one = getglobalint(name("orc.tierone"));
        uint64_t tier_two = getglobalint(name("orc.tiertwo"));

        if(total_votes >= tier_two) { return 2; }
        if(total_votes >= tier_one) { return 1; }
        
        return 0;
    }

    
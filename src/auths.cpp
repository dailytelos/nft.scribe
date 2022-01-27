
ACTION carboncert::setauthlevel(const name& authuser, const name& user, const uint8_t& level, const uint64_t& org) {
    checkfreeze();

    require_auth(authuser);
    min_auth(authuser, AUTH_LEVEL_ROOTADMIN);
    check( is_account( user ), "User account does not exist. ");

    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    if(auths_itr == _auths.end()) { //emplace

        _auths.emplace( get_self(), [&]( auto& auth_row ) {
            auth_row.user   = user;
            auth_row.level  = level;
        });
    } else { //modify

        _auths.modify( auths_itr, get_self(), [&]( auto& auth_row ) {
            auth_row.level = level;
        });
    }
}

ACTION carboncert::setorg(const name& authuser, const uint64_t& orgid, const string& orgname) {
    checkfreeze();

    require_auth(authuser);
    min_auth(authuser, AUTH_LEVEL_ROOTADMIN);

    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    if(orgs_itr == _orgs.end()) { //emplace

        _orgs.emplace( get_self(), [&]( auto& org_row ) {
            org_row.id   = orgid;
            org_row.orgname  = orgname;
        });
    } else { //modify

        _orgs.modify( orgs_itr, get_self(), [&]( auto& org_row ) {
            org_row.orgname = orgname;
        });
    }
}

ACTION carboncert::delauthlevel(const name& authuser, const name& user) {
    checkfreeze();

    require_auth(authuser);
    min_auth(authuser, AUTH_LEVEL_ROOTADMIN);
    
    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    check(auths_itr != _auths.end(), "User account does not have auth setting. ");

	auths_itr = _auths.erase( auths_itr );
}


ACTION carboncert::delorg(const name& authuser, const uint64_t& orgid) {
    checkfreeze();

    require_auth(authuser);
    min_auth(authuser, AUTH_LEVEL_ROOTADMIN);

    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    check(orgs_itr != _orgs.end(), "Organsiation ID does not exist. ");

	orgs_itr = _orgs.erase( orgs_itr );
}

void carboncert::min_auth(const name& user, const uint8_t& level) {
    if(get_self().value == user.value) { //contract itself
        require_auth(get_self());
        return;
    } else {
        auths_index _auths( get_self(), get_self().value );
        auto auths_itr = _auths.find(user.value);

        check(auths_itr != _auths.end(), "User auth level does not exist. ");
        check(auths_itr->level >= level, "User fails to meet minimum authorisation. ");
    }
}

uint64_t carboncert::getorgid(const name& user) {
    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    check(auths_itr != _auths.end(), "User auth level does not exist. ");
    return auths_itr->orgid;
}


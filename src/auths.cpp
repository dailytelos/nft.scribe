
ACTION carboncert::setauthlevel(const name& authuser, const name& user, const uint8_t& level, const uint64_t& org) {
    checkfreeze();

    require_auth(authuser);
    uint8_t nAuth = get_org_auth(authuser);

    if(authuser.value != get_self().value)
    { check(nAuth > level, "Authuser does not have sufficient permission. "); }

    check( (nAuth == AUTH_LEVEL_CORP_ADMIN) || (nAuth == AUTH_ADMIN_CORP_ROLES) || (nAuth == AUTH_ADMIN_ADMIN_ROLES) || (nAuth == AUTH_LEVEL_ROOTADMIN), "Authuser does not have sufficient role for action. ");
    
    if(nAuth < AUTH_ADMIN_CORP_ROLES)
    { check(get_org_id(authuser) == get_org_id(user),"Authuser must belong to the same organisation as user. "); }

    check( is_account( user ), "User account does not exist. ");

    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    if(auths_itr == _auths.end()) { //emplace

        _auths.emplace( get_self(), [&]( auto& auth_row ) {
            auth_row.user   = user;
            auth_row.level  = level;
            auth_row.orgid  = org;
        });
    } else { //modify

        _auths.modify( auths_itr, get_self(), [&]( auto& auth_row ) {
            auth_row.level = level;
            auth_row.orgid  = org;
        });
    }
}

ACTION carboncert::delauthlevel(const name& authuser, const name& user) {
    checkfreeze();

    require_auth(authuser);
    uint8_t nAuth = get_org_auth(authuser);
    uint8_t level = get_org_auth(user);

    if(authuser.value != get_self().value)
    { check(nAuth > level, "Authuser does not have sufficient permission. "); }

    check( (nAuth == AUTH_LEVEL_CORP_ADMIN) || (nAuth == AUTH_ADMIN_CORP_ROLES) || (nAuth == AUTH_ADMIN_ADMIN_ROLES) || (nAuth == AUTH_LEVEL_ROOTADMIN), "Authuser does not have sufficient role for action. ");
    
    if(nAuth < AUTH_ADMIN_CORP_ROLES)
    { check(get_org_id(authuser) == get_org_id(user),"Authuser must belong to the same organisation as user. "); }

    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    check(auths_itr != _auths.end(), "User account does not have auth setting. ");

	auths_itr = _auths.erase( auths_itr );
}


ACTION carboncert::setorg(const name& authuser, const uint64_t& orgid, const string& orgname) {
    checkfreeze();

    require_auth(authuser);
    uint8_t nAuth = get_org_auth(authuser);

    check(orgid > 10, "Parameter orgid must be set above value 10, below this value are reserved. ");
    check((nAuth == AUTH_ADMIN_CORP_ROLES) || (nAuth == AUTH_ADMIN_ADMIN_ROLES) || (nAuth == AUTH_LEVEL_ROOTADMIN), "Authuser does not have sufficient role for action. ");

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


ACTION carboncert::delorg(const name& authuser, const uint64_t& orgid) {
    checkfreeze();

    require_auth(authuser);
    uint8_t nAuth = get_org_auth(authuser);

    check((nAuth == AUTH_ADMIN_ADMIN_ROLES) || (nAuth == AUTH_LEVEL_ROOTADMIN), "Authuser does not have sufficient role for action. ");

    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    check(orgs_itr != _orgs.end(), "Organsiation ID does not exist. ");

	orgs_itr = _orgs.erase( orgs_itr );
}

void carboncert::min_org_auth(const name& user, const uint8_t& level) {
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


void carboncert::has_org_auth(const name& user, const uint8_t& level) {
    if(get_self().value == user.value) { //contract itself
        require_auth(get_self());
        return;
    } else {
        auths_index _auths( get_self(), get_self().value );
        auto auths_itr = _auths.find(user.value);

        check(auths_itr != _auths.end(), "User auth level does not exist. ");
        check(auths_itr->level == level, "User fails to meet required authorisation. ");
    }
}

uint8_t carboncert::get_org_auth(const name& user) {
    if(get_self().value == user.value) { //contract itself
        return AUTH_LEVEL_ROOTADMIN;
    } else {
        auths_index _auths( get_self(), get_self().value );
        auto auths_itr = _auths.find(user.value);

        check(auths_itr != _auths.end(), "User auth level does not exist. ");

        return auths_itr->level;
    }
}

uint64_t carboncert::get_org_id(const name& user) {
    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    check(auths_itr != _auths.end(), "User auth level does not exist. ");
    return auths_itr->orgid;
}

tuple<name, uint8_t, uint64_t> carboncert::get_auth_row(const name& user) {

    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    if(auths_itr == _auths.end()) { //no entry
        return make_tuple(
                user,
                AUTH_LEVEL_VIEWER,
                0
            );
    } 

    return make_tuple(
                auths_itr->user,
                auths_itr->level,
                auths_itr->orgid
            );
}

ACTION carboncert::setauthlevel(const name& authuser, const name& user, const uint8_t& level, const uint64_t& org) {
    checkfreeze();

    check( is_account( user ), "User account does not exist. ");

    require_auth(authuser);
    checkorgexists(org);
    
    uint64_t auth_org = get_org_id(authuser);
    uint8_t  nAuth    = get_auth_by_org(authuser, auth_org);

    //check who can set
    check( (nAuth == AUTH_LEVEL_CORP_ADMIN) || (nAuth == AUTH_ADMIN_MASTER) || (nAuth == AUTH_LEVEL_ROOTADMIN), "User lacks authorisation to set roles for organsation. ");

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
    uint64_t auth_org = get_org_id(authuser);
    uint64_t user_org = get_org_id(user);

    uint8_t nAuth = get_auth_by_org(authuser, auth_org);
    uint8_t level = get_auth_by_org(user, user_org);

    if(nAuth < AUTH_ADMIN_MASTER)
    { check(auth_org == user_org,"Authuser must belong to the same organisation as user. "); }

    if(authuser.value != get_self().value)
    { check(nAuth > level, "Authuser does not have sufficient permission. "); }

    check( (nAuth == AUTH_LEVEL_CORP_ADMIN) || (nAuth == AUTH_ADMIN_MASTER) || (nAuth == AUTH_LEVEL_ROOTADMIN), "Authuser does not have sufficient role for action. ");
    
    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    check(auths_itr != _auths.end(), "User account does not have auth setting. ");

	auths_itr = _auths.erase( auths_itr );
}
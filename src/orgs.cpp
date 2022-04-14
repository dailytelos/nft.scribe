

ACTION carboncert::setorg(const name& authuser, const uint64_t& orgid, const string& orgname, const uint8_t& producer, const uint8_t& supplier, const uint8_t& retire) {
    checkfreeze();

    require_auth(authuser);
    uint64_t auth_org = get_org_id(authuser);
    uint8_t nAuth = get_auth_by_org(authuser, auth_org);

    if(authuser.value != get_self().value) {
        check(orgid > 10, "Parameter orgid must be set above value 10, below this value are reserved. ");
    }

    check((nAuth == AUTH_ADMIN_MASTER) || (nAuth == AUTH_LEVEL_ROOTADMIN), "Authuser does not have sufficient role for action. ");

    _setorg(orgid, orgname, producer, supplier, retire);
}


ACTION carboncert::setorgcert(const name& authuser, const uint64_t& orgid, const uint64_t& ebcid) {
    checkfreeze();

    require_auth(authuser);
    uint64_t auth_org = get_org_id(authuser);
    uint8_t nAuth = get_auth_by_org(authuser, auth_org);

    check((nAuth == AUTH_LEVEL_CORP_ADMIN) || (nAuth == AUTH_ADMIN_MASTER) || (nAuth == AUTH_LEVEL_ROOTADMIN), "Authuser does not have sufficient role for action. ");

    _setorgcert(orgid, ebcid);
}


ACTION carboncert::delorg(const name& authuser, const uint64_t& orgid) {
    checkfreeze();

    require_auth(authuser);
    uint64_t auth_org = get_org_id(authuser);
    uint8_t nAuth = get_auth_by_org(authuser, auth_org);

    //check(orgid > 10, "Parameter orgid must be set above value 10, below this value are reserved. ");
    check((nAuth == AUTH_ADMIN_MASTER) || (nAuth == AUTH_LEVEL_ROOTADMIN), "Authuser does not have sufficient role for action. ");

    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    check(orgs_itr != _orgs.end(), "Organsiation ID does not exist. ");

	orgs_itr = _orgs.erase( orgs_itr );
}


void carboncert::_setorg(const uint64_t& orgid, const string& orgname, const uint8_t& producer, const uint8_t& supplier, const uint8_t& retire) {
    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    if(orgs_itr == _orgs.end()) { //emplace

        _orgs.emplace( get_self(), [&]( auto& org_row ) {
            org_row.id        = orgid;
            org_row.orgname   = orgname;
            org_row.producer  = (producer > 0) ? 1 : 0;
            org_row.supplier  = (supplier > 0) ? 1 : 0;
            org_row.retire    = (retire > 0) ? 1 : 0;
        });
    } else { //modify

        _orgs.modify( orgs_itr, get_self(), [&]( auto& org_row ) {
            org_row.orgname   = orgname;
            org_row.producer  = (producer > 0) ? 1 : 0;
            org_row.supplier  = (supplier > 0) ? 1 : 0;
            org_row.retire    = (retire > 0) ? 1 : 0;
        });
    }
}

void carboncert::_setorgcert(const uint64_t& orgid, const uint64_t& ebcid) {
    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    if(orgs_itr == _orgs.end()) { //emplace
        check(false, "orgid does not exist for _setorgcert. ");
    } else { //modify

        _orgs.modify( orgs_itr, get_self(), [&]( auto& org_row ) {
            org_row.ebcid = ebcid;
        });
    }
}

uint64_t carboncert::get_org_cert(const uint64_t& orgid) {
    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    if(orgs_itr == _orgs.end()) { //emplace
        check(false, "orgid does not exist for getorgcert. ");
    } else { //modify
        return orgs_itr->ebcid;
    }

    return NULL;
}

/*uint8_t carboncert::get_auth_by_org(const name& user, const uint64_t& orgid) {
    if(get_self().value == user.value) { //contract itself
        return AUTH_LEVEL_ROOTADMIN; //auth applies to all organisations
    } else {
        auths_index _auths( get_self(), get_self().value );
        auto auths_itr = _auths.find(user.value);

        check(auths_itr != _auths.end(), "User auth level does not exist. ");

            orgs_index _orgs( get_self(), get_self().value );
            auto orgs_itr = _orgs.find(orgid);

            check(orgs_itr != _orgs.end(), "Org ID does not exist. ");

            if(auths_itr->level < AUTH_LEVEL_CORP_ADMIN) { //if specific auth for organisation
                
                //check they belong to the specific organisation
                check(auths_itr->orgid == orgid, "Requested get_auth_by_org does not match users orgid. ");
            }

        return auths_itr->level;
    }
}*/

uint8_t carboncert::get_auth_by_org(const name& user, const uint64_t& orgid) {
    if(get_self().value == user.value) { //contract itself
        return AUTH_LEVEL_ROOTADMIN; //auth applies to all organisations
    } else {
        auths_index _auths( get_self(), get_self().value );
        auto auths_itr = _auths.find(user.value);

        check(auths_itr != _auths.end(), "User auth level does not exist. ");

        orgs_index _orgs( get_self(), get_self().value );
        auto orgs_itr = _orgs.find(orgid);

        check(orgs_itr != _orgs.end(), "Org ID does not exist. ");

        if(auths_itr->orgid == ORG_ADMIN_ID) { //belongs to admin organisation
            if(auths_itr->level >= AUTH_ADMIN_AUTO_APPROVE) {
                return auths_itr->level;
            }
        } else {
            if(auths_itr->level < AUTH_ADMIN_AUTO_APPROVE) {
                return (orgid == auths_itr->orgid) ? auths_itr->level : AUTH_LEVEL_VIEWER;
            }
        }
    }

    return AUTH_LEVEL_VIEWER;
}

uint64_t carboncert::get_org_id(const name& user) {

    auths_index _auths( get_self(), get_self().value );
    auto auths_itr = _auths.find(user.value);

    if(auths_itr == _auths.end()) {
        if(user.value == get_self().value) {
            return ORG_ADMIN_ID;
        } else {
            return 0;
        }
    }

    checkorgexists(auths_itr->orgid);

    return auths_itr->orgid;
}

tuple<string, uint64_t, uint8_t, uint8_t, uint8_t> carboncert::get_org_tuple(const uint64_t& orgid) {

    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    check(orgs_itr != _orgs.end(), "Org ID does not exist. ");

    return make_tuple(orgs_itr->orgname, orgs_itr->ebcid, orgs_itr->producer, orgs_itr->supplier, orgs_itr->retire);
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


void carboncert::checkorgexists(const uint64_t& orgid) {

    orgs_index _orgs( get_self(), get_self().value );
    auto orgs_itr = _orgs.find(orgid);

    check(orgs_itr != _orgs.end(), "Org ID does not exist. ");
}
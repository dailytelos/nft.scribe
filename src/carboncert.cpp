#include <carboncert.hpp>

#include "./utility.cpp"
#include "./manage.cpp"
#include "./auths.cpp"

ACTION carboncert::issuecert(const name& issuer, const string& certid, const asset& supply) {
    checkfreeze();
    require_auth(issuer);

    min_auth(issuer, AUTH_LEVEL_CORP_ADMIN);

    newcert(issuer, certid, supply);
}

ACTION carboncert::claimcert(const name& issuer, const uint64_t& certn) {
    checkfreeze();
    require_auth(issuer);

    min_auth(issuer, AUTH_LEVEL_CORP_ADMIN);

    getcertfunds(issuer, certn);
}

ACTION carboncert::retirefunds(const name& sender, const asset& supply) {
    checkfreeze();
    require_auth(sender);

    min_auth(sender, AUTH_LEVEL_CORP_ADMIN);

    //check for sufficient deposit and reduce
    reducedep(sender, supply);

    //update certs and retire
    retiredep(sender, supply, getorgid(sender));
}

void carboncert::newcert(const name& issuer, const string& certid, const asset& supply) {

    int64_t nAmt = supply.amount;
    symbol_code cUnit = supply.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = supply.symbol.precision();
    string sMemo = "";

    check(nAmt > 0, "Certificate must have a positive supply value. ");
    check(sUnit == getglobalstr(name("tokensymbol")), "Symbol specified is incorrect for this contract. ");
    check(nPrec == getglobalint(name("tokenprec")), "Issue precision of token is mis-match with contract precision.  ");

    uint64_t certn = getglobalint(name("certcount")) + 1;

    certs_index _certs( get_self(), get_self().value );
    auto cert_itr = _certs.find(certn);

    check(cert_itr == _certs.end(), "Contract error, cert number already exists! ");

    if(cert_itr == _certs.end()) {
        _certs.emplace( get_self(), [&]( auto& cert_row ) {
            cert_row.certnum = certn;
            cert_row.issuer  = issuer;
            cert_row.certid  = certid;
            cert_row.supply  = supply;
            cert_row.claimed = false;
            cert_row.qtyretired = asset(0, symbol(cUnit, 4));
            cert_row.issuedate = time_point_sec(current_time_point().sec_since_epoch());
            cert_row.claimdate = time_point_sec(0);
        
            sMemo = "Certificate #", to_string(certn) ,"  for " + sUnit + " was created by ", issuer.to_string() + " to issue " + supply.to_string() + " -- (" + certid + ")";
            print(sMemo);
        });

        //issue token supply (will still need to be claimed)
        action(
            permission_level{ getcontract(), "active"_n},
            getcontract(),
            "issue"_n,
            std::make_tuple(
                getcontract(),
                supply,
                sMemo
            )
        ).send();

        setglobalint(name("certcount"), certn);

        issuestat(getorgid(issuer), supply);
    }
    else {
        check(false, "Contract error.");
    }
}

void carboncert::getcertfunds(const name& issuer, const uint64_t& certn) {
   
    certs_index _certs( get_self(), get_self().value );
    auto cert_itr = _certs.find(certn);

    check(cert_itr != _certs.end(), "Contract error, cert number does not exist. ");
    check(cert_itr->issuer == issuer, "Only the issuer may claim their token.");
    check(cert_itr->claimed == false, "Certificate has already been claimed.");

    int64_t nAmt = cert_itr->supply.amount;
    symbol_code cUnit = cert_itr->supply.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = cert_itr->supply.symbol.precision();
    string sMemo = "";

    check(nAmt > 0, "Certificate must have a positive supply value. ");
    check(sUnit == getglobalstr(name("tokensymbol")), "Symbol specified is incorrect for this contract. ");
    check(nPrec == getglobalint(name("tokenprec")), "Issue precision of token is mis-match with contract precision.  ");

    _certs.modify( cert_itr, get_self(), [&]( auto& this_row ) {
        this_row.claimed = true;
        this_row.claimdate = time_point_sec(current_time_point().sec_since_epoch());
    });

    sMemo = "Certificate #", to_string(certn) ,"  for " + sUnit + " was created by ", issuer.to_string() + " to issue " + cert_itr->supply.to_string() + " -- (" + cert_itr->certid + ")";
    
    action(
        permission_level{ getcontract(), "active"_n},
        getcontract(),
        "transfer"_n,
        std::make_tuple(
            getcontract(),
            issuer,
            cert_itr->supply,
            sMemo
        )
    ).send();
}

void carboncert::retiredep(const name& sender, const asset& supply, const uint64_t& orgid) { 
    
    int64_t nAmtToRetire = supply.amount;
    symbol_code cUnit = supply.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = supply.symbol.precision();

    asset aAmtToRetire = asset(nAmtToRetire, symbol(cUnit, 4));

    check(nAmtToRetire > 0, "E-t152 - Certificate must have a positive supply value. ");
    check(sUnit == getglobalstr(name("tokensymbol")), "E-t153 - Symbol specified is incorrect for this contract. ");
    check(nPrec == getglobalint(name("tokenprec")), "E-t154 - Issue precision of token is mis-match with contract precision.  ");

    //get globals
    uint64_t certcount   = getglobalint(name("certcount"));
    uint64_t retirecount = getglobalint(name("retirecount"));

    check(retirecount <= certcount, "E-t155 - Unable to process transaction (contract error retirecount <= certcount.) ");

    certs_index _certs( get_self(), get_self().value );
    auto cert_itr = _certs.find(retirecount);

    int64_t nSupply = cert_itr->supply.amount;
    int64_t nRetired = cert_itr->qtyretired.amount;

    check(nRetired < nSupply, "E-t156 - Unable to process transaction, no remaining amount to retire on certn (" + to_string(retirecount) + "). ");

    int64_t nPriorAmt = 0;
    uint8_t nIndex = 0;

    vector<carboncert::retiredcert>    retCerts;
    carboncert::retiredcert retCert;

    while((aAmtToRetire.amount > 0) && (nIndex < 20)) {
        retCert.certnum = cert_itr->certnum;
        retCert.certid = cert_itr->certid;
        
        nPriorAmt = aAmtToRetire.amount;

        //check if exceeds certcount
        check((retirecount + nIndex) <= certcount, "E-t192 - Unable to retire such a large supply, choose to retire less credits. ");

        aAmtToRetire = retireamount((retirecount + nIndex), aAmtToRetire);
        retCert.qtyretired = asset(nPriorAmt - aAmtToRetire.amount, symbol(cUnit, 4));

        retCerts.push_back(retCert);

        nIndex++;
        cert_itr = _certs.find(retirecount + nIndex);
    }

    check(nIndex < 50, "E-t51 - Unable to retire such a large supply, choose to retire less credits. ");

    //add entry to: typedef multi_index<name("retired"), retirements> retire_index;
    uint64_t nRetCt = getglobalint(name("retiredtblct")) + 1; 
    retire_index _retired( get_self(), get_self().value );
    auto ret_itr = _retired.find(nRetCt);

    check(ret_itr == _retired.end(), "E-t162 - Value error with retiredtblct global row already exists. ");

    _retired.emplace( get_self(), [&]( auto& ret_row ) {
        ret_row.retirenum = nRetCt;
        ret_row.orgid = getorgid(sender);
        ret_row.account = sender;
        ret_row.supplyret = supply;
        ret_row.certs = retCerts;
        ret_row.retiredate = time_point_sec(current_time_point().sec_since_epoch());
    });

    setglobalint(name("retiredtblct"), nRetCt);

    string sMemo = "Token retirement from " + get_self().to_string();
    
    action(
            permission_level{ get_self(), "active"_n},
            getcontract(),
            "transfer"_n,
            std::make_tuple(
                get_self(),
                getcontract(),
                supply,
                sMemo
            )
        ).send();
    
    //update typedef multi_index<name("retirestat"), certstats> retirestat_index;
    retirestat(orgid, supply);
}

asset carboncert::retireamount(const uint64_t& certn, const asset& supply) {

    int64_t nAmtToRetire = supply.amount;
    symbol_code cUnit = supply.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = supply.symbol.precision();

    check(nAmtToRetire > 0, "E-t1152 - Certificate must have a positive supply value. ");

    certs_index _certs( get_self(), get_self().value );
    auto cert_itr = _certs.find(certn);

    check(sUnit == getglobalstr(name("tokensymbol")), "E-t1153 - Symbol specified is incorrect for this contract. ");
    check(nPrec == getglobalint(name("tokenprec")), "E-t1154 - Issue precision of token is mis-match with contract precision.  ");
    check(cert_itr != _certs.end(), "E-t1122 - Contract error, cert number does not exist. ");

    int64_t nSupply = cert_itr->supply.amount;
    int64_t nRetired = cert_itr->qtyretired.amount;
    int64_t nRetiredUpdate = nRetired;

    check(nRetired < nSupply, "E-t1556 - Unable to process transaction, no remaining amount to retire on certn (" + to_string(certn) + "). ");

    int64_t nCertRemain = nSupply - nRetired;
    int64_t nReturnAmt = 0;
    bool setRetired = false;

    if(nAmtToRetire >= nCertRemain) {
        nReturnAmt = nAmtToRetire - nCertRemain;
        nRetiredUpdate = nSupply;
        setRetired = true;
    } else {
        nReturnAmt = 0;
        nRetiredUpdate = nRetired + nAmtToRetire;
        setRetired = false;
    }

    _certs.modify( cert_itr, get_self(), [&]( auto& this_row ) {

        this_row.qtyretired = asset(nRetiredUpdate, symbol(cUnit, 4));

        if(setRetired) {
            this_row.retiredate = time_point_sec(current_time_point().sec_since_epoch());
            this_row.retired = true;

            //update retirecount
            setglobalint(name("retirecount"), certn + 1);
        }
    });

    return asset(nReturnAmt, symbol(cUnit, 4));
}

void carboncert::retirestat(const uint64_t& orgid, const asset& supplyret) {

    int64_t nAmt = supplyret.amount;
    symbol_code cUnit = supplyret.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = supplyret.symbol.precision();

    string sDate = time_point_sec(current_time_point().sec_since_epoch()).to_string();
    uint64_t nYear = (uint64_t) stoi(split(sDate,"-")[0]);
    uint64_t nMonth = (uint64_t) stoi(split(sDate,"-")[1]);
    uint64_t dateval = getdataval(nYear,nMonth);
    uint64_t id = ((uint64_t)orgid << 32)|(uint64_t)dateval;

    retirestat_index _retirestat( get_self(), get_self().value );
    auto stat_itr = _retirestat.find(id);

    if(stat_itr == _retirestat.end()) {

        _retirestat.emplace( get_self(), [&]( auto& stat_row ) {
            stat_row.orgid = orgid;
            stat_row.dateval = dateval;
            stat_row.supplystat = supplyret;
        });
    } else { //modify by adding

        int64_t nAdd = nAmt + stat_itr->supplystat.amount;

        _retirestat.modify( stat_itr, get_self(), [&]( auto& stat_row ) {
            stat_row.supplystat = asset(nAdd, symbol(cUnit, 4));
        });
    }
}


void carboncert::issuestat(const uint64_t& orgid, const asset& supplyiss) {

    int64_t nAmt = supplyiss.amount;
    symbol_code cUnit = supplyiss.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = supplyiss.symbol.precision();

    string sDate = time_point_sec(current_time_point().sec_since_epoch()).to_string();
    uint64_t nYear = (uint64_t) stoi(split(sDate,"-")[0]);
    uint64_t nMonth = (uint64_t) stoi(split(sDate,"-")[1]);
    uint64_t dateval = getdataval(nYear,nMonth);
    uint64_t id = ((uint64_t)orgid << 32)|(uint64_t)dateval;

    issuestat_index _issuestat( get_self(), get_self().value );
    auto stat_itr = _issuestat.find(id);

    if(stat_itr == _issuestat.end()) {

        _issuestat.emplace( get_self(), [&]( auto& stat_row ) {
            stat_row.orgid = orgid;
            stat_row.dateval = dateval;
            stat_row.supplystat = supplyiss;
        });
    } else { //modify by adding

        int64_t nAdd = nAmt + stat_itr->supplystat.amount;

        _issuestat.modify( stat_itr, get_self(), [&]( auto& stat_row ) {
            stat_row.supplystat = asset(nAdd, symbol(cUnit, 4));
        });
    }
}

uint64_t carboncert::getdataval(const uint64_t& year, const uint64_t& month) {
    return (year * 1000) + month;
}

void carboncert::reducedep(const name& sender, const asset& supply) {

    name depositor = sender;
    uint64_t nOnDep = getglobalint(name("depositacct"));

    check(nOnDep >= supply.amount, "E-t1144 - depositacct lacks sufficient balance to reduce. ");

    asset aDep = getdepamt(depositor);

    check(aDep.amount >= supply.amount, "E-t1145 - " + sender.to_string() + " lacks sufficient retirement deposits balance to reduce. ");

    asset aSupply = supply;
    int64_t nSetDep = aDep.amount - supply.amount;
    string sMemo = "retire credits on deposit";

    if(nSetDep > 0) {
        subdeposit(depositor, aSupply, sMemo);
    } else {
        deldeposit(depositor);
    }
}
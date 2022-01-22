#include <carboncert.hpp>

#include "./manage.cpp"

ACTION carboncert::issuecert(const name& issuer, const string& certid, const asset& supply) {
    //checkfreeze();
    require_auth(issuer);

    check(false, "No go");

    //newcert(issuer, certid, supply);
}

ACTION carboncert::claimcert(const name& issuer, const uint64_t& certn) {
    //checkfreeze();
    require_auth(issuer);

    check(false, "No go");

    //getcertfunds(issuer, certn);
}

/*
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
    }
    else {
        check(false, "Contract error.");
    }
}

void carboncert::getcertfunds(const name& issuer, const uint64_t& certn) {
   
    certs_index _certs( get_self(), get_self().value );
    auto cert_itr = _certs.find(certn);

    check(cert_itr != _certs.end(), "Contract error, cert number does not exist. ");

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
}*/

//EOSIO_DISPATCH(carboncert, (issuecert)(claimcert))

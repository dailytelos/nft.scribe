#include <carboncert.hpp>

#include "./utility.cpp"
#include "./manage.cpp"
#include "./auths.cpp"

/**************************************************
* ACTION draftcertpr(const name& creator, const string& strid, const string& cname, const string& exid, const asset& tpot)
*  This action drafts a production certificate.
*
* creator - authorised creator drafting cert
* strid   - system String ID assigned to cert such as (FvnXL-xDwkfL-FHog)
* cname   - A title string that the certificate may have
* exid    - External ID (such as original production certificate ID)
* tpot    - 0.0000 MT asset representing metric tonne of production certificate
*/
ACTION carboncert::draftcertpr(const name& creator, const string& strid, const string& cname, const string& exid, const asset& tpot) {
    checkfreeze();
    require_auth(creator);

    //check for draftcert auth
    uint8_t auth = get_org_auth(creator);

    check( (auth == AUTH_LEVEL_CORP_CERTS) ||
           (auth == AUTH_LEVEL_CORP_ADMIN) ||
           (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");

    _draftcert_p(creator, strid, cname, exid, tpot);
}
/**************************************************
* ACTION draftcertcs( ... )
*  This action drafts a c-sink certificate.
*
* creator  - authorised creator drafting cert
* strid    - system String ID assigned to cert such as (FvnXL-xDwkfL-FHog)
* loc      - ISO 3 digit code -- location code ISO 3166:  https://en.wikipedia.org/wiki/ISO_3166-1_numeric
* type     - Type of C-Sink such as "biochar"
* desc     - Description of type of c-sink
* gross    - 0.0000 MT asset representing metric tonne of c-sink gross
* humidity - 0.00 P asset representing percentage 0.00 to 100.00 P inclusive
* tmin     -  0.0000 MT asset representing min stat
* tmax     -  0.0000 MT asset representing max stat
* tavg     -  0.0000 MT asset representing avg stat (what the contract uses)
* ystart   -  year it starts
* yend     -  year it ends
* portid   -  portfolio id
* prodid   -  production certificate id
*/
ACTION carboncert::draftcertcs( const name& creator,
                                const string& strid,
                                const uint16_t& loc,
                                const string& type,
                                const string& desc,
                                const asset& gross,
                                const asset& humidity,
                                const asset& tmin,
                                const asset& tmax,
                                const asset& tavg,
                                const uint32_t& ystart,
                                const uint32_t& yend,
                                const uint64_t& portid,
                                const uint64_t& prodid) {
    checkfreeze();
    require_auth(creator);

    //check for draftcert auth
    uint8_t auth = get_org_auth(creator);

    check( (auth == AUTH_LEVEL_CORP_CERTS) ||
           (auth == AUTH_LEVEL_CORP_ADMIN) ||
           (auth == AUTH_LEVEL_ROOTADMIN), "You lack the neccessary authorisation to perform this action. ");

    _draftcert_c(   creator,
                    strid,
                    loc,
                    type,
                    desc,
                    gross,
                    humidity,
                    tmin,
                    tmax,
                    tavg,
                    ystart,
                    yend,
                    portid,
                    prodid  );
}

void carboncert::_draftcert_p(const name& creator, const string& strid, const string& cname, const string& exid, const asset& tpot) {

    int64_t nAmt = tpot.amount;
    symbol_code cUnit = tpot.symbol.code();
    string sUnit = cUnit.to_string();
    uint8_t nPrec = tpot.symbol.precision();
    string sMemo = "";

    check(nAmt > 0, "Certificate must have a positive value of carbon production. ");
    check(sUnit == "MT", "Tonne symbol specified is incorrect for this contract, must be MT. ");
    check(nPrec == 4, "Issue precision of carbon is mis-match with contract precision.  ");

    uint64_t certn = getglobalint(name("pcertcount")) + 1;

    prodcert_index _prodcerts( get_self(), get_self().value );
    auto pcert_itr = _prodcerts.find(certn);

    check(pcert_itr == _prodcerts.end(), "Contract error, cert number already exists! ");

    tuple<name, uint8_t, uint64_t> tplApr = get_auth_row(creator);
    strctapproval cApprove = strctapproval(creator, get<1>(tplApr), get<2>(tplApr), time_point_sec(current_time_point().sec_since_epoch()));

    _prodcerts.emplace( get_self(), [&]( auto& cert_row ) {
        cert_row.id = certn;
        cert_row.c  = cert_production(
                        strctheader(certn, strid, name("production"), creator, get_org_id(creator), STATUS_CERT_DRAFT),
                        crtproduction(cname, exid, tpot)
                      );

        cert_row.c.header.add_approval(cApprove);
       
        sMemo = "Production Certificate #" + to_string(certn) + "  was drafted by ", creator.to_string() + " for carbon tons totalling: " + tpot.to_string() + " -- (" + strid + ")";
        print(sMemo);
    });

    setglobalint(name("pcertcount"), certn);
}


void carboncert::_draftcert_c(  const name& creator,
                                const string& strid,
                                const uint16_t& loc,
                                const string& type,
                                const string& desc,
                                const asset& gross,
                                const asset& humidity,
                                const asset& tmin,
                                const asset& tmax,
                                const asset& tavg,
                                const uint32_t& ystart,
                                const uint32_t& yend,
                                const uint64_t& portid,
                                const uint64_t& prodid) {

    string sMemo = "";
    
    check(gross.amount > 0, "Certificate must have a positive value of carbon production. ");
    check(gross.symbol.code().to_string() == "MT", "Tonne symbol specified is incorrect for this contract, must be MT. ");
    check(gross.symbol.precision() == 4, "Asset precision must be set to 4 decimals.  ");

    check(humidity.amount > 0, "Certificate must have a positive value of carbon production. ");
    check(humidity.symbol.code().to_string() == "P", "Percent asset symbol specified is incorrect for this contract, must be P. ");
    check(humidity.symbol.precision() == 2, "Asset precision of humidity must be set to 2 decimals.  ");

    check(tmin.amount > 0, "Certificate must have a positive value of carbon production. ");
    check(tmin.symbol.code().to_string() == "MT", "Tonne symbol specified is incorrect for this contract, must be MT. ");
    check(tmin.symbol.precision() == 4, "Asset precision must be set to 4 decimals.  ");

    check(tmax.amount > 0, "Certificate must have a positive value of carbon production. ");
    check(tmax.symbol.code().to_string() == "MT", "Tonne symbol specified is incorrect for this contract, must be MT. ");
    check(tmax.symbol.precision() == 4, "Asset precision must be set to 4 decimals.  ");

    check(tavg.amount > 0, "Certificate must have a positive value of carbon production. ");
    check(tavg.symbol.code().to_string() == "MT", "Tonne symbol specified is incorrect for this contract, must be MT. ");
    check(tavg.symbol.precision() == 4, "Asset precision must be set to 4 decimals.  ");

    uint64_t certn = getglobalint(name("csinkcount")) + 1;

    csinkcert_index _csinkcerts( get_self(), get_self().value );
    auto ccert_itr = _csinkcerts.find(certn);

    check(ccert_itr == _csinkcerts.end(), "Contract error, cert number already exists! ");

    tuple<name, uint8_t, uint64_t> tplApr = get_auth_row(creator);
    strctapproval cApprove = strctapproval(creator, get<1>(tplApr), get<2>(tplApr), time_point_sec(current_time_point().sec_since_epoch()));

    _csinkcerts.emplace( get_self(), [&]( auto& cert_row ) {
        cert_row.id = certn;
        cert_row.c  = cert_csink(
                        strctheader(certn, strid, name("csink"), creator, get_org_id(creator), STATUS_CERT_DRAFT),
                        crtcsink(loc, type, desc, gross, humidity, tmin, tmax, tavg, ystart, yend, portid, prodid)
                      );

        cert_row.c.header.add_approval(cApprove);
       
        sMemo = "C-Sink Certificate #" + to_string(certn) + "  was drafted by ", creator.to_string() + " for carbon tons totalling: " + tavg.to_string() + " -- (" + strid + ")";
        print(sMemo);
    });

    setglobalint(name("csinkcount"), certn);
}

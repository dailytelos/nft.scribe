struct crtproduction {
    string cname; //Title / Header string for certificate
    string exid;  //external ID

    asset tpot;  // asset in X.XXXX MT Mformat -- Total C-Sink Potential

    asset tsunk; //asset in X.XXXX MT Mformat -- total sunk
    bool bsunk;  //flag as true when production cert has been fully utilized

    time_point_sec tstart;  //date of certificate start
    time_point_sec tend;    //date of certificate end -- typically 1 year later

    vector<uint64_t> scinks;  //c-sink ids of carbon sinks performed on production cert

    crtproduction() {};

    crtproduction(string i_cname, string i_exid, asset i_tpot){
        cname = i_cname;
        exid  = i_exid;
        tpot  = i_tpot;
    };

    EOSLIB_SERIALIZE(crtproduction, (cname)(exid)(tpot)(tsunk)(bsunk)(tstart)(tend)(scinks));
};

struct crtcsink {

    uint16_t  loc;  //location code ISO 3166:  https://en.wikipedia.org/wiki/ISO_3166-1_numeric
                    //requires private server data as well, for address of sink, contact details too

    string    type; //c-sink type, such as "biochar"
    string    desc; //description provided by user, limit 1000 characters

    //tCO2 measurements for certificate
    asset     gross;    // asset in X.XXXX MT Mformat - raw gross weight in ton
    asset     humidity; // asset in X.XX P format

    asset     tmin;     // asset in X.XXXX MT Mformat
    asset     tmax;     // asset in X.XXXX MT Mformat
    asset     tavg;     // asset in X.XXXX MT Mformat

    uint32_t  ystart;
    uint32_t  yend;     //usually 100 years later

    //tokenized c-sink retirements
    asset     qtyretired;  // asset qty to specify retirement of MT
    bool      retired;     // true when fully retired

    //cross-reference ID's
    uint64_t  portid;  //portfolio id
    uint64_t  prodid;  //production certificate id

    crtcsink() {};

    crtcsink(uint16_t i_loc, string i_type, string i_desc, asset i_gross, asset i_humidity, asset i_tmin, asset i_tmax, asset i_tavg, uint32_t i_ystart, uint32_t i_yend, uint64_t i_portid, uint64_t i_prodid) {
        loc = i_loc;
        type = i_type;
        desc = i_desc;
        gross = i_gross;
        humidity = i_humidity;
        tmin = i_tmin;
        tmax = i_tmax;
        tavg = i_tavg;
        ystart = i_ystart;
        yend = i_yend;
        qtyretired = asset(0, symbol("MT",4));
        retired = false;
        portid = i_portid;
        prodid = i_prodid;
    };

    EOSLIB_SERIALIZE(crtcsink, (loc)(type)(desc)(gross)(humidity)(tmin)(tmax)(tavg)(ystart)(yend)(qtyretired)(retired)(portid)(prodid));
};


struct cert_production {

    carboncert::strctheader header;
    carboncert::crtproduction data;  //production cert data, if it's that type

    cert_production() {};

    cert_production(carboncert::strctheader i_header, carboncert::crtproduction i_data) {
        header = i_header;
        data   = i_data;
    };

    EOSLIB_SERIALIZE(cert_production, (header)(data));
};

struct cert_csink {

    carboncert::strctheader header;
    carboncert::crtcsink data;  //c-sink cert data, if it's that type

    cert_csink() {};

    cert_csink(carboncert::strctheader i_header, carboncert::crtcsink i_data) {
        header = i_header;
        data   = i_data;
    };

    EOSLIB_SERIALIZE(cert_csink, (header)(data));
};



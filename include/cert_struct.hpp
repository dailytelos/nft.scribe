struct crtproduction {
    string cname; //Title / Header string for certificate
    string exid;  //external ID

    asset tpot;  // asset in X.XXXX T format -- Total C-Sink Potential

    asset tsunk; //asset in X.XXXX T format -- total sunk
    bool bsunk;  //flag as true when production cert has been fully utilized

    time_point_sec tstart;  //date of certificate start
    time_point_sec tend;    //date of certificate end -- typically 1 year later

    vector<uint64_t> scinks;  //c-sink ids of carbon sinks performed on production cert

    EOSLIB_SERIALIZE(crtproduction, (cname)(exid)(tpot)(tsunk)(bsunk)(tstart)(tend)(scinks));
};

struct crtcsink {

    uint16_t  loc;  //location code ISO 3166:  https://en.wikipedia.org/wiki/ISO_3166-1_numeric
                    //requires private server data as well, for address of sink, contact details too

    name      type; //c-sink type, such as "biochar"
    string    desc; //description provided by user, limit 1000 characters

    //tCO2 measurements for certificate
    asset     gross;    // asset in X.XXXX T format - raw gross weight in ton
    asset     humidity; // asset in X.XX P format

    asset     tmin;     // asset in X.XXXX T format
    asset     tmax;     // asset in X.XXXX T format
    asset     tavg;     // asset in X.XXXX T format

    uint32_t  ystart;
    uint32_t  yend;    //usually 100 years later

    //tokenized c-sink retirements
    asset     qtyretired;  // asset qty to specify retirement of cert tokens
    bool      retired;     // true when fully retired

    //cross-reference ID's
    uint64_t  portid;  //portfolio id
    uint64_t  prodid;  //production certificate id

    EOSLIB_SERIALIZE(crtcsink, (loc)(type)(desc)(gross)(humidity)(tmin)(tmax)(tavg)(ystart)(yend)(qtyretired)(retired)(portid)(prodid));
};


struct cert_production {

    carboncert::strctheader header;
    carboncert::crtproduction data;  //production cert data, if it's that type

    EOSLIB_SERIALIZE(cert_production, (header)(data));
};

struct cert_csink {

    carboncert::strctheader header;
    carboncert::crtcsink data;  //c-sink cert data, if it's that type

    EOSLIB_SERIALIZE(cert_csink, (header)(data));
};


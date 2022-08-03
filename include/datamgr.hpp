

  public:

    //vector<string> -- Data Definitions
    vector<string> VARDEF_CERT_EBC      = {"s_company","s_company_num","s_cert_num","t_issue","t_expire","a_csink_gross","a_csink_net","a_csink_pers","s_filename"};
    vector<string> VARDEF_CERT_PROD     = {"s_title","u_ebc_certn","a_tproduced","a_tcsunk","a_tissued","s_limit_orgids","a_max_tcsunk","n_yprod","n_mprod"};
    vector<string> VARDEF_CERT_CSNK     = {"u_prod_certn","u_ebc_certn","n_dbid","s_loc","s_type","s_application","a_gross","a_tmin","a_tmax","a_tavg","n_ystart","n_yend","n_claimed","a_qtyretired","n_retired"};
    vector<string> VARDEF_CERT_CSNK_ISS = {"u_prod_certn","u_ebc_certn","n_dbid","s_loc","s_type","s_application","a_gross","a_tmin","a_tmax","a_tavg","n_ystart","n_yend","n_claimed","a_qtyretired","n_retired"};
    vector<string> VARDEF_DATA_PORTF    = {"s_title","s_desc","s_img","a_csinks","a_retired"};
    vector<string> VARDEF_ACT_SEND      = {"s_from","s_to","a_qty","s_memo"};
    vector<string> VARDEF_ACT_RETIRE    = {"a_retired","s_data"};


  // Intitial Data Creation
  ACTION datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id);

  // Submission / Approval / Deletion Updates to Data
  ACTION datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type);

  //Execute the data
  ACTION execute(const name& approver, const name& type, const uint64_t& id);

  ACTION claim(const name& approver, const name& type, const uint64_t& id);

  ACTION retire(const name& approver, const asset& quant);

  ACTION zzdeleterow(const name& type, uint64_t& id);


  private:

  void _datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id);

  void _datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type);

  void _execute(const name& approver, const name& type, const uint64_t& id);

  void _claim(const name& approver, const name& type, const uint64_t& id);

  void _retire(const name& approver, const asset& quant);
  
  struct_data _get_data_by_id(const name& type, uint64_t& id);


    TABLE tbldata {
      uint64_t   id;      //numeric id of activity
      struct_data d;      //cert data structure

      uint64_t primary_key() const { return id; }
      uint128_t by_secondary() const { return ((uint128_t) d.header.orgid << 64) | (uint128_t) id; };
      uint128_t by_tertiary() const { return ((uint128_t) d.header.reforgid << 64) | (uint128_t) id; };
    };
    
    //typedef multi_index<name("data"), tbldata> data_index; 

/*
  typedef eosio::multi_index<
    name("data"),
    tbldata,
    eosio::indexed_by<
      name("secin"),
      eosio::const_mem_fun<
        tbldata,
        uint128_t,
        &tbldata::secondary_key
      >
    >
  >  data_index;
*/

//typedef eosio::multi_index<"mystruct"_n, mystruct, indexed_by<"secondid"_n, const_mem_fun<mystruct, uint64_t, &mystruct::by_id>>, indexed_by<"anotherid"_n, const_mem_fun<mystruct, uint64_t, &mystruct::by_anotherid>>> datastore;

typedef eosio::multi_index<"data"_n, tbldata, eosio::indexed_by<"secid"_n, eosio::const_mem_fun<tbldata, uint128_t, &tbldata::by_secondary>>, eosio::indexed_by<"terid"_n, eosio::const_mem_fun<tbldata, uint128_t, &tbldata::by_tertiary>>> data_index;


//typedef eosio::multi_index<"data"_n, tbldata, eosio::indexed_by<"secid"_n, eosio::const_mem_fun<tbldata, uint128_t, &tbldata::by_secondary>>> data_index;

  // more than 2 index example:  https://developers.eos.io/manuals/eosio.cdt/v1.7/group__multiindex
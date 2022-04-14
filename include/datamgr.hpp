

  public:



    //vector<string> -- Data Definitions
    vector<string> VARDEF_CERT_EBC      = {"s_company","s_company_num","s_cert_num","t_issue","t_expire"};
    vector<string> VARDEF_CERT_PROD     = {"s_title","n_ebc_certn","a_tproduced","a_tissued"};
    vector<string> VARDEF_CERT_CSNK     = {"n_prod_certn","n_port_certn","s_loc","s_type","s_desc","a_gross","a_humidity","a_tmin","a_tmax","a_tavg","n_ystart","n_yend","n_claimed","a_qtyretired","n_retired"};
    vector<string> VARDEF_DATA_PORTF    = {"s_title","s_desc","s_img","a_csinks","a_retired"};
    vector<string> VARDEF_ACT_SEND      = {"s_contract","s_from","s_to","a_qty","s_memo"};

  // Intitial Data Creation
  ACTION datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id);

  // Submission / Approval / Deletion Updates to Data
  ACTION datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type);

  //Execute the data
  ACTION execute(const name& approver, const name& type, const uint64_t& id);

  ACTION claim(const name& approver, const name& type, const uint64_t& id);

  // issued on c-sink
  //ACTION issuecredits(const name& approver, const uint64_t& id);

  // claim on c-sink
  //ACTION claimcredits(const name& approver, const uint64_t& id);

  // retire credits that were issued
  // ACTION retirefunds(const name& sender, const asset& supply);

  private:

  void _datadraft(const name& creator, const name& type, const string& strid, const string& data, const string& token, const uint8_t& edit, uint64_t& id);

  void _datasubmit(const name& approver, const name& type, const uint64_t& id, const string& appr_type);

  void _execute(const name& approver, const name& type, const uint64_t& id);

  void _claim(const name& approver, const name& type, const uint64_t& id);
  
/*
  void isebccertvalid(const uint64_t& certid);

  void _issuecredits(const name& approver, const uint64_t& id);

  void _claimcredits(const name& approver, const uint64_t& id);
*/


  //void _retirefunds(const name& sender, const asset& supply);

    TABLE tbldata {
      uint64_t   id;      //numeric id of activity
      struct_data d;      //cert data structure

      uint64_t primary_key() const { return id; }
    };
    
    typedef multi_index<name("data"), tbldata> data_index; 
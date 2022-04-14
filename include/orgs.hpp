  public:

    ACTION setorg(const name& authuser, const uint64_t& orgid, const string& orgname, const uint8_t& producer, const uint8_t& supplier, const uint8_t& retire);
  
    ACTION setorgcert(const name& authuser, const uint64_t& orgid, const uint64_t& ebcid);

    ACTION delorg(const name& authuser, const uint64_t& orgid);

    
  private:

    void _setorg(const uint64_t& orgid, const string& orgname, const uint8_t& producer, const uint8_t& supplier, const uint8_t& retire);
    void _setorgcert(const uint64_t& orgid, const uint64_t& ebcid);
    uint64_t get_org_cert(const uint64_t& orgid);


    //uint8_t get_status_auth(const name& user, const name& activity, const bool& submit, const bool& approve, const bool& lockstatus, const bool& delstatus);

    uint64_t get_org_id(const name& user);

    uint8_t get_auth_by_org(const name& user, const uint64_t& orgid);

    tuple<name, uint8_t, uint64_t> get_auth_row(const name& user);

    tuple<string, uint64_t, uint8_t, uint8_t, uint8_t> get_org_tuple(const uint64_t& orgid);

    void checkorgexists(const uint64_t& orgid);

    TABLE orgs {
      uint64_t  id;        //org id
      string    orgname;   //string name of organisation such as BLUEFIELD RENEWABLE ENERGY PTE LTD
      uint64_t  ebcid;     //ebc.cert id for active cert for this organisation
      uint8_t   producer;  //producer capability -- Issue Production
      uint8_t   supplier;  //supplier capability -- Issue CSink
      uint8_t   retire;    //retirement capability -- Retire Token
      
      uint64_t primary_key() const { return id; }
    };

    typedef multi_index<name("orgregister"), orgs> orgs_index;



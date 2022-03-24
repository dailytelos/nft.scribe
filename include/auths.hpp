  public:

    ACTION setauthlevel(const name& authuser, const name& user, const uint8_t& level, const uint64_t& org);

    ACTION delauthlevel(const name& authuser, const name& user);

    ACTION setorg(const name& authuser, const uint64_t& orgid, const string& orgname);

    ACTION delorg(const name& authuser, const uint64_t& orgid);
    
  private:
    void _setorg(const uint64_t& orgid, const string& orgname);
    //void min_org_auth(const name& user, const uint8_t& level);
    //void has_org_auth(const name& user, const uint8_t& level);
    uint8_t get_org_auth(const name& user, const uint64_t& orgid);
    uint64_t get_org_id(const name& user);
    tuple<name, uint8_t, uint64_t> get_auth_row(const name& user);

    void checkorgexists(const uint64_t& orgid);

    uint8_t get_status_auth(const name& user, const name& activity, const bool& submit, const bool& approve);

    TABLE auths {
      name      user;    //user account
      uint8_t   level;   //authorisation level assigned to user
      uint64_t  orgid;   //organisation id

      uint64_t primary_key() const { return user.value; }
    };

    typedef multi_index<name("auths"), auths> auths_index;

    TABLE orgs {
      uint64_t  id;        //org id
      string    orgname;   //string name of organisation such as BLUEFIELD RENEWABLE ENERGY PTE LTD
      

      uint64_t primary_key() const { return id; }
    };

    typedef multi_index<name("orgregister"), orgs> orgs_index;

    
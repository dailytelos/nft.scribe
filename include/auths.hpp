  public:

    ACTION setauthlevel(const name& authuser, const name& user, const uint8_t& level, const uint64_t& org);

    ACTION setorg(const name& authuser, const uint64_t& orgid, const string& orgname);

    ACTION delauthlevel(const name& authuser, const name& user);

    ACTION delorg(const name& authuser, const uint64_t& orgid);
    
  private:

    void min_auth(const name& user, const uint8_t& level);
    uint64_t getorgid(const name& user);

    TABLE auths {
      name      user;    //user account
      uint8_t   level;   //authorisation level assigned to user
      uint64_t  org;     //organisation id

      uint64_t primary_key() const { return user.value; }
    };

    typedef multi_index<name("auths"), auths> auths_index;

    TABLE orgs {
      uint64_t  id;      //org id
      string   orgname;   //string name of organisation such as BLUEFIELD RENEWABLE ENERGY PTE LTD
      uint64_t retirenum;  //incrementor tracking number of retirefunds actions issued

      uint64_t primary_key() const { return id; }
    };

    typedef multi_index<name("orgregister"), orgs> orgs_index;

    
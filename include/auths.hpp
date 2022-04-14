  public:

    ACTION setauthlevel(const name& authuser, const name& user, const uint8_t& level, const uint64_t& org);

    ACTION delauthlevel(const name& authuser, const name& user);

    
  private:


    TABLE auths {
      name      user;    //user account
      uint8_t   level;   //authorisation level assigned to user
      uint64_t  orgid;   //organisation id

      uint64_t primary_key() const { return user.value; }
    };

    typedef multi_index<name("auths"), auths> auths_index;

    
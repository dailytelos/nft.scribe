//oracles.hpp
  // oracle 0 tier:  less than 34 mil BP votes, either a past BP or registered through contract multisig
  // oracle 1st tier: >= 34 mil BP votes
  // oracle 2nd tier: >= 54 mil BP votes

  // An oracle of a higher tier may kick the lower tiers, for 1 hour "also resetting active status to 0"

  public:

    //get_self() or a BP who has surpassed the vote threshold
    ACTION orcregister(const name& auth, const name& oracle_id, const name& network_id, const string& apisource);

    //requires the authority of the oracle itself
    ACTION orcstatus(const name& oracle_id, const name& network_id, const int8_t& active);

    ACTION orckick(const name& auth, const name& oracle_id, const name& network_id, const uint32_t& hours);

    ACTION orcban(const name& auth, const name& oracle_id, const name& network_id, const uint8_t& ban);

    ACTION orcrefresh(const name& auth, const name& oracle_id, const name& network_id);

    ACTION orcpost(const name& oracle_id, const struct_post& cPost);

  private:

    void _orcregister(const name& auth, const name& oracle_id, const name& network_id, const string& apisource);

    void _orcstatus(const name& oracle_id, const name& network_id, const int8_t& active);

    void _orckick(const name& auth, const name& oracle_id, const name& network_id, const uint32_t& hours);

    void _orcban(const name& auth, const name& oracle_id, const name& network_id, const uint8_t& ban);

    void _orcrefresh(const name& auth, const name& oracle_id, const name& network_id);

    void _orcpost(const name& oracle_id, const struct_post& cPost);

    struct_oracle get_oracle(const name& oracle_id, const name& network_id);

    uint64_t get_orcbpvotes(const name& oracle_id);

    uint64_t get_orctier(const name& oracle_id);

    uint64_t get_oracle_threshold(const name& network_id);

    // scope: TABLE networks->id
    //  per default: "eth.mainnet" will be the default network determining contract permissions
    TABLE oracles {
        name id;                //name of the Telos Native account setup as an oracle
        struct_oracle oracle;
        
    
        uint64_t primary_key() const { return id.value; }
    };

    typedef multi_index<name("oracles"), oracles> oracle_index;

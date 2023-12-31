//oracles.hpp
  // oracle 0 tier:  less than 34 mil BP votes, either a past BP or registered through contract multisig
  // oracle 1st tier: >= 34 mil BP votes
  // oracle 2nd tier: >= 54 mil BP votes

  // An oracle of a higher tier may kick the lower tiers, for 1 hour "also resetting active status to 0"

  public:

    //Can be called by get_self() "nft.scribe"
    ACTION orcregister(const name& auth, const name& oracle_id, const name& network_id, const string& apisource);

    //can only be called by the oracle "orc1.scribe"
    ACTION orcstatus(const name& oracle_id, const name& network_id, const int8_t& active);

    //Can be called by get_self() "nft.scribe"
    ACTION orckick(const name& auth, const name& oracle_id, const name& network_id, const uint32_t& hours);

    //Can be called by get_self() "nft.scribe"
    ACTION orcban(const name& auth, const name& oracle_id, const name& network_id, const uint8_t& ban);

    //can only be called by the oracle "orc1.scribe"
    ACTION orcrefresh(const name& auth, const name& oracle_id, const name& network_id);

  private:

    void _orcregister(const name& auth, const name& oracle_id, const name& network_id, const string& apisource);

    void _orcstatus(const name& oracle_id, const name& network_id, const int8_t& active);

    void _orckick(const name& auth, const name& oracle_id, const name& network_id, const uint32_t& hours);

    void _orcban(const name& auth, const name& oracle_id, const name& network_id, const uint8_t& ban);

    void _orcrefresh(const name& auth, const name& oracle_id, const name& network_id);

    struct_oracle get_oracle(const name& oracle_id, const name& network_id);

    bool is_oracle(const name& oracle_id, const name& network_id);

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
